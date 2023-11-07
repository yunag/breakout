#ifndef YU_POSTPROCESSOR_H
#define YU_POSTPROCESSOR_H

#include <memory>
#include <cstdint>

#include "breakout/texture2d.hpp"

class Shader;

class PostProcessor {
public:
  enum class Effect {
    SHAKE = 0,
    CHAOS,
    CONFUSE,
  };

public:
  PostProcessor(const PostProcessor &) = delete;
  PostProcessor(PostProcessor &&) = delete;
  PostProcessor &operator=(const PostProcessor &) = delete;
  PostProcessor &operator=(PostProcessor &&) = delete;
  PostProcessor(std::shared_ptr<Shader> shader, uint32_t width,
                uint32_t height);
  void begin_render();
  void end_render();
  void render(float time);

  void enable_effect(Effect effect);
  void disable_effect(Effect effect);

  bool is_effect_enabled(Effect effect);

private:
  void init_render_data();
  bool &get_effect_field(Effect effect);

private:
  std::shared_ptr<Shader> m_shader;
  Texture2D m_texture;
  uint32_t m_width, m_height;
  bool m_confuse, m_chaos, m_shake;

  uint32_t m_msfbo, m_fbo;
  uint32_t m_rbo;
  uint32_t m_vao;
};

#endif /* !YU_POSTPROCESSOR_H */
