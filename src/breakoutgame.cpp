#include "glm/geometric.hpp"
#include <GLFW/glfw3.h>

#include <algorithm>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/common.hpp>

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <vector>
#include <memory>
#include <string>

#include "breakout/breakout_game.hpp"
#include "breakout/input.hpp"
#include "breakout/log.hpp"
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

const int NUM_LIVES = 3;

BreakoutGame::BreakoutGame(uint32_t width, uint32_t height)
    : m_lives(NUM_LIVES), m_state(GameState::MENU), m_width(width),
      m_height(height) {}

BreakoutGame::~BreakoutGame() { destroy_audio_engine(); }

static bool load_sound(const char *filename, ma_engine &engine,
                       ma_sound &sound) {
  ma_result result =
      ma_sound_init_from_file(&engine, filename, 0, nullptr, nullptr, &sound);
  if (result != MA_SUCCESS) {
    LOG_ERROR("Failed to load sound: {}", filename);
    return false;
  }
  return true;
}

bool BreakoutGame::init_audio_engine() {
  m_audio_engine = std::make_unique<ma_engine>();
  ma_result result = ma_engine_init(nullptr, m_audio_engine.get());

  if (result != MA_SUCCESS) {
    LOG_CRITICAL("Failed to initialize sound engine");
    return false;
  }

  m_main_theme = std::make_unique<ma_sound>();
  m_paddle_sound = std::make_unique<ma_sound>();
  if (!load_sound("res/audio/breakout.mp3", *m_audio_engine, *m_main_theme)) {
    return false;
  }
  if (!load_sound("res/audio/bleep.mp3", *m_audio_engine, *m_paddle_sound)) {
    return false;
  }

  ma_sound_set_looping(m_main_theme.get(), MA_TRUE);
  ma_sound_set_volume(m_main_theme.get(), 0.3f);
  ma_sound_start(m_main_theme.get());
  return true;
}

void BreakoutGame::destroy_audio_engine() {
  ma_sound_uninit(m_main_theme.get());
  ma_sound_uninit(m_paddle_sound.get());
  ma_engine_uninit(m_audio_engine.get());
}

void BreakoutGame::load_textures() {
  struct TextureInfo {
    const char *name;
    const char *filename;
    bool alpha;
  };

  /* TODO: Use json format to load all necessary data */
  std::vector<TextureInfo> texture_infos = {
      {"background", "res/textures/background.jpg", false},
      {"face", "res/textures/awesomeface.png", true},
      {"block", "res/textures/block.png", false},
      {"block_solid", "res/textures/block_solid.png", false},
      {"paddle", "res/textures/paddle.png", true},
      {"particle", "res/textures/particle.png", true},
      {"powerup_speed", "res/textures/powerup_speed.png", true},
      {"powerup_sticky", "res/textures/powerup_sticky.png", true},
      {"powerup_increase", "res/textures/powerup_increase.png", true},
      {"powerup_confuse", "res/textures/powerup_confuse.png", true},
      {"powerup_chaos", "res/textures/powerup_chaos.png", true},
      {"powerup_passthrough", "res/textures/powerup_passthrough.png", true},
  };

  for (TextureInfo &tinfo : texture_infos) {
    ResourceManager::load_texture(tinfo.name, tinfo.filename, tinfo.alpha);
  }
}

void BreakoutGame::load_shaders() {
  struct ShaderInfo {
    const char *name;
    const char *vert_path;
    const char *frag_path;
    const char *geom_path = nullptr;
  };

  /* TODO: Use json format to load all necessary data */
  std::vector<ShaderInfo> shader_infos = {
      {"sprite", "res/shaders/vert/sprite.glsl",
       "res/shaders/frag/sprite.glsl"},
      {"particle", "res/shaders/vert/particle.glsl",
       "res/shaders/frag/particle.glsl"},
      {"postprocessing", "res/shaders/vert/post_processing.glsl",
       "res/shaders/frag/post_processing.glsl"},
  };
  for (ShaderInfo &sinfo : shader_infos) {
    ResourceManager::load_shader(sinfo.name, sinfo.vert_path, sinfo.frag_path,
                                 sinfo.geom_path);
  }
}

static inline glm::vec2 calc_player_pos(uint32_t window_width) {
  return glm::vec2(window_width / 2.0f - Player::INITIAL_SIZE.x / 2.0f,
                   Player::INITIAL_SIZE.y);
}

static inline glm::vec2 calc_ball_pos(glm::vec2 player_pos) {
  return player_pos +
         glm::vec2(Player::INITIAL_SIZE.x / 2.0f - BallObject::INITIAL_RADIUS,
                   BallObject::INITIAL_RADIUS * 2.0f);
}

static inline bool is_powerup_active(std::vector<PowerUp> &powerups,
                                     PowerUpType type) {
  return powerups.end() != std::find_if(powerups.begin(), powerups.end(),
                                        [type](PowerUp &powerup) {
                                          return powerup.type() == type &&
                                                 powerup.is_activated();
                                        });
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
  init_audio_engine();

  load_shaders();
  load_textures();

  const glm::vec2 player_pos = calc_player_pos(m_width);
  const glm::vec2 ball_pos = calc_ball_pos(player_pos);

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
  shader->unbind();

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

  load_levels();
}

void BreakoutGame::reset_level() {
  for (GameObject &brick : m_levels[m_current_level].bricks()) {
    brick.is_destroyed = false;
  }
  m_powerups.clear();
  m_lives = NUM_LIVES;
}

void BreakoutGame::reset_player() {
  m_player->size = Player::INITIAL_SIZE;
  m_player->position = calc_player_pos(m_width);
  m_player->color = glm::vec3(1.0f);
  m_player->size = Player::INITIAL_SIZE;

  m_ball->reset(calc_ball_pos(m_player->position),
                BallObject::INITIAL_VELOCITY);

  m_postprocessor->disable_effect(PostProcessor::Effect::CHAOS);
  m_postprocessor->disable_effect(PostProcessor::Effect::CONFUSE);

  m_ball->pass_through = false;
  m_ball->sticky = false;
  m_ball->color = glm::vec3(1.0f);
}

void BreakoutGame::update(float dt) {
  m_ball->move(dt, m_width, m_height);
  resolve_collisions();

  const glm::vec2 particle_offset =
      glm::vec2(m_ball->radius / 2.0f, -m_ball->radius);

  m_particles->update(dt, *m_ball, 2, particle_offset);
  update_powerups(dt);

  if (m_shake_time > 0.0f) {
    m_shake_time -= dt;
    if (m_shake_time <= 0.0f) {
      m_postprocessor->disable_effect(PostProcessor::Effect::SHAKE);
    }
  }

  if (m_ball->position.y <= 0) {
    if (--m_lives == 0) {
      reset_level();
      m_state = GameState::MENU;
      m_lives = 3;
    }

    reset_player();
  }

  if (m_state == GameState::ACTIVE &&
      m_levels[m_current_level].is_completed()) {
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
          (m_current_level - 1 + m_levels.size()) % m_levels.size();
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

    m_text_renderer->render(("Lives: " + std::to_string(m_lives)).c_str(), 5.0f,
                            m_height - 30.0f, 1.0f);
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
      {0.0f, 1.0f},
      {1.0f, 0.0f},
      {0.0f, -1.0f},
      {-1.0f, 0.0f},
  };

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
      ma_engine_play_sound(m_audio_engine.get(), "res/audio/solid.wav",
                           nullptr);
      m_shake_time = 0.05f;
      m_postprocessor->enable_effect(PostProcessor::Effect::SHAKE);
    } else {
      ma_engine_play_sound(m_audio_engine.get(), "res/audio/bleep.wav",
                           nullptr);
      box.is_destroyed = true;
      spawn_powerups(box);
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

void BreakoutGame::resolve_poweup_collisions() {
  for (PowerUp &power_up : m_powerups) {
    if (power_up.is_destroyed) {
      continue;
    }
    if (power_up.position.x <= 0) {
      power_up.is_destroyed = true;
    }
    if (check_collision(*m_player, power_up)) {
      ma_engine_play_sound(m_audio_engine.get(), "res/audio/powerup.wav",
                           nullptr);

      activate_powerup(power_up);
      power_up.is_destroyed = true;
      power_up.set_activated(true);
    }
  }
}

void BreakoutGame::resolve_player_collisions() {
  Collision result = check_collision(*m_ball, *m_player);
  if (!m_ball->is_stuck && result.collided) {
    ma_sound_start(m_paddle_sound.get());

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
  resolve_poweup_collisions();
  resolve_player_collisions();
}

static bool roll(uint32_t chance) {
  uint32_t random = rand() % chance;
  return random == 0;
}

void BreakoutGame::spawn_powerups(GameObject &block) {
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
          pinfo.type, pinfo.color, pinfo.duration, block.position,
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

    case PowerUpType::PAD_SIZE_INCREASE:
    case PowerUpType::SPEED:
      break;
    }
  }

  m_powerups.erase(std::remove_if(m_powerups.begin(), m_powerups.end(),
                                  [](PowerUp &powerUp) {
                                    return powerUp.is_destroyed &&
                                           !powerUp.is_activated();
                                  }),
                   m_powerups.end());
}
