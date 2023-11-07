#include <cstddef>

#include "breakout/input.hpp"

Input::KeyArray Input::m_keys = {{{false, false}}};
Input::MouseArray Input::m_mouse = {false};

bool Input::is_key_pressed(const KeyCode key_code) {
  if (!key_is_safe(key_code)) {
    return false;
  }
  return m_keys[static_cast<size_t>(key_code)].pressed;
}

void Input::press_key(const KeyCode key_code) {
  if (!key_is_safe(key_code)) {
    return;
  }
  m_keys[static_cast<size_t>(key_code)].pressed = true;
}

bool Input::is_key_processed(const KeyCode key_code) {
  if (!key_is_safe(key_code)) {
    return false;
  }
  return m_keys[static_cast<size_t>(key_code)].processed;
}

void Input::key_unset_proccessed_all() {
  for (Key key : m_keys) {
    key.processed = false;
  }
}

void Input::release_key(const KeyCode key_code) {
  if (!key_is_safe(key_code)) {
    return;
  }
  m_keys[static_cast<size_t>(key_code)].pressed = false;
  m_keys[static_cast<size_t>(key_code)].processed = true;
}

void Input::key_unset_proccessed(const KeyCode key_code) {
  if (!key_is_safe(key_code)) {
    return;
  }
  m_keys[static_cast<size_t>(key_code)].processed = false;
}

bool Input::is_mouse_button_pressed(const MouseButtonCode mb_code) {
  return m_mouse[static_cast<size_t>(mb_code)];
}

void Input::press_mouse_button(const MouseButtonCode mb_code) {
  m_mouse[static_cast<size_t>(mb_code)] = true;
}
void Input::release_mouse_button(const MouseButtonCode mb_code) {
  m_mouse[static_cast<size_t>(mb_code)] = false;
}

bool Input::key_is_safe(const KeyCode key_code) {
  return static_cast<size_t>(key_code) < KEYS_COUNT;
}
