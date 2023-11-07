#ifndef YU_PARTICLE_H
#define YU_PARTICLE_H

#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

#include <cstdint>
#include <memory>
#include <vector>

class Shader;
class Texture2D;
class GameObject;

struct Particle {
  Particle() : color(1.0f), pos(0.0f), velocity(0.0f), life(0.0f) {}
  glm::vec4 color;
  glm::vec2 pos, velocity;
  float life;
};

class ParticleGenerator {
public:
  ParticleGenerator(const ParticleGenerator &) = delete;
  ParticleGenerator(ParticleGenerator &&) = delete;
  ParticleGenerator &operator=(const ParticleGenerator &) = delete;
  ParticleGenerator &operator=(ParticleGenerator &&) = delete;
  ParticleGenerator(std::shared_ptr<Shader> shader,
                    std::shared_ptr<Texture2D> texture, size_t amount);
  ~ParticleGenerator();
  void update(float dt, GameObject &object, size_t new_particles,
              glm::vec2 offset = glm::vec2(0.0f));
  void draw() const;

private:
  void init();
  const Particle &first_unused_particle() const;
  void respawn_particle(Particle &particle, GameObject &object,
                        glm::vec2 offset = glm::vec2(0.0f));

private:
  std::vector<Particle> m_particles;
  size_t m_num_particles;

  std::shared_ptr<Shader> m_shader;
  std::shared_ptr<Texture2D> m_texture;

  uint32_t m_vao;
  uint32_t m_vbo;
};

#endif /* !YU_PARTICLE_H */
