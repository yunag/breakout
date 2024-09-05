#include <glad/glad.h>

#include "breakout/postprocessor.hpp"
#include "breakout/log.hpp"
#include "breakout/texture2d.hpp"
#include "breakout/shader.hpp"

PostProcessor::PostProcessor(std::shared_ptr<Shader> shader, uint32_t width,
                             uint32_t height)
    : m_shader(shader), m_texture(), m_width(width), m_height(height) {
  glGenFramebuffers(1, &m_msfbo);
  glGenFramebuffers(1, &m_fbo);
  glGenRenderbuffers(1, &m_rbo);

  glBindFramebuffer(GL_FRAMEBUFFER, m_msfbo);
  glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);

  GLint max_samples;
  glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
  /* Allocate storage for render buffer object */
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, max_samples, GL_RGB, width,
                                   height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                            GL_RENDERBUFFER, m_rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    LOG_ERROR("Failed to initialize MSFBO");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  m_texture.generate(width, height, nullptr);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         m_texture.id(), 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    LOG_ERROR("Failed to initialize FBO");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  init_render_data();
  m_shader->bind();
  m_shader->seti("scene", 0);

  float offset = 1.0f / 300.0f;
  float offsets[9][2] = {
      {-offset, offset},  // top-left
      {0.0f, offset},     // top-center
      {offset, offset},   // top-right
      {-offset, 0.0f},    // center-left
      {0.0f, 0.0f},       // center-center
      {offset, 0.0f},     // center - right
      {-offset, -offset}, // bottom-left
      {0.0f, -offset},    // bottom-center
      {offset, -offset}   // bottom-right
  };

  glUniform2fv(glGetUniformLocation(m_shader->id(), "offsets"), 9,
               (float *)offsets);
  int edge_kernel[9] = {
      -1, -1, -1, //
      -1, 8,  -1, //
      -1, -1, -1, //
  };
  glUniform1iv(glGetUniformLocation(m_shader->id(), "edge_kernel"), 9,
               edge_kernel);

  float blur_kernel[9] = {
      1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f, //
      2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f, //
      1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f, //
  };

  glUniform1fv(glGetUniformLocation(m_shader->id(), "blur_kernel"), 9,
               blur_kernel);
}

void PostProcessor::begin_render() {
  glBindFramebuffer(GL_FRAMEBUFFER, m_msfbo);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::end_render() {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_msfbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
  glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static const char *effect_type_to_str(PostProcessor::Effect effect) {
  switch (effect) {
  case PostProcessor::Effect::SHAKE:
    return "shake";
  case PostProcessor::Effect::CHAOS:
    return "chaos";
  case PostProcessor::Effect::CONFUSE:
    return "confuse";
  }
}

void PostProcessor::render(float time) {
  m_shader->bind();
  m_shader->setf("time", time);

  for (size_t i = 0; i < EFFECTS_COUNT; ++i) {
    m_shader->seti(effect_type_to_str(static_cast<Effect>(i)), m_effects[i]);
  }

  glActiveTexture(GL_TEXTURE0);
  m_texture.bind();

  glBindVertexArray(m_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void PostProcessor::init_render_data() {
  float vertices[] = {
      // pos        // tex
      -1.0f, -1.0f, 0.0f, 0.0f, //
      1.0f,  -1.0f, 1.0f, 0.0f, //
      -1.0f, 1.0f,  0.0f, 1.0f, //
      1.0f,  1.0f,  1.0f, 1.0f  //
  };
  uint32_t vbo;
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(m_vao);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void PostProcessor::enable_effect(Effect effect) {
  m_effects[static_cast<size_t>(effect)] = true;
}
void PostProcessor::disable_effect(Effect effect) {
  m_effects[static_cast<size_t>(effect)] = false;
}

bool PostProcessor::is_effect_enabled(Effect effect) const {
  return m_effects[static_cast<size_t>(effect)];
}
