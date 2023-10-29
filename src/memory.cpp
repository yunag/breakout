#include <fstream>
#include <sstream>

#include "breakout/memory.hpp"
#include "breakout/log.hpp"

namespace Memory {
std::string read_file(const std::string &filepath) {
  std::ifstream file(filepath, std::ios::binary);
  if (!file) {
    LOG_ERROR("Failed to open file at path: {}", filepath);
    return "";
  }
  std::stringstream ss;
  ss << file.rdbuf();
  return ss.str();
}
} // namespace Memory
