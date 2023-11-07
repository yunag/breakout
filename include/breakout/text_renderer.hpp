#ifndef YU_TEXT_RENDERER
#define YU_TEXT_RENDERER

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <cstdint>
#include <memory>
#include <unordered_map>

class Shader;

struct TextCharacter {
  uint32_t texture_id;
  glm::ivec2 size;
  glm::ivec2 bearing;
  int32_t advance;
};

class TextRenderer {
public:
  using CharMap = std::unordered_map<char, TextCharacter>;

public:
  TextRenderer(const TextRenderer &) = delete;
  TextRenderer(TextRenderer &&) = delete;
  TextRenderer &operator=(const TextRenderer &) = delete;
  TextRenderer &operator=(TextRenderer &&) = delete;
  TextRenderer(uint32_t width, uint32_t height);
  ~TextRenderer();

  void load(const char *path, uint32_t font_size);
  void render(const char *text, float x, float y, float scale,
              glm::vec3 color = glm::vec3(1.0f));

private:
  CharMap m_characters;
  std::shared_ptr<Shader> m_shader;
  uint32_t m_vao;
  uint32_t m_vbo;
};

#endif /* !YU_TEXT_RENDERER */
