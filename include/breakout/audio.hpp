#ifndef YU_AUDIO_H
#define YU_AUDIO_H

struct ma_engine;
struct ma_sound;

class AudioEngine {
  friend class Sound;

public:
  AudioEngine();
  AudioEngine(const AudioEngine &) = delete;
  AudioEngine(AudioEngine &&) = delete;
  AudioEngine &operator=(const AudioEngine &) = delete;
  AudioEngine &operator=(AudioEngine &&) = delete;
  ~AudioEngine();

  bool play(const char *filename) const;

  bool is_initialized() const { return m_engine_initialized; }

private:
  ma_engine *m_engine;
  bool m_engine_initialized;
};

class Sound {
public:
  Sound(AudioEngine &engine);
  Sound(const Sound &) = delete;
  Sound(Sound &&) = delete;
  Sound &operator=(const Sound &) = delete;
  Sound &operator=(Sound &&) = delete;
  ~Sound();

  bool load(const char *filename, bool loop = false, float volume = 1.0f);
  bool play() const;

  bool is_loaded() const { return m_loaded; }

private:
  bool m_loaded;
  ma_sound *m_sound;
  AudioEngine &m_audio_engine;
};

#endif /* !YU_AUDIO_H */
