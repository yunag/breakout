#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <cstdint>
#include <memory>

#include "shader.hpp"

class Texture2D;

class SpriteRenderer {
public:
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
};
