#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>

#include "breakout_game.hpp"
#include "macro.hpp"
#include "resource_manager.hpp"
#include "shader.hpp"
#include "sprite_renderer.hpp"

BreakoutGame::BreakoutGame(uint32_t width, uint32_t height)
    : m_state(GameState::GAME_ACTIVE), m_width(width), m_height(height) {}

BreakoutGame::~BreakoutGame() { delete m_renderer; }

void BreakoutGame::init() {
  auto shader = ResourceManager::load_shader(
      "sprite", "res/shaders/vert/sprite.glsl", "res/shaders/frag/sprite.glsl");
  glm::mat4 projection =
      glm::ortho(0.0f, static_cast<float>(m_width),
                 static_cast<float>(m_height), 0.0f, -1.0f, 1.0f);
  shader->bind();
  shader->seti("image", 0);
  shader->setmat4f("projection", projection);

  m_renderer = new SpriteRenderer(shader);

  ResourceManager::load_texture("face", "res/textures/awesomeface.png", true);
}

void BreakoutGame::update(float dt) { YU_UNUSED(dt); }

void BreakoutGame::process_input(float dt) { YU_UNUSED(dt); }

void BreakoutGame::render() {
  m_renderer->draw(ResourceManager::texture("face"), glm::vec2(200.0f, 200.0f),
                   glm::vec2(300.0f, 400.0f), 45.0f,
                   glm::vec3(0.0f, 1.0f, 0.0f));
}
