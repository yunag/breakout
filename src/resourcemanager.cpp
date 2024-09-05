#include <glad/glad.h>
#include <stb_image.h>

#include <memory>

#include "breakout/memory.hpp"
#include "breakout/resource_manager.hpp"
#include "breakout/log.hpp"
#include "breakout/shader.hpp"
#include "breakout/texture2d.hpp"

void ResourceManager::load_resources_impl() {
  /* TODO: Use json format to load all necessary data */

  struct TextureInfo {
    const char *name;
    const char *filename;
    bool alpha;
  };

  std::vector<TextureInfo> texture_infos = {
      {"background", "res/textures/background.jpg", false},
      {"face", "res/textures/awesomeface.png", true},
      {"block", "res/textures/block.png", false},
      {"block_solid", "res/textures/block_solid.png", false},
      {"paddle", "res/textures/paddle.png", true},
      {"particle", "res/textures/particle.png", true},
      {"powerup_speed", "res/textures/powerup_speed.png", true},
      {"powerup_sticky", "res/textures/powerup_sticky.png", true},
      {"powerup_increase", "res/textures/powerup_increase.png", true},
      {"powerup_confuse", "res/textures/powerup_confuse.png", true},
      {"powerup_chaos", "res/textures/powerup_chaos.png", true},
      {"powerup_passthrough", "res/textures/powerup_passthrough.png", true},
  };

  for (TextureInfo &tinfo : texture_infos) {
    ResourceManager::load_texture(tinfo.name, tinfo.filename, tinfo.alpha);
  }

  struct ShaderInfo {
    const char *name;
    const char *vert_path;
    const char *frag_path;
    const char *geom_path = nullptr;
  };

  std::vector<ShaderInfo> shader_infos = {
      {"sprite", "res/shaders/vert/sprite.glsl",
       "res/shaders/frag/sprite.glsl"},
      {"particle", "res/shaders/vert/particle.glsl",
       "res/shaders/frag/particle.glsl"},
      {"postprocessing", "res/shaders/vert/post_processing.glsl",
       "res/shaders/frag/post_processing.glsl"},
  };

  for (ShaderInfo &sinfo : shader_infos) {
    ResourceManager::load_shader(sinfo.name, sinfo.vert_path, sinfo.frag_path,
                                 sinfo.geom_path);
  }
}

std::shared_ptr<Shader>
ResourceManager::load_shader_impl(const char *name, const char *vert_path,
                                  const char *frag_path,
                                  const char *geom_path) {
  return m_shaders[name] = std::make_shared<Shader>(
             Memory::read_file(vert_path).c_str(),
             Memory::read_file(frag_path).c_str(),
             geom_path ? Memory::read_file(geom_path).c_str() : nullptr);
}

std::shared_ptr<Shader> ResourceManager::shader_impl(const char *name) {
  auto it = m_shaders.find(name);
  if (it != m_shaders.end()) {
    return it->second;
  }
  LOG_ERROR("Shader with name `{}` doesn't exist", name);
  return nullptr;
}

std::shared_ptr<Texture2D> ResourceManager::load_texture_impl(const char *name,
                                                              const char *path,
                                                              bool alpha) {
  return m_textures[name] = load_texture_from_file(path, alpha);
}

std::shared_ptr<Texture2D> ResourceManager::texture_impl(const char *name) {
  auto it = m_textures.find(name);
  if (it != m_textures.end()) {
    return it->second;
  }
  LOG_ERROR("Texture with name `{}` doesn't exist", name);
  return nullptr;
}

void ResourceManager::clear_impl() {
  m_shaders.clear();
  m_textures.clear();
}

std::shared_ptr<Texture2D>
ResourceManager::load_texture_from_file(const char *path, bool alpha) {
  std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>();
  if (alpha) {
    texture->set_internal_format(GL_RGBA);
    texture->set_image_format(GL_RGBA);
  }

  int width, height, nrChannels;
  unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
  if (!data) {
    LOG_ERROR("Failed to load texture at path: {}", path);
    return texture;
  }

  texture->generate(width, height, data);

  stbi_image_free(data);
  return texture;
}
