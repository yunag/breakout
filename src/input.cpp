#include <array>

#include "breakout/input.hpp"

Input::KeyArray Input::m_keys_pressed = {false};
Input::MouseArray Input::m_mouse_button_pressed = {false};

bool Input::is_key_pressed(const KeyCode key_code) {
  if (!key_is_safe(key_code)) {
    return false;
  }
  return m_keys_pressed[static_cast<size_t>(key_code)];
}

void Input::press_key(const KeyCode key_code) {
  if (!key_is_safe(key_code)) {
    return;
  }
  m_keys_pressed[static_cast<size_t>(key_code)] = true;
}

void Input::release_key(const KeyCode key_code) {
  if (!key_is_safe(key_code)) {
    return;
  }
  m_keys_pressed[static_cast<size_t>(key_code)] = false;
}

bool Input::is_mouse_button_pressed(const MouseButtonCode mb_code) {
  return m_mouse_button_pressed[static_cast<size_t>(mb_code)];
}

void Input::press_mouse_button(const MouseButtonCode mb_code) {
  m_mouse_button_pressed[static_cast<size_t>(mb_code)] = true;
}
void Input::release_mouse_button(const MouseButtonCode mb_code) {
  m_mouse_button_pressed[static_cast<size_t>(mb_code)] = false;
}

bool Input::key_is_safe(const KeyCode key_code) {
  return static_cast<size_t>(key_code) < KEYS_COUNT;
}
