#ifndef YU_BALL_OBJECT_H
#define YU_BALL_OBJECT_H

#include <glm/vec2.hpp>

#include <cstdint>
#include <memory>

#include "breakout/gameobject.hpp"

class BallObject : public GameObject {
public:
  static constexpr float INITIAL_RADIUS = 12.5f;
  static constexpr glm::vec2 INITIAL_VELOCITY = glm::vec2(150.0f, 325.0f);

public:
  BallObject();
  BallObject(const BallObject &) = default;
  BallObject(BallObject &&) = delete;
  BallObject &operator=(const BallObject &) = default;
  BallObject &operator=(BallObject &&) = delete;
  BallObject(glm::vec2 pos, float radius, glm::vec2 velocity,
             std::shared_ptr<Texture2D> sprite);
  glm::vec2 move(float dt, uint32_t window_width, uint32_t window_height);
  void reset(glm::vec2 pos, glm::vec2 velocity);

public:
  float radius;
  bool is_stuck;
  bool sticky;
  bool pass_through;
};

#endif /* !YU_BALL_OBJECT_H */
