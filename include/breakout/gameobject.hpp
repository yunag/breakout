#ifndef YU_GAMEOBJECT_H
#define YU_GAMEOBJECT_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <memory>

#include "breakout/texture2d.hpp"

class SpriteRenderer;

class GameObject {

public:
  GameObject();
  GameObject(const GameObject &) = default;
  GameObject(GameObject &&) = default;
  GameObject &operator=(const GameObject &) = default;
  GameObject &operator=(GameObject &&) = default;
  GameObject(glm::vec2 pos, glm::vec2 size, std::shared_ptr<Texture2D> sprite,
             glm::vec3 color = glm::vec3(1.0f),
             glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
  virtual ~GameObject() {}

  virtual void draw(SpriteRenderer &renderer);

public:
  glm::vec3 color;
  glm::vec2 position;
  glm::vec2 size;
  glm::vec2 velocity;

  float rotation;
  bool is_solid;
  bool is_destroyed;

protected:
  std::shared_ptr<Texture2D> m_sprite;
};

#endif /* !YU_GAMEOBJECT_H */
