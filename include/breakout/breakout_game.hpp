#ifndef YU_GAME_H
#define YU_GAME_H

#include <cstdint>

class SpriteRenderer;

enum class GameState {
  GAME_ACTIVE = 0,
  GAME_MENU,
  GAME_WIN,
};

class BreakoutGame {
public:
  BreakoutGame(uint32_t width, uint32_t height);
  ~BreakoutGame();

  void init();
  void process_input(float dt);
  void update(float dt);
  void render();

private:
  SpriteRenderer *m_renderer;
  GameState m_state;
  uint32_t m_width, m_height;
};

#endif /* !YU_GAME_H */
