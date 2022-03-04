#pragma once

#include <string>

#include "System/Audio/Midi.hpp"
#include "System/Audio/Soundfont.hpp"

namespace Game
{
  namespace Audio
  {
    class Synthesizer
    {
    private:
      void  generateChannel(const Game::Audio::Midi::Sequence& sequence, const Game::Audio::Midi::Sequence::Track& track, const Game::Audio::Midi::Sequence::Track::Channel& channel, std::vector<float>& buffer, std::mutex& buffer_lock, std::size_t sampleRate) const;

    public:
      Synthesizer(const std::string& midi, const std::string& soundfont);
      ~Synthesizer() = default;

      Game::Audio::Midi       midi;
      Game::Audio::Soundfont  soundfont;
      
      std::vector<float>  generate(std::size_t sequenceId, std::size_t sampleRate = 22050) const;  // Generate music from MIDI sequence, 2 channel at given sample rate
    };
  }
}