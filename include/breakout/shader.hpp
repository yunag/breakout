#ifndef YU_SHADER_H
#define YU_SHADER_H

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/mat4x4.hpp>

#include <string>
#include <unordered_map>

class Shader {
public:
  using LocationCacheMap = std::unordered_map<std::string, int>;

  Shader(const char *vertex_source, const char *fragment_source,
         const char *geometry_source = nullptr);
  Shader(const Shader &) = delete;
  Shader(Shader &&shader);
  Shader &operator=(const Shader &) = delete;
  Shader &operator=(Shader &&shader);
  ~Shader();

  /* use/activate the shader */
  void bind() const;
  void unbind() const;
  uint32_t id() const;
  /* utility uniform functions */
  void setb(const char *name, const uint8_t value);
  void seti(const char *name, const int32_t value);
  void setf(const char *name, const float value);
  void set2f(const char *name, const float v0, const float v1);
  void setvec2f(const char *name, const glm::vec2 &value);
  void set3f(const char *name, const float v0, const float v1, const float v2);
  void setvec3f(const char *name, const glm::vec3 &value);
  void set4f(const char *name, const float v0, const float v1, const float v2,
             const float v3);
  void setvec4f(const char *name, const glm::vec4 &value);
  void setmat2f(const char *name, const glm::mat2 &mat);
  void setmat3f(const char *name, const glm::mat3 &mat);
  void setmat4f(const char *name, const glm::mat4 &mat);

private:
  uint32_t compile_shader(uint32_t e_shader_type, const char *source) const;
  int32_t find_uniform(const char *name);

private:
  uint32_t m_id;
  LocationCacheMap m_location_cache;
};

#endif /* !YU_SHADER_H */
