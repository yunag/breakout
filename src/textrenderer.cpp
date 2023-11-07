#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include <utility>
#include FT_FREETYPE_H

#include "breakout/text_renderer.hpp"
#include "breakout/shader.hpp"
#include "breakout/log.hpp"

#include "breakout/resource_manager.hpp"

TextRenderer::TextRenderer(uint32_t width, uint32_t height) {
  m_shader = ResourceManager::load_shader(
      "text", "res/shaders/vert/text_2d.glsl", "res/shaders/frag/text_2d.glsl");

  glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f,
                                    static_cast<float>(height));
  m_shader->bind();
  m_shader->setmat4f("projection", projection);
  m_shader->seti("text", 0);

  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

TextRenderer::~TextRenderer() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
}

void TextRenderer::load(const char *path, uint32_t font_size) {
  m_characters.clear();

  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    LOG_CRITICAL("Failed to initialize font library!");
    return;
  }

  FT_Face face;
  if (FT_New_Face(ft, path, 0, &face)) {
    LOG_ERROR("Failed to load font at path: {}", path);
    return;
  }

  FT_Set_Pixel_Sizes(face, 0, font_size);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  for (unsigned char c = 0; c < 128; ++c) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      LOG_WARN("Font library failed to load glyph: {}", c);
      continue;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                 face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                 face->glyph->bitmap.buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_characters.emplace(std::make_pair<char, TextCharacter>(
        c, {texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x}));
  }
  glBindTexture(GL_TEXTURE_2D, 0);

  FT_Done_Face(face);
  FT_Done_FreeType(ft);
}

void TextRenderer::render(const char *text, float x, float y, float scale,
                          glm::vec3 color) {
  m_shader->bind();
  m_shader->setvec3f("textColor", color);

  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(m_vao);

  for (const char *c = text; *c; ++c) {
    auto it = m_characters.find(*c);
    if (it == m_characters.end()) {
      LOG_WARN("Failed to render '{}` character", *c);
      continue;
    }
    TextCharacter ch = it->second;

    float xpos = x + ch.bearing.x * scale;
    float ypos = y - (ch.size.y - ch.bearing.y) * scale;

    float w = ch.size.x * scale;
    float h = ch.size.y * scale;
    float vertices[6][4] = {
        {xpos, ypos + h, 0.0f, 0.0f},     //
        {xpos + w, ypos + h, 1.0f, 0.0f}, //
        {xpos, ypos, 0.0f, 1.0f},         //
        {xpos + w, ypos, 1.0f, 1.0f},     //
    };

    glBindTexture(GL_TEXTURE_2D, ch.texture_id);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // bitshift by 6 to get value in pixels (2^6 = 64)
    x += (ch.advance >> 6) * scale;
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
