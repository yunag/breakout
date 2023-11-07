#ifndef YU_INPUT_H
#define YU_INPUT_H

#include <array>

#include "breakout/keys.hpp"

class Input {
public:
  struct Key {
    bool pressed;
    bool processed;
  };

  static constexpr size_t KEYS_COUNT =
      static_cast<size_t>(KeyCode::KEY_LAST) + 1;
  static constexpr size_t MOUSE_BUTTON_COUNT =
      static_cast<size_t>(MouseButtonCode::MOUSE_BUTTON_LAST) + 1;

  using KeyArray = std::array<Key, KEYS_COUNT>;
  using MouseArray = std::array<bool, MOUSE_BUTTON_COUNT>;

public:
  Input() = delete;
  Input(const Input &) = delete;
  Input(Input &&) = delete;
  Input &operator=(const Input &) = delete;
  Input &operator=(Input &&) = delete;

  static bool is_key_pressed(const KeyCode key_code);
  static void press_key(const KeyCode key_code);
  static void release_key(const KeyCode key_code);

  static bool is_key_processed(const KeyCode key_code);
  static void key_unset_proccessed(const KeyCode key_code);
  static void key_unset_proccessed_all();

  static bool is_mouse_button_pressed(const MouseButtonCode mb_code);
  static void press_mouse_button(const MouseButtonCode mb_code);
  static void release_mouse_button(const MouseButtonCode mb_code);

private:
  static bool key_is_safe(const KeyCode key_code);

private:
  static KeyArray m_keys;
  static MouseArray m_mouse;
}; // namespace Input

#endif // !YU_INPUT_H
