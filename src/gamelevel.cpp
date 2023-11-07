#include <glm/vec2.hpp>

#include <sstream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>

#include "breakout/gamelevel.hpp"
#include "breakout/gameobject.hpp"
#include "breakout/log.hpp"
#include "breakout/resource_manager.hpp"

void GameLevel::init(TileData tile_data, uint32_t window_height,
                     uint32_t level_width, uint32_t level_height) {
  /* Calculate dimensions */
  uint32_t height = tile_data.size();
  uint32_t width = tile_data[0].size();
  float unit_width = level_width / static_cast<float>(width);
  float unit_height = level_height / static_cast<float>(height);

  /* Initialize level tiles based on tile data */
  for (uint32_t y = 0; y < height; ++y) {
    for (uint32_t x = 0; x < width; ++x) {
      glm::vec2 pos(unit_width * x, window_height - unit_height * y);
      glm::vec2 size(unit_width, unit_height);
      BlockType block_type = static_cast<BlockType>(tile_data[y][x]);
      if (block_type == BlockType::NOBLOCK) {
        continue;
      }

      if (block_type == BlockType::SOLID) {
        m_bricks.emplace_back(pos, size,
                              ResourceManager::texture("block_solid")),
            glm::vec3(0.8f, 0.8f, 0.7f);
        m_bricks.back().is_solid = true;
      } else {
        std::unordered_map<BlockType, glm::vec3> color_map = {
            {BlockType::BLUE, glm::vec3(0.2f, 0.6f, 1.0f)},
            {BlockType::GREEN, glm::vec3(0.0f, 0.7f, 0.0f)},
            {BlockType::YELLOW, glm::vec3(0.8f, 0.8f, 0.4f)},
            {BlockType::ORANGE, glm::vec3(1.0f, 0.5f, 0.0f)},
        };
        glm::vec3 color = color_map[block_type];
        m_bricks.emplace_back(pos, size, ResourceManager::texture("block"),
                              color);
      }
    }
  }
}

void GameLevel::draw(SpriteRenderer &renderer) {
  for (GameObject &tile : m_bricks) {
    if (!tile.is_destroyed) {
      tile.draw(renderer);
    }
  }
}

bool GameLevel::is_completed() const {
  for (const GameObject &tile : m_bricks) {
    if (!tile.is_solid && !tile.is_destroyed) {
      return false;
    }
  }
  return true;
}

void GameLevel::load(const char *path, uint32_t window_height,
                     uint32_t level_width, uint32_t level_height) {
  m_bricks.clear();

  GameLevel level;
  TileData tile_data;
  uint32_t tile_code;

  std::string line;
  std::ifstream file(path);

  if (!file) {
    LOG_ERROR("Failed to load level at path: {}", path);
    return;
  }

  while (std::getline(file, line)) {
    std::istringstream sstream(line);
    std::vector<uint32_t> row;
    while (sstream >> tile_code) {
      row.push_back(tile_code);
    }
    tile_data.push_back(std::move(row));
  }
  if (tile_data.size() > 0) {
    init(std::move(tile_data), window_height, level_width, level_height);
  }
}

std::vector<GameObject> &GameLevel::bricks() { return m_bricks; }
