#pragma once

#include <filesystem>
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
      struct Note
      {
        std::uint8_t  key;        // Key number of the note
        std::int8_t   velocity;   // Velocity of pressed key
        std::uint8_t  polyphonic; // Pressure on pressed key

        std::uint16_t             channel_pitch;        // Current channel pitch of the note
        std::uint8_t              channel_program;      // Current channel program
        std::array<uint8_t, 128>  channel_controllers;  // Current channel controllers
      };

      std::size_t         _sampleRate; // Number of sample per second
      std::vector<float>  _buffer;     // Rendering target, stereo
      std::mutex          _bufferLock; // Lock for multithreaded rendering

      void  generateChannel(const Game::Audio::Midi::Sequence& sequence, const Game::Audio::Midi::Sequence::Track& track, const Game::Audio::Midi::Sequence::Track::Channel& channel);
      void  generateNote(const Game::Audio::Midi::Sequence& sequence, const Game::Audio::Midi::Sequence::Track& track, const Game::Audio::Midi::Sequence::Track::Channel& channel, std::list<Game::Audio::Midi::Sequence::Track::Channel::Event>::const_iterator event, Game::Audio::Synthesizer::Note note);
      void  generateInstrument(const Game::Audio::Midi::Sequence& sequence, const Game::Audio::Midi::Sequence::Track& track, const Game::Audio::Midi::Sequence::Track::Channel& channel, std::list<Game::Audio::Midi::Sequence::Track::Channel::Event>::const_iterator event, Game::Audio::Synthesizer::Note note, const Game::Audio::Soundfont::Preset& preset, const Game::Audio::Soundfont::Preset::Instrument& instrument, const Game::Audio::Soundfont::Preset::Instrument::Bag& bag);

    public:
      Synthesizer(const std::filesystem::path& midi, const std::filesystem::path& soundfont, std::size_t sampleRate = 22050);
      ~Synthesizer() = default;

      Game::Audio::Midi       midi;
      Game::Audio::Soundfont  soundfont;
      
      std::vector<float>  generate(std::size_t sequenceId); // Generate music from MIDI sequence, 2 channel at given sample rate
    };
  }
}