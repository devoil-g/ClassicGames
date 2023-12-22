#pragma once

#include <cstdint>
#include <array>
#include <SFML/Audio/SoundStream.hpp>

#include "Scenes/AbstractScene.hpp"

namespace Game
{
  class MidiScene : public Game::AbstractScene, public sf::SoundStream
  {
  private:
    static const unsigned int ChannelCount = 1;
    static const unsigned int SampleRate = 22050;
    static const unsigned int FramePerChunk = SampleRate / 48;
    static const float        MiddleCFrequency;

    enum Note
    {
      Do,
      DoD,
      Re,
      ReD,
      Mi,
      Fa,
      FaD,
      Sol,
      SolD,
      La,
      LaD,
      Si,

      NoteCount
    };

    constexpr float GetRatio(Game::MidiScene::Note note) {
      const std::array<float, Game::MidiScene::Note::NoteCount> ratio = {
        1.f,                // Do
        1.06787109375f,     // DoD
        1.125f,             // Re
        1.20135498046875f,  // ReD
        1.265625f,          // Mi
        1.33333333333333f,  // Fa
        1.423828125f,       // FaD
        1.5f,               // Sol
        1.601806640625f,    // SolD
        1.6875f,            // La
        1.802032470703125f, // LaD
        1.8984375f          // Si
      };

      return ratio[note];
    }

    std::array<int16_t, ChannelCount * FramePerChunk> _buffer;  // Audio buffer used for output
    std::size_t                                       _offset;  // Number of sample already played
    int                                               _octave;  // Current octave [+/-]
    std::array<float, 128>                            _wave;    // Pre-computed wave for fast cosinus

    virtual bool onGetData(sf::SoundStream::Chunk& data) override;
    virtual void onSeek(sf::Time timeOffset) override;

  public:
    MidiScene(Game::SceneMachine& machine);
    ~MidiScene() override = default;

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}