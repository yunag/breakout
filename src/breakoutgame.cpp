#include <GLFW/glfw3.h>

#include <algorithm>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/common.hpp>

#include <vector>
#include <memory>
#include <string>

#include "breakout/breakout_game.hpp"
#include "breakout/input.hpp"
#include "breakout/audio.hpp"
#include "breakout/resource_manager.hpp"
#include "breakout/shader.hpp"
#include "breakout/sprite_renderer.hpp"
#include "breakout/particle.hpp"
#include "breakout/gamelevel.hpp"
#include "breakout/ballobject.hpp"
#include "breakout/powerup.hpp"
#include "breakout/postprocessor.hpp"
#include "breakout/text_renderer.hpp"
#include "breakout/player.hpp"

BreakoutGame::BreakoutGame(uint32_t width, uint32_t height)
    : m_lives(Player::INITIAL_NUM_LIVES), m_state(GameState::MENU),
      m_width(width), m_height(height) {}

BreakoutGame::~BreakoutGame() {}

static inline glm::vec2 calc_player_pos(uint32_t window_width) {
  return glm::vec2(window_width / 2.0f - Player::INITIAL_SIZE.x / 2.0f,
                   Player::INITIAL_SIZE.y);
}

static inline glm::vec2 calc_ball_pos(glm::vec2 player_pos) {
  return player_pos +
         glm::vec2(Player::INITIAL_SIZE.x / 2.0f - BallObject::INITIAL_RADIUS,
                   BallObject::INITIAL_RADIUS * 2.0f);
}

static bool is_powerup_active(std::vector<PowerUp> &powerups,
                              PowerUpType type) {
  for (const PowerUp &powerup : powerups) {
    if (powerup.type() == type && powerup.is_activated()) {
      return true;
    }
  }
  return false;
}

void BreakoutGame::load_levels() {
  const std::vector<std::string> levels = {
      "one.level", "two.level", "three.level", "four.level", "five.level",
  };

  for (const std::string &path : levels) {
    GameLevel level;
    level.load(("res/levels/" + path).c_str(), m_height, m_width,
               path != "five.level" ? m_height / 2.0f : m_height / 1.3f);
    m_levels.push_back(std::move(level));
  }
  m_current_level = 0;
}

void BreakoutGame::init() {
  ResourceManager::load_resources();

  std::shared_ptr<Shader> shader = ResourceManager::shader("sprite");
  const glm::mat4 projection =
      glm::ortho(0.0f, static_cast<float>(m_width), 0.0f,
                 static_cast<float>(m_height), -1.0f, 1.0f);
  shader->bind();
  shader->seti("image", 0);
  shader->setmat4f("projection", projection);

  m_renderer = std::make_unique<SpriteRenderer>(shader);

  shader = ResourceManager::shader("particle");
  shader->bind();
  shader->setmat4f("projection", projection);

  const glm::vec2 player_pos = calc_player_pos(m_width);
  const glm::vec2 ball_pos = calc_ball_pos(player_pos);

  m_ball = std::make_unique<BallObject>(ball_pos, BallObject::INITIAL_RADIUS,
                                        BallObject::INITIAL_VELOCITY,
                                        ResourceManager::texture("face"));

  m_player = std::make_unique<Player>(player_pos, Player::INITIAL_SIZE,
                                      ResourceManager::texture("paddle"));
  m_particles = std::make_unique<ParticleGenerator>(
      ResourceManager::shader("particle"), ResourceManager::texture("particle"),
      500);
  m_postprocessor = std::make_unique<PostProcessor>(
      ResourceManager::shader("postprocessing"), m_width, m_height);

  m_text_renderer = std::make_unique<TextRenderer>(m_width, m_height);
  m_text_renderer->load("res/fonts/Anton.ttf", 24);

  m_audio_engine = std::make_unique<AudioEngine>();

  m_main_theme = std::make_unique<Sound>(*m_audio_engine);
  m_main_theme->load("res/audio/breakout.mp3", true, 0.3f);
  m_main_theme->play();

  m_paddle_sound = std::make_unique<Sound>(*m_audio_engine);
  m_paddle_sound->load("res/audio/bleep.mp3");

  load_levels();
}

void BreakoutGame::reset_level() {
  for (GameObject &brick : m_levels[m_current_level].bricks()) {
    brick.is_destroyed = false;
  }
  m_powerups.clear();
  m_lives = Player::INITIAL_NUM_LIVES;
}

void BreakoutGame::reset_player() {
  m_player->size = Player::INITIAL_SIZE;
  m_player->position = calc_player_pos(m_width);
  m_player->color = glm::vec3(1.0f);

  m_ball->reset(calc_ball_pos(m_player->position),
                BallObject::INITIAL_VELOCITY);

  m_postprocessor->disable_effect(PostProcessor::Effect::CHAOS);
  m_postprocessor->disable_effect(PostProcessor::Effect::CONFUSE);
}

void BreakoutGame::update(float dt) {
  m_ball->move(dt, m_width, m_height);

  resolve_collisions();

  const glm::vec2 offset = glm::vec2(m_ball->radius / 2.0f, -m_ball->radius);
  m_particles->update(dt, *m_ball, 2, offset);

  update_powerups(dt);

  m_shake_time -= dt;
  if (m_shake_time <= 0.0f) {
    m_postprocessor->disable_effect(PostProcessor::Effect::SHAKE);
  }

  if (m_ball->position.y <= 0) {
    m_lives -= 1;
    if (m_lives == 0) {
      reset_level();
      m_state = GameState::MENU;
      m_lives = Player::INITIAL_NUM_LIVES;
    }

    reset_player();
  }

  if (m_levels[m_current_level].is_completed()) {
    reset_level();
    reset_player();

    m_postprocessor->enable_effect(PostProcessor::Effect::CHAOS);
    m_state = GameState::WIN;
  }
}

void BreakoutGame::process_input(float dt) {
  switch (m_state) {
  case GameState::ACTIVE: {
    const float velocity = Player::INITIAL_VELOCITY * dt;
    if (Input::is_key_pressed(KeyCode::KEY_A) && m_player->position.x >= 0) {
      m_player->position.x -= velocity;
      if (m_ball->is_stuck) {
        m_ball->position.x -= velocity;
      }
    }

    if (Input::is_key_pressed(KeyCode::KEY_D) &&
        m_player->position.x <= m_width - m_player->size.x) {
      m_player->position.x += velocity;
      if (m_ball->is_stuck) {
        m_ball->position.x += velocity;
      }
    }

    if (Input::is_key_pressed(KeyCode::KEY_SPACE)) {
      m_ball->is_stuck = false;
    }
    break;
  }

  case GameState::MENU: {
    if (Input::is_key_processed(KeyCode::KEY_ENTER)) {
      m_state = GameState::ACTIVE;
      Input::key_unset_proccessed(KeyCode::KEY_ENTER);
    }
    if (Input::is_key_processed(KeyCode::KEY_W)) {
      m_current_level = (m_current_level + 1) % m_levels.size();
      Input::key_unset_proccessed(KeyCode::KEY_W);
    }
    if (Input::is_key_processed(KeyCode::KEY_S)) {
      m_current_level =
          (m_current_level + m_levels.size() - 1) % m_levels.size();
      Input::key_unset_proccessed(KeyCode::KEY_S);
    }
    break;
  }

  case GameState::WIN: {
    if (Input::is_key_processed(KeyCode::KEY_ENTER)) {
      m_postprocessor->disable_effect(PostProcessor::Effect::CHAOS);
      m_state = GameState::MENU;
      Input::key_unset_proccessed(KeyCode::KEY_ENTER);
    }
    break;
  }
  }
}

void BreakoutGame::render() {
  if (m_state == GameState::ACTIVE || m_state == GameState::MENU ||
      m_state == GameState::WIN) {
    m_postprocessor->begin_render();

    m_renderer->draw(*ResourceManager::texture("background"),
                     glm::vec2(0.0f, m_height), glm::vec2(m_width, m_height),
                     0.0f);
    m_levels[m_current_level].draw(*m_renderer);

    m_player->draw(*m_renderer);

    for (PowerUp &powerup : m_powerups) {
      if (!powerup.is_destroyed) {
        powerup.draw(*m_renderer);
      }
    }

    m_particles->draw();
    m_ball->draw(*m_renderer);

    m_postprocessor->end_render();
    m_postprocessor->render(glfwGetTime());

    std::string lives = "Lives: " + std::to_string(m_lives);
    m_text_renderer->render(lives.c_str(), 5.0f, m_height - 30.0f, 1.0f);
  }

  if (m_state == GameState::MENU) {
    m_text_renderer->render("Press ENTER to start", 300.0f, m_height / 2.0f,
                            1.0f);
    m_text_renderer->render("Press W or S to select level", 295.0f,
                            m_height / 2.0f + 30.0f, 0.75);
  }

  if (m_state == GameState::WIN) {
    m_text_renderer->render("You WON!!!", 300.0f, m_height / 2.0f, 1.0,
                            glm::vec3(0.0, 1.0, 0.0));
    m_text_renderer->render("Press ENTER to retry or ESC to quit", 130.0f,
                            m_height / 2.0f + 30.0f, 1.0,
                            glm::vec3(1.0, 1.0, 0.0));
  }
}

glm::vec2 vector_direction(glm::vec2 target) {
  static const glm::vec2 compass[] = {
      {0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, -1.0f}, {-1.0f, 0.0f}};

  float max = 0.0f;
  glm::vec2 direction = glm::vec2(0.0f);

  target = glm::normalize(target);
  for (const glm::vec2 &possible_dir : compass) {
    float dot_product = glm::dot(target, possible_dir);
    if (dot_product > max) {
      max = dot_product;
      direction = possible_dir;
    }
  }
  return direction;
}

Collision check_collision(BallObject &one, GameObject &two) {
  const glm::vec2 center(one.position.x + one.radius,
                         one.position.y - one.radius);
  const glm::vec2 aabb_half_extents(two.size.x / 2.0f, two.size.y / 2.0f);
  const glm::vec2 aabb_center(two.position.x + aabb_half_extents.x,
                              two.position.y - aabb_half_extents.y);

  glm::vec2 difference = center - aabb_center;

  const glm::vec2 clamped =
      glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
  const glm::vec2 closest = aabb_center + clamped;

  difference = closest - center;

  return glm::length(difference) <= one.radius
             ? Collision(true, vector_direction(difference), difference)
             : Collision(false, glm::vec2(0.0f), glm::vec2(0.0f));
}

void BreakoutGame::activate_powerup(PowerUp &powerup) {
  switch (powerup.type()) {
  case PowerUpType::SPEED: {
    m_ball->velocity *= 1.2;
    break;
  }

  case PowerUpType::STICKY: {
    m_ball->sticky = true;
    m_player->color = glm::vec3(1.0f, 0.5f, 1.0f);
    break;
  }

  case PowerUpType::PASS_THROUGH: {
    m_ball->pass_through = true;
    m_ball->color = glm::vec3(1.0f, 0.5f, 0.5f);
    break;
  }

  case PowerUpType::PAD_SIZE_INCREASE: {
    m_player->size.x += 50;
    break;
  }

  case PowerUpType::CONFUSE: {
    if (!m_postprocessor->is_effect_enabled(PostProcessor::Effect::CHAOS)) {
      m_postprocessor->enable_effect(PostProcessor::Effect::CONFUSE);
    }
    break;
  }

  case PowerUpType::CHAOS: {
    if (!m_postprocessor->is_effect_enabled(PostProcessor::Effect::CONFUSE)) {
      m_postprocessor->enable_effect(PostProcessor::Effect::CHAOS);
    }
    break;
  }
  }
}

bool check_collision(const GameObject &one, const GameObject &two) {
  const bool collisionX = one.position.x + one.size.x >= two.position.x &&
                          two.position.x + two.size.x >= one.position.x;

  const bool collisionY = one.position.y - one.size.y <= two.position.y &&
                          two.position.y - two.size.y <= one.position.y;
  return collisionX && collisionY;
}

void BreakoutGame::resolve_box_collisions() {
  for (GameObject &box : m_levels[m_current_level].bricks()) {
    if (box.is_destroyed) {
      continue;
    }

    Collision collision = check_collision(*m_ball, box);
    if (!collision.collided) {
      continue;
    }

    if (box.is_solid) {
      m_audio_engine->play("res/audio/solid.wav");
      m_shake_time = 0.05f;
      m_postprocessor->enable_effect(PostProcessor::Effect::SHAKE);
    } else {
      m_audio_engine->play("res/audio/bleep.wav");
      box.is_destroyed = true;
      spawn_powerups(box.position);
    }

    glm::vec2 dir = collision.direction;
    glm::vec2 diff = collision.difference;
    if (!m_ball->pass_through || box.is_solid) {
      glm::vec2 penetration = diff - dir * m_ball->radius;
      m_ball->velocity = glm::reflect(m_ball->velocity, -1.0f * dir);
      m_ball->position += penetration;
    }
    break;
  }
}

void BreakoutGame::resolve_powerup_collisions() {
  for (PowerUp &power_up : m_powerups) {
    if (power_up.is_destroyed) {
      continue;
    }
    if (power_up.position.x <= 0) {
      power_up.is_destroyed = true;
    }
    if (check_collision(*m_player, power_up)) {
      m_audio_engine->play("res/audio/powerup.wav");

      activate_powerup(power_up);
      power_up.is_destroyed = true;
      power_up.set_activated(true);
    }
  }
}

void BreakoutGame::resolve_player_collisions() {
  Collision result = check_collision(*m_ball, *m_player);
  if (!m_ball->is_stuck && result.collided) {
    m_paddle_sound->play();

    float center_board = m_player->position.x + m_player->size.x / 2.0f;
    float distance = m_ball->position.x + m_ball->radius - center_board;
    float percentage = distance / (m_player->size.x / 2.0f);

    float strength = 2.0f;
    glm::vec2 old_velocity = m_ball->velocity;
    m_ball->velocity.x = BallObject::INITIAL_VELOCITY.x * percentage * strength;
    m_ball->velocity =
        glm::normalize(m_ball->velocity) * glm::length(old_velocity);
    m_ball->velocity.y = std::abs(m_ball->velocity.y);

    m_ball->is_stuck = m_ball->sticky;
  }
}

void BreakoutGame::resolve_collisions() {
  resolve_box_collisions();
  resolve_powerup_collisions();
  resolve_player_collisions();
}

static bool roll(uint32_t chance) {
  uint32_t random = rand() % chance;
  return random == 0;
}

void BreakoutGame::spawn_powerups(glm::vec2 position) {
  struct PowerUpInfo {
    PowerUpType type;
    std::string texture_name;
    glm::vec3 color;
    float duration;
    uint8_t spawn_chance;
  };

  /* TODO: Use json format to load all necessary data */
  std::vector<PowerUpInfo> powerup_info = {
      {PowerUpType::SPEED, "powerup_speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f,
       75},
      {PowerUpType::STICKY, "powerup_sticky", glm::vec3(1.0f, 0.5f, 1.0f),
       20.0f, 75},
      {PowerUpType::PASS_THROUGH, "powerup_passthrough",
       glm::vec3(1.0f, 0.5f, 1.0f), 10.0f, 75},
      {PowerUpType::PAD_SIZE_INCREASE, "powerup_increase",
       glm::vec3(1.0f, 0.6f, 0.4), 0.0f, 75},
      {PowerUpType::CONFUSE, "powerup_confuse", glm::vec3(1.0f, 0.3f, 0.3f),
       15.0f, 15},
      {PowerUpType::CHAOS, "powerup_chaos", glm::vec3(0.9f, 0.25f, 0.25f),
       15.0f, 15},
  };

  for (PowerUpInfo &pinfo : powerup_info) {
    if (roll(pinfo.spawn_chance)) {
      m_powerups.emplace_back(
          pinfo.type, pinfo.color, pinfo.duration, position,
          ResourceManager::texture(pinfo.texture_name.c_str()));
    }
  }
}

void BreakoutGame::update_powerups(float dt) {
  for (PowerUp &powerup : m_powerups) {
    powerup.position += powerup.velocity * dt;
    if (!powerup.is_activated()) {
      continue;
    }
    powerup.set_duration(powerup.duration() - dt);
    if (powerup.duration() > 0.0f) {
      continue;
    }

    powerup.set_activated(false);
    PowerUpType type = powerup.type();
    if (is_powerup_active(m_powerups, type)) {
      continue;
    }

    switch (type) {
    case PowerUpType::STICKY: {
      m_ball->sticky = false;
      m_player->color = glm::vec3(1.0f);
      break;
    }

    case PowerUpType::PASS_THROUGH: {
      m_ball->pass_through = false;
      m_ball->color = glm::vec3(1.0f);
      break;
    }

    case PowerUpType::CONFUSE: {
      m_postprocessor->disable_effect(PostProcessor::Effect::CONFUSE);
      break;
    }

    case PowerUpType::CHAOS: {
      m_postprocessor->disable_effect(PostProcessor::Effect::CHAOS);
      break;
    }
    default:
      break;
    }
  }

  auto remove_iter = std::remove_if(
      m_powerups.begin(), m_powerups.end(), [](const PowerUp &powerUp) -> bool {
        return powerUp.is_destroyed && !powerUp.is_activated();
      });

  m_powerups.erase(remove_iter, m_powerups.end());
}
