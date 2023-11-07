#include <glad/glad.h>

#include "breakout/postprocessor.hpp"
#include "breakout/log.hpp"
#include "breakout/texture2d.hpp"
#include "breakout/shader.hpp"

PostProcessor::PostProcessor(std::shared_ptr<Shader> shader, uint32_t width,
                             uint32_t height)
    : m_shader(shader), m_texture(), m_width(width), m_height(height),
      m_confuse(false), m_chaos(false), m_shake(false) {
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

void PostProcessor::render(float time) {
  m_shader->bind();
  m_shader->setf("time", time);
  m_shader->seti("confuse", m_confuse);
  m_shader->seti("chaos", m_chaos);
  m_shader->seti("shake", m_shake);

  glActiveTexture(GL_TEXTURE0);
  m_texture.bind();
  glBindVertexArray(m_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void PostProcessor::init_render_data() {
  float vertices[] = {
      // pos        // tex
      -1.0f, -1.0f, 0.0f, 0.0f, //
      1.0f, 1.0f, 1.0f, 1.0f,   //
      -1.0f, 1.0f, 0.0f, 1.0f,  //
                                //
      -1.0f, -1.0f, 0.0f, 0.0f, //
      1.0f, -1.0f, 1.0f, 0.0f,  //
      1.0f, 1.0f, 1.0f, 1.0f    //
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

bool &PostProcessor::get_effect_field(Effect effect) {
  switch (effect) {
  case PostProcessor::Effect::SHAKE:
    return m_shake;
  case PostProcessor::Effect::CHAOS:
    return m_chaos;
  case PostProcessor::Effect::CONFUSE:
    return m_confuse;
  }
}

void PostProcessor::enable_effect(Effect effect) {
  get_effect_field(effect) = true;
}
void PostProcessor::disable_effect(Effect effect) {
  get_effect_field(effect) = false;
}

bool PostProcessor::is_effect_enabled(Effect effect) {
  return get_effect_field(effect);
}
