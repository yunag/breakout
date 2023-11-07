#ifndef YU_GAMELEVEL_H
#define YU_GAMELEVEL_H

#include <vector>
#include <cstdint>

#include "breakout/gameobject.hpp"

class SpriteRenderer;

enum class BlockType { NOBLOCK = 0, SOLID, BLUE, GREEN, YELLOW, ORANGE };

class GameLevel {
public:
  using TileData = std::vector<std::vector<uint32_t>>;

public:
  GameLevel(){};
  GameLevel(const GameLevel &) = default;
  GameLevel(GameLevel &&) = default;
  GameLevel &operator=(const GameLevel &) = default;
  GameLevel &operator=(GameLevel &&) = default;
  ~GameLevel(){};

  void load(const char *path, uint32_t window_height, uint32_t level_width,
            uint32_t level_height);
  void draw(SpriteRenderer &renderer);
  bool is_completed() const;
  std::vector<GameObject> &bricks();

private:
  void init(TileData tile_data, uint32_t window_height, uint32_t level_width,
            uint32_t level_height);

private:
  std::vector<GameObject> m_bricks;
};

#endif /* !YU_GAMELEVEL_H */
