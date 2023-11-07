#include <glad/glad.h>

#include "breakout/particle.hpp"
#include "breakout/shader.hpp"
#include "breakout/texture2d.hpp"
#include "breakout/gameobject.hpp"

ParticleGenerator::ParticleGenerator(std::shared_ptr<Shader> shader,
                                     std::shared_ptr<Texture2D> texture,
                                     size_t amount)
    : m_num_particles(amount), m_shader(shader), m_texture(texture) {
  init();
}

ParticleGenerator::~ParticleGenerator() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
}

void ParticleGenerator::update(float dt, GameObject &object,
                               size_t new_particles, glm::vec2 offset) {
  for (size_t i = 0; i < new_particles; ++i) {
    Particle &unused_particle = const_cast<Particle &>(first_unused_particle());
    respawn_particle(unused_particle, object, offset);
  }

  for (Particle &particle : m_particles) {
    particle.life -= dt;
    if (particle.life > 0.0f) {
      particle.pos -= particle.velocity * dt;
      particle.color.a -= dt * 2.5f;
    }
  }
}

void ParticleGenerator::draw() const {
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  m_shader->bind();
  for (const Particle &particle : m_particles) {
    if (particle.life > 0.0f) {
      m_shader->setvec2f("offset", particle.pos);
      m_shader->setvec4f("color", particle.color);
      m_texture->bind();

      glBindVertexArray(m_vao);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      glBindVertexArray(0);
    }
  }

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init() {
  const float particle_quad[] = {
      // pos      // tex
      0.0f, 0.0f,  0.0f, 0.0f, //
      0.0f, -1.0f, 0.0f, 1.0f, //
      1.0f, 0.0f,  1.0f, 0.0f, //
      1.0f, -1.0f, 1.0f, 1.0f, //
  };
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glBindVertexArray(m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  for (size_t i = 0; i < m_num_particles; ++i) {
    m_particles.emplace_back();
  }
}

const Particle &ParticleGenerator::first_unused_particle() const {
  for (const Particle &particle : m_particles) {
    if (particle.life <= 0.0) {
      return particle;
    }
  }
  return m_particles[0];
}

void ParticleGenerator::respawn_particle(Particle &particle, GameObject &object,
                                         glm::vec2 offset) {
  float random = ((rand() % 100) - 50) / 10.0f;
  float r_color = 0.5f + ((rand() % 100) / 100.0f);
  particle.pos = object.position + random + offset;
  particle.color = glm::vec4(r_color, r_color, r_color, 1.0f);
  particle.life = 1.0f;
  particle.velocity = object.velocity * 0.1f;
}
