#include <glad/glad.h>
#include <stb_image.h>

#include <memory>

#include "breakout/memory.hpp"
#include "breakout/resource_manager.hpp"
#include "breakout/log.hpp"
#include "breakout/shader.hpp"
#include "breakout/texture2d.hpp"

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
