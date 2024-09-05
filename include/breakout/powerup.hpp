#ifndef YU_POWERUP_H
#define YU_POWERUP_H

#include <memory>

#include "breakout/gameobject.hpp"

class Texture2D;

enum class PowerUpType {
  SPEED,
  STICKY,
  PASS_THROUGH,
  PAD_SIZE_INCREASE,
  CONFUSE,
  CHAOS
};

class PowerUp : public GameObject {
public:
  static constexpr glm::vec2 size = glm::vec2(60.0f, 20.0f);
  static constexpr glm::vec2 velocity = glm::vec2(0.0f, -150.0f);

public:
  PowerUp(const PowerUp &) = default;
  PowerUp(PowerUp &&) = default;
  PowerUp &operator=(const PowerUp &) = default;
  PowerUp &operator=(PowerUp &&) = default;
  PowerUp(PowerUpType type, glm::vec3 color, float duration, glm::vec2 pos,
          std::shared_ptr<Texture2D> texture);
  void set_activated(bool is_powerup_activated) {
    m_activated = is_powerup_activated;
  }
  bool is_activated() const { return m_activated; }

  void set_duration(float duration) { m_duration = duration; }
  float duration() const { return m_duration; }

  PowerUpType type() const { return m_type; }

private:
  PowerUpType m_type;
  float m_duration;
  bool m_activated;
};

#endif /* !YU_POWERUP_H */
