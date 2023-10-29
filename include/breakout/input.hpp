#ifndef YU_INPUT_H
#define YU_INPUT_H

#include <array>

#include "keys.hpp"

class Input {
public:
  static constexpr size_t KEYS_COUNT =
      static_cast<size_t>(KeyCode::KEY_LAST) + 1;
  static constexpr size_t MOUSE_BUTTON_COUNT =
      static_cast<size_t>(MouseButtonCode::MOUSE_BUTTON_LAST) + 1;

  using KeyArray = std::array<bool, KEYS_COUNT>;
  using MouseArray = std::array<bool, MOUSE_BUTTON_COUNT>;

public:
  static bool is_key_pressed(const KeyCode key_code);
  static void press_key(const KeyCode key_code);
  static void release_key(const KeyCode key_code);

  static bool is_mouse_button_pressed(const MouseButtonCode mb_code);
  static void press_mouse_button(const MouseButtonCode mb_code);
  static void release_mouse_button(const MouseButtonCode mb_code);

private:
  static bool key_is_safe(const KeyCode key_code);

private:
  static KeyArray m_keys_pressed;
  static MouseArray m_mouse_button_pressed;
}; // namespace Input

#endif // !YU_INPUT_H
