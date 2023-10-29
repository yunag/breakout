#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "breakout/shader.hpp"
#include "breakout/log.hpp"

Shader::Shader(const char *vertex_src, const char *fragment_src,
               const char *geometry_src) {
  m_id = glCreateProgram();

  std::vector<GLuint> shaders{
      compile_shader(GL_VERTEX_SHADER, vertex_src),
      compile_shader(GL_FRAGMENT_SHADER, fragment_src),
  };
  if (geometry_src) { // Optional geometry shader
    shaders.push_back(compile_shader(GL_GEOMETRY_SHADER, geometry_src));
  }

  /* Attach GLSL code to the same program */
  for (GLuint shader : shaders) {
    glAttachShader(m_id, shader);
  }
  glLinkProgram(m_id);

  GLint success;
  glGetProgramiv(m_id, GL_LINK_STATUS, &success);
  if (!success) {
    GLchar info_log[1024];
    /* Store Shader errors logs */
    glGetProgramInfoLog(m_id, 1024, NULL, info_log);
    glDeleteProgram(m_id);
    m_id = 0;

    LOG_ERROR(info_log);
  }

  /* Check if current program can execute given the current OpenGL state */
  glValidateProgram(m_id);

  for (GLuint shader : shaders) {
    /* Before removing shaders we must detach them from the shader program,
     * otherwise they will not be removed */
    glDetachShader(m_id, shader);
    /* Remove a shader object from the program object;
     * this does not affect the behavior of the program
     */
    glDeleteShader(shader);
  }
}

Shader::~Shader() { glDeleteProgram(m_id); }

Shader &Shader::operator=(Shader &&shader) {
  glDeleteProgram(m_id);
  m_id = shader.m_id;
  m_location_cache = std::move(shader.m_location_cache);

  shader.m_id = 0;
  return *this;
}

Shader::Shader(Shader &&shader) {
  glDeleteProgram(m_id);
  m_id = shader.m_id;
  m_location_cache = std::move(shader.m_location_cache);

  shader.m_id = 0;
}

GLuint Shader::compile_shader(GLuint shader_type, const char *source) const {
  /* Create an empty shader object of a particular shader type */
  GLuint shader = glCreateShader(shader_type);
  /* Set GLSL code into shader object */
  glShaderSource(shader, 1, &source, NULL);
  /* Compile the shader object with the previously set strings */
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar info_log[1024];
    glGetShaderInfoLog(shader, 1024, NULL, info_log);
    glDeleteShader(shader);
    LOG_ERROR(info_log);
    return 0;
  }
  return shader;
};

void Shader::bind() const { glUseProgram(m_id); }

void Shader::unbind() const { glUseProgram(0); }

void Shader::setb(const char *name, const GLboolean v) {
  glUniform1i(find_uniform(name), (int32_t)v);
}

void Shader::seti(const char *name, const GLint v) {
  glUniform1i(find_uniform(name), v);
}

void Shader::setf(const char *name, const GLfloat v) {
  glUniform1f(find_uniform(name), v);
}

void Shader::set2f(const char *name, const GLfloat v0, const GLfloat v1) {
  glUniform2f(find_uniform(name), v0, v1);
}

void Shader::setvec2f(const char *name, const glm::vec2 &value) {
  glUniform2fv(find_uniform(name), 1, glm::value_ptr(value));
}

void Shader::set3f(const char *name, const GLfloat v0, const GLfloat v1,
                   const GLfloat v2) {
  glUniform3f(find_uniform(name), v0, v1, v2);
}

void Shader::setvec3f(const char *name, const glm::vec3 &value) {
  glUniform3fv(find_uniform(name), 1, glm::value_ptr(value));
}

void Shader::set4f(const char *name, const GLfloat v0, const GLfloat v1,
                   const GLfloat v2, const GLfloat v3) {
  glUniform4f(find_uniform(name), v0, v1, v2, v3);
}

void Shader::setvec4f(const char *name, const glm::vec4 &value) {
  glUniform4fv(find_uniform(name), 1, glm::value_ptr(value));
}

void Shader::setmat2f(const char *name, const glm::mat2 &mat) {

  glUniformMatrix2fv(find_uniform(name), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setmat3f(const char *name, const glm::mat3 &mat) {
  glUniformMatrix3fv(find_uniform(name), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setmat4f(const char *name, const glm::mat4 &mat) {
  glUniformMatrix4fv(find_uniform(name), 1, GL_FALSE, glm::value_ptr(mat));
}

GLuint Shader::id() const { return m_id; }

GLint Shader::find_uniform(const char *name) {
  if (m_location_cache.find(name) != m_location_cache.end()) {
    return m_location_cache.at(name);
  }
  GLint unif_loc = glGetUniformLocation(m_id, name);
  if (unif_loc == -1) {
    LOG_ERROR("Uniform with name `{}` doesn't exist", name);
    return -1;
  }

  m_location_cache[name] = unif_loc;
  return unif_loc;
}
