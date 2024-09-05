#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "breakout/audio.hpp"
#include "breakout/log.hpp"

AudioEngine::AudioEngine() : m_engine_initialized(false) {
  m_engine = new ma_engine;

  ma_result result = ma_engine_init(nullptr, m_engine);

  /* Should we care about errors? ma_engine_init can return only OUT_OF_MEMORY
   * errors */
  if (result != MA_SUCCESS) {
    LOG_CRITICAL("Failed to initialize sound engine");
    return;
  }
  m_engine_initialized = true;
}

AudioEngine::~AudioEngine() {
  if (m_engine_initialized) {
    ma_engine_uninit(m_engine);
  }
  delete m_engine;
}

bool AudioEngine::play(const char *filename) const {
  if (!m_engine_initialized) {
    return false;
  }

  ma_result result = ma_engine_play_sound(m_engine, filename, nullptr);
  if (result != MA_SUCCESS) {
    LOG_ERROR("Failed to play sound at path: {}", filename);
    return false;
  }

  return true;
}

Sound::Sound(AudioEngine &engine) : m_loaded(false), m_audio_engine(engine) {
  m_sound = new ma_sound;
}

Sound::~Sound() {
  if (m_loaded) {
    ma_sound_uninit(m_sound);
  }
  delete m_sound;
}

bool Sound::load(const char *filename, bool loop, float volume) {
  if (m_loaded) {
    ma_sound_uninit(m_sound);
  }

  ma_result result = ma_sound_init_from_file(m_audio_engine.m_engine, filename,
                                             0, nullptr, nullptr, m_sound);
  if (result != MA_SUCCESS) {
    LOG_ERROR("Failed to load sound: {}", filename);
    m_loaded = false;
    return false;
  }
  m_loaded = true;

  ma_sound_set_looping(m_sound, loop);
  ma_sound_set_volume(m_sound, volume);
  return true;
}

bool Sound::play() const {
  if (!m_loaded) {
    return false;
  }

  ma_result result = ma_sound_start(m_sound);
  if (result != MA_SUCCESS) {
    LOG_WARN("Failed to play sound");
    return false;
  }

  return true;
}
