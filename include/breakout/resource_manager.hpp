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
  using TextureMap = std::unordered_map<std::string, Texture2D>;

  ResourceManager() = delete;
  static std::shared_ptr<Shader> load_shader(const char *name,
                                             const char *vert_path,
                                             const char *frag_path,
                                             const char *geom_path = nullptr);
  static std::shared_ptr<Shader> shader(const char *name);
  static Texture2D &texture(const char *name);
  static Texture2D &load_texture(const char *name, const char *path,
                                 bool alpha);

  /* Deallocate all loaded resources */
  static void clear();

private:
  static Texture2D load_texture_from_file(const char *file, bool alpha);

private:
  static ShaderMap m_shaders;
  static TextureMap m_textures;
};

#endif /* !YU_RESOURCE_MANAGER_H */
