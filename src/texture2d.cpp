#include <glad/glad.h>

#include "breakout/texture2d.hpp"

Texture2D::Texture2D()
    : m_width(0), m_height(0), m_internal_format(GL_RGB),
      m_image_format(GL_RGB), m_wrap_s(GL_REPEAT), m_wrap_t(GL_REPEAT),
      m_filter_min(GL_LINEAR), m_filter_max(GL_LINEAR) {
  glGenTextures(1, &m_id);
}

Texture2D::~Texture2D() { glDeleteTextures(1, &m_id); }

Texture2D &Texture2D::operator=(Texture2D &&texture) {
  glDeleteTextures(1, &m_id);

  m_id = texture.m_id;
  m_width = texture.m_width;
  m_height = texture.m_height;
  m_filter_max = texture.m_filter_max;
  m_filter_min = texture.m_filter_min;
  m_wrap_s = texture.m_wrap_s;
  m_wrap_t = texture.m_wrap_t;
  m_internal_format = texture.m_internal_format;
  m_image_format = texture.m_image_format;
  texture.m_id = 0;
  return *this;
}

Texture2D::Texture2D(Texture2D &&texture)
    : m_id(texture.m_id), m_width(texture.m_width), m_height(texture.m_height),
      m_internal_format(texture.m_internal_format),
      m_image_format(texture.m_image_format), m_wrap_s(texture.m_wrap_s),
      m_wrap_t(texture.m_wrap_t), m_filter_min(texture.m_filter_min),
      m_filter_max(texture.m_filter_max) {
  texture.m_id = 0;
}

void Texture2D::generate(uint32_t width, uint32_t height, unsigned char *data) {
  m_width = width;
  m_height = height;

  /* Create texture */
  glBindTexture(GL_TEXTURE_2D, m_id);
  glTexImage2D(GL_TEXTURE_2D, 0, m_internal_format, m_width, m_height, 0,
               m_image_format, GL_UNSIGNED_BYTE, data);

  /* Set texture's wrap and filter methods */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap_s);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap_t);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_filter_min);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filter_max);

  /* Unbind texture */
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::set_internal_format(uint32_t format) {
  m_internal_format = format;
}

void Texture2D::set_image_format(uint32_t format) { m_image_format = format; }

void Texture2D::bind() const { glBindTexture(GL_TEXTURE_2D, m_id); }
void Texture2D::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

uint32_t Texture2D::id() const { return m_id; }
