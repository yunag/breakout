#include "breakout/powerup.hpp"

PowerUp::PowerUp(PowerUpType type, glm::vec3 color, float duration,
                 glm::vec2 pos, std::shared_ptr<Texture2D> texture)
    : GameObject(pos, PowerUp::size, texture, color, PowerUp::velocity),
      m_type(type), m_duration(duration), m_activated(false) {}
