#include "breakout/gameobject.hpp"
#include "breakout/sprite_renderer.hpp"

GameObject::GameObject()
    : color(1.0f), position(0.0f, 0.0f), size(1.0f, 1.0f), velocity(0.0f),
      rotation(0.0f), is_solid(false), is_destroyed(false), m_sprite() {}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size,
                       std::shared_ptr<Texture2D> sprite, glm::vec3 color,
                       glm::vec2 velocity)
    : color(color), position(pos), size(size), velocity(velocity),
      rotation(0.0f), is_solid(false), is_destroyed(false), m_sprite(sprite) {}

void GameObject::draw(SpriteRenderer &renderer) {
  renderer.draw(*m_sprite, this->position, this->size, this->rotation,
                this->color);
}
