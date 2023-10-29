#include <glad/glad.h>
#include <memory>
#include <stb_image.h>

#include "breakout/memory.hpp"
#include "breakout/resource_manager.hpp"
#include "breakout/log.hpp"
#include "breakout/shader.hpp"
#include "breakout/texture2d.hpp"

ResourceManager::TextureMap ResourceManager::m_textures;
ResourceManager::ShaderMap ResourceManager::m_shaders;

std::shared_ptr<Shader> ResourceManager::load_shader(const char *name,
                                                     const char *vert_path,
                                                     const char *frag_path,
                                                     const char *geom_path) {
  return m_shaders[name] = std::make_shared<Shader>(Shader(
             Memory::read_file(vert_path).c_str(),
             Memory::read_file(frag_path).c_str(),
             geom_path ? Memory::read_file(geom_path).c_str() : nullptr));
}

std::shared_ptr<Shader> ResourceManager::shader(const char *name) {
  return m_shaders.at(name);
}

Texture2D &ResourceManager::load_texture(const char *name, const char *path,
                                         bool alpha) {
  return m_textures[name] = load_texture_from_file(path, alpha);
}

Texture2D &ResourceManager::texture(const char *name) {
  return m_textures.at(name);
}

void ResourceManager::clear() {
  m_shaders.clear();
  m_textures.clear();
}

Texture2D ResourceManager::load_texture_from_file(const char *path,
                                                  bool alpha) {
  Texture2D texture;
  if (alpha) {
    texture.set_internal_format(GL_RGBA);
    texture.set_image_format(GL_RGBA);
  }

  int width, height, nrChannels;
  unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
  if (!data) {
    LOG_ERROR("Failed to load texture at '{}'", path);
    return texture;
  }

  texture.generate(width, height, data);

  stbi_image_free(data);
  return texture;
}
