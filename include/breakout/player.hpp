#ifndef YU_PLAYER_H
#define YU_PLAYER_H

#include <glm/vec2.hpp>

#include "breakout/gameobject.hpp"

class Player : public GameObject {
public:
  static constexpr float INITIAL_VELOCITY = 500.0f;
  static constexpr glm::vec2 INITIAL_SIZE = glm::vec2(100.0f, 20.0f);

public:
  using GameObject::GameObject;
};

#endif /* !YU_PLAYER_H */
