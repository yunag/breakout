#include <glad/glad.h>

#include <glm/ext/matrix_transform.hpp>
#include <memory>

#include "breakout/shader.hpp"
#include "breakout/sprite_renderer.hpp"
#include "breakout/texture2d.hpp"

SpriteRenderer::SpriteRenderer(std::shared_ptr<Shader> shader) {
  m_shader = shader;
  init_render_data();
}

SpriteRenderer::~SpriteRenderer() {
  glDeleteVertexArrays(1, &m_quad_vao);
  glDeleteBuffers(1, &m_quad_vbo);
}

void SpriteRenderer::draw(Texture2D &texture, glm::vec2 position,
                          glm::vec2 size, float rotate, glm::vec3 color) {
  glm::mat4 model =
      glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)) *
      glm::translate(glm::mat4(1.0f),
                     glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)) *
      glm::rotate(glm::mat4(1.0f), glm::radians(rotate),
                  glm::vec3(0.0f, 0.0f, 1.0f)) *
      glm::translate(glm::mat4(1.0f),
                     glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)) *
      glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

  m_shader->bind();
  m_shader->setmat4f("model", model);
  m_shader->setvec3f("spriteColor", color);

  glActiveTexture(GL_TEXTURE0);
  texture.bind();

  glBindVertexArray(m_quad_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void SpriteRenderer::init_render_data() {
  float vertices[] = {
      // pos      // tex
      0.0f, 0.0f,  0.0f, 0.0f, //
      0.0f, -1.0f, 0.0f, 1.0f, //
      1.0f, 0.0f,  1.0f, 0.0f, //
      1.0f, -1.0f, 1.0f, 1.0f, //
  };
  glGenVertexArrays(1, &m_quad_vao);
  glGenBuffers(1, &m_quad_vbo);

  glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(m_quad_vao);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
