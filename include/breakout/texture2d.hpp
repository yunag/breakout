#ifndef YU_TEXTURE_H
#define YU_TEXTURE_H

#include <cstdint>

class Texture2D {
public:
  Texture2D();
  Texture2D(const Texture2D &) = delete;
  Texture2D(Texture2D &&);
  Texture2D &operator=(const Texture2D &) = delete;
  Texture2D &operator=(Texture2D &&);
  ~Texture2D();

  void generate(uint32_t width, uint32_t height, unsigned char *data);
  void bind() const;
  void set_internal_format(uint32_t format);
  void set_image_format(uint32_t format);

  void unbind() const;
  uint32_t id() const;

private:
  uint32_t m_id;
  uint32_t m_width, m_height;
  uint32_t m_internal_format;
  uint32_t m_image_format;

  uint32_t m_wrap_s, m_wrap_t;
  uint32_t m_filter_min, m_filter_max;
};

#endif /* !YU_TEXTURE_H */
