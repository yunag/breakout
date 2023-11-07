#include "breakout/ballobject.hpp"

#include "breakout/texture2d.hpp"

BallObject::BallObject() : GameObject(), radius(12.5f), is_stuck(true) {}

BallObject::BallObject(glm::vec2 pos, float _radius, glm::vec2 _velocity,
                       std::shared_ptr<Texture2D> sprite)
    : GameObject(pos, glm::vec2(_radius * 2.0f, _radius * 2.0f), sprite,
                 glm::vec3(1.0f), _velocity),
      radius(_radius), is_stuck(true), sticky(false), pass_through(false) {}

glm::vec2 BallObject::move(float dt, uint32_t window_width,
                           uint32_t window_height) {
  if (this->is_stuck) {
    return this->position;
  }

  this->position += this->velocity * dt;

  if (position.x <= 0.0f) {
    this->velocity.x = -this->velocity.x;
    this->position.x = 0.0f;
  } else if (this->position.x + this->size.x >= window_width) {
    this->velocity.x = -this->velocity.x;
    this->position.x = window_width - this->size.x;
  }

  if (this->position.y >= window_height) {
    this->velocity.y = -this->velocity.y;
    this->position.y = window_height;
  }
  return this->position;
}

void BallObject::reset(glm::vec2 pos, glm::vec2 velocity) {
  this->position = pos;
  this->velocity = velocity;

  this->is_stuck = true;
  this->sticky = false;
  this->pass_through = false;
}
