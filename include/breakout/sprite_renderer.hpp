#ifndef YU_SPRITE_RENDERER_H
#define YU_SPRITE_RENDERER_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <cstdint>
#include <memory>

class Texture2D;
class Shader;

class SpriteRenderer {
public:
  SpriteRenderer(const SpriteRenderer &) = delete;
  SpriteRenderer(SpriteRenderer &&) = delete;
  SpriteRenderer &operator=(const SpriteRenderer &) = delete;
  SpriteRenderer &operator=(SpriteRenderer &&) = delete;
  SpriteRenderer(std::shared_ptr<Shader> shader);
  ~SpriteRenderer();

  void draw(Texture2D &texture, glm::vec2 position,
            glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f,
            glm::vec3 color = glm::vec3(1.0f));

private:
  void init_render_data();

private:
  std::shared_ptr<Shader> m_shader;
  uint32_t m_quad_vao;
  uint32_t m_quad_vbo;
};

#endif /* !YU_SPRITE_RENDERER_H */
