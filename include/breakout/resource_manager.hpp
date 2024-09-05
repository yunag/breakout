#ifndef YU_RESOURCE_MANAGER_H
#define YU_RESOURCE_MANAGER_H

#include <unordered_map>
#include <string>

#include <memory>

class Texture2D;
class Shader;

class ResourceManager {
public:
  using ShaderMap = std::unordered_map<std::string, std::shared_ptr<Shader>>;
  using TextureMap =
      std::unordered_map<std::string, std::shared_ptr<Texture2D>>;

public:
  ResourceManager(const ResourceManager &) = delete;
  ResourceManager(ResourceManager &&) = delete;
  ResourceManager &operator=(const ResourceManager &) = delete;
  ResourceManager &operator=(ResourceManager &&) = delete;

  static void load_resources() {
    return ResourceManager::get().load_resources_impl();
  }
  static ResourceManager &get() {
    static ResourceManager rmanager;
    return rmanager;
  };

  static std::shared_ptr<Shader> load_shader(const char *name,
                                             const char *vert_path,
                                             const char *frag_path,
                                             const char *geom_path = nullptr) {
    return ResourceManager::get().load_shader_impl(name, vert_path, frag_path,
                                                   geom_path);
  }
  static std::shared_ptr<Shader> shader(const char *name) {
    return ResourceManager::get().shader_impl(name);
  }
  static std::shared_ptr<Texture2D> texture(const char *name) {
    return ResourceManager::get().texture_impl(name);
  }
  static std::shared_ptr<Texture2D> load_texture(const char *name,
                                                 const char *path, bool alpha) {
    return ResourceManager::get().load_texture_impl(name, path, alpha);
  }
  static void clear() { return ResourceManager::get().clear_impl(); }

private:
  ResourceManager() {}

  void load_resources_impl();
  std::shared_ptr<Shader> load_shader_impl(const char *name,
                                           const char *vert_path,
                                           const char *frag_path,
                                           const char *geom_path = nullptr);
  std::shared_ptr<Shader> shader_impl(const char *name);
  std::shared_ptr<Texture2D> texture_impl(const char *name);
  std::shared_ptr<Texture2D> load_texture_impl(const char *name,
                                               const char *path, bool alpha);
  void clear_impl();

  std::shared_ptr<Texture2D> load_texture_from_file(const char *file,
                                                    bool alpha);

private:
  ShaderMap m_shaders;
  TextureMap m_textures;
};

#endif /* !YU_RESOURCE_MANAGER_H */
