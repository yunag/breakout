#ifndef YU_GAME_H
#define YU_GAME_H

#include <cstdint>
#include <memory>
#include <vector>

#include <glm/vec2.hpp>

/* Forward declarations */
class ParticleGenerator;
class SpriteRenderer;
class GameLevel;
class GameObject;
class BallObject;
class PostProcessor;
class PowerUp;
class TextRenderer;
class Player;
class AudioEngine;
class Sound;

enum class GameState {
  ACTIVE = 0,
  MENU,
  WIN,
};

struct Collision {
  Collision(bool _colided, glm::vec2 _direction, glm::vec2 _difference)
      : difference(_difference), direction(_direction), collided(_colided) {}
  glm::vec2 difference;
  glm::vec2 direction;
  bool collided;
};

class BreakoutGame {
public:
  BreakoutGame(const BreakoutGame &) = delete;
  BreakoutGame(BreakoutGame &&) = delete;
  BreakoutGame &operator=(const BreakoutGame &) = delete;
  BreakoutGame &operator=(BreakoutGame &&) = delete;
  BreakoutGame(uint32_t width, uint32_t height);
  ~BreakoutGame();

  void init();
  void process_input(float dt);
  void update(float dt);
  void render();

private:
  void load_levels();

  void spawn_powerups(glm::vec2 position);
  void update_powerups(float dt);

  void resolve_collisions();
  void resolve_box_collisions();
  void resolve_powerup_collisions();
  void resolve_player_collisions();

  void activate_powerup(PowerUp &power_up);
  void reset_player();
  void reset_level();

private:
  std::vector<GameLevel> m_levels;
  std::vector<PowerUp> m_powerups;
  size_t m_current_level;

  int32_t m_lives;

  std::unique_ptr<SpriteRenderer> m_renderer;
  std::unique_ptr<ParticleGenerator> m_particles;
  std::unique_ptr<PostProcessor> m_postprocessor;
  std::unique_ptr<TextRenderer> m_text_renderer;

  std::unique_ptr<Player> m_player;
  std::unique_ptr<BallObject> m_ball;

  std::unique_ptr<AudioEngine> m_audio_engine;
  std::unique_ptr<Sound> m_main_theme;
  std::unique_ptr<Sound> m_paddle_sound;

  float m_shake_time = 0;

  GameState m_state;
  uint32_t m_width, m_height;
};

#endif /* !YU_GAME_H */
