#pragma once

#include <array>
#include <fstream>
#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <mutex>
#include <unordered_map>

#include "System/Audio/Soundfont.hpp"

namespace Game
{
  namespace Audio
  {
    class Midi
    {
    private:
      struct MidiHeader
      {
        uint32_t  type; // Chunk type, MThd or MTrk
        uint32_t  size; // Number of byte in the chunk (header not included)
      };

      enum MidiTrackFormat : uint16_t
      {
        SingleTrack = 0,        // The file contains a single multi-channel track
        SimultaneousTracks = 1, // The file contains one or more simultaneous tracks (or MIDI outputs) of a sequence
        MultipleTracks = 2      // The file contains one or more sequentially independent single-track patterns
      };

      enum MidiTimeFormat
      {
        QuarterNote = 0,        // The value is the division of a quarter note
        SMPTE = 1               // The value is the number of ticks per second
      };

    public:
      struct Sequence
      {
        struct MetaData
        {
          struct Smpte
          {
            uint8_t framerate;    // Number of frames per second
            uint8_t hours;        // Number of hours
            uint8_t minutes;      // Number of minutes
            uint8_t seconds;      // Number of seconds
            uint8_t frames;       // Number of the frame
            uint8_t fractionals;  // Fraction of frame
          };

          struct Signature
          {
            uint8_t numerator;    // Numerator of the signature as notated on sheet music
            uint8_t denominator;  // Denominator of the signature as notated on sheet music
            uint8_t metronome;    // Number of MIDI clocks in a metronome click
            uint8_t quarter;      // Number of notated 32nd notes in a MIDI quarter note (24 MIDI clocks)
          };

          struct Key
          {
            uint8_t key;    // 0 for the key of C, a positive value for each sharp above C, or a negative value for each flat below C
            bool    major;  // True when major, false when minor
          };

          enum Tag : uint8_t
          {
            TagGenre = 1,
            TagArtist = 2,
            TagComposer = 3,
            TagDuration = 4,
            TagBpm = 5
          };

          std::string                                       name;       // Specifies the title of the sequence
          std::string                                       copyright;  // Specifies copyright information for the sequence
          std::string                                       instrument; // Names the instrument intended to play the contents of this track
          std::list<std::pair<float, std::string>>          lyrics;     // Lyrics intended to be sung at the given time
          std::list<std::pair<float, std::string>>          markers;    // Points in the sequence which occurs at the given time
          std::list<std::pair<float, std::string>>          cues;       // Identifies synchronisation point which occurs at the specified time
          std::list<std::pair<float, std::string>>          texts;      // Supplies an arbitrary text at the specified time
          uint16_t                                          sequence;   // Specifies the sequence number
          std::list<std::pair<std::size_t, uint32_t>>       tempos;     // Number of microseconds per quarter note at the specified tick (default 120BPM)
          MetaData::Smpte                                   smpte;      // Specify the starting point offset from the beginning of the track
          std::list<std::pair<float, MetaData::Signature>>  signatures; // Time signature of the track at the given time
          std::list<std::pair<float, MetaData::Key>>        keys;       // Keys signature of the track at the given time
          std::string                                       program;    // The name of the program (ie, patch) used to play the track
          std::list<std::pair<float, std::string>>          devices;    // The name of the MIDI devices (port) where the track is routed at the specified time
          std::vector<uint8_t>                              datas;      // Store vendor-proprietary data
          std::unordered_map<MetaData::Tag, std::string>    tags;       // Music tags (not standard)
          float                                             end;        // Clock value at end of sequence
        };

        struct Track
        {
          struct Channel
          {
            struct Note
            {
              uint8_t key;      // Key number
              int8_t  velocity; // Velocity of press/release
            };

            struct Polyphonic
            {
              uint8_t key;      // Key number
              uint8_t pressure; // Pressure applied to the key
            };

            struct Controller
            {
              enum Type : uint8_t
              {
                ControllerBankSelectCoarse = 0,
                ControllerModulationWheelCoarse = 1,
                ControllerBreathCoarse = 2,
                // ... (undefined)
                ControllerFootPedalCoarse = 4,
                ControllerPortamentoTimeCoarse = 5,
                ControllerDataEntryCoarse = 6,
                ControllerVolumeCoarse = 7,
                ControllerBalanceCoarse = 8,
                // ... (undefined)
                ControllerPanCoarse = 10,
                ControllerExpressionCoarse = 11,
                ControllerEffect1Coarse = 12,
                ControllerEffect2Coarse = 13,
                // ... (undefined)
                ControllerGeneralPurpose1 = 16,
                ControllerGeneralPurpose2 = 17,
                ControllerGeneralPurpose3 = 18,
                ControllerGeneralPurpose4 = 19,
                // ... (undefined)
                ControllerBankSelectFine = 32,
                ControllerModulationWheelFine = 33,
                ControllerBreathFine = 34,
                // ... (undefined)
                ControllerFootPedalFine = 36,
                ControllerPortamentoTimeFine = 37,
                ControllerDataEntryFine = 38,
                ControllerVolumeFine = 39,
                ControllerBalanceFine = 40,
                // ... (undefined)
                ControllerPanFine = 42,
                ControllerExpressionFine = 43,
                ControllerEffect1Fine = 44,
                ControllerEffect2Fine = 45,
                // ... (undefined)
                ControllerHoldPedal = 64,
                ControllerPortamento = 65,
                ControllerSustenutoPedal = 66,
                ControllerSoftPedal = 67,
                ControllerLegatoPedal = 68,
                ControllerHoldPedal2 = 69,
                ControllerSoundVariation = 70,
                ControllerSoundTimbre = 71,
                ControllerSoundReleaseTime = 72,
                ControllerSoundAttackTime = 73,
                ControllerSoundBrightness = 74,
                ControllerSound6 = 75,
                ControllerSound7 = 76,
                ControllerSound8 = 77,
                ControllerSound9 = 78,
                ControllerSound10 = 79,
                ControllerGeneralPurposeButton1 = 80,
                ControllerGeneralPurposeButton2 = 81,
                ControllerGeneralPurposeButton3 = 82,
                ControllerGeneralPurposeButton4 = 83,
                // ... (undefined)
                ControllerEffectLevel = 91,
                ControllerTremuloLevel = 92,
                ControllerChorusLevel = 93,
                ControllerCelesteLevel = 94,
                ControllerPhaserLevel = 95,
                ControllerDataButtonIncrement = 96,
                ControllerDataButtonDecrement = 97,
                ControllerNonRegisteredParameterNumberFine = 98,
                ControllerNonRegisteredParameterNumberCoarse = 99,
                ControllerRegisteredParameterNumberFine = 100,
                ControllerRegisteredParameterNumberCoarse = 101,
                // ... (undefined)
                ControllerAllSoundOff = 120,
                ControllerAllControllerOff = 121,
                ControllerLocalKeyboard = 122,
                ControllerAllNoteOff = 123,
                ControllerOmniOff = 124,
                ControllerOmniOn = 125,
                ControllerMonophonicOperation = 126,
                ControllerPolyhonicOperation = 127,

                ControllerCount = 128
              };

              Controller::Type  type;
              uint8_t           value;
            };

            struct Event
            {
              enum Type : uint8_t
              {
                EventKey,
                EventPolyphonicKey,
                EventProgram,
                EventPitch,
                EventController
              };

              float       clock;  // Time of the event (in second)
              Event::Type type;   // Type of the event

              union
              {
                Channel::Note       note;
                Channel::Polyphonic polyphonic;
                Channel::Controller controller;
                uint8_t             program;
                uint16_t            pitch;
              } data; // Data of the event
            };

            struct Context
            {
              std::array<int8_t, 128>                                         keys;         // Velocity of channel keys
              std::array<uint8_t, 128>                                        polyphonics;  // Pressure on channel keys
              std::array<uint8_t, Channel::Controller::Type::ControllerCount> controllers;  // Controllers of channel
              uint8_t                                                         program;      // Current program of channel
              uint16_t                                                        pitch;        // Current pitch of channel

              Context();
              ~Context() = default;

              void  update(const Channel::Event& event);
            };

            std::list<Channel::Event> events;
          };

          std::string                     name;     // Name of the track
          std::array<Track::Channel, 16>  channel;  // Instruction for each channel
        };

        Game::Audio::Midi::Sequence::MetaData                               metadata; // Informations of the track
        std::unordered_map<std::size_t, Game::Audio::Midi::Sequence::Track> tracks;   // Subtracks of the track
      };

    private:
      Game::Audio::Soundfont              _soundfont;   // Sound library
      Game::Audio::Midi::MidiTrackFormat  _format;      // MIDI file format 
      uint16_t                            _trackNumber; // Number of tracks in the file
      Game::Audio::Midi::MidiTimeFormat   _timeFormat;  // Track time format
      uint16_t                            _timeValue;   // Track time format value

      void  load(const std::string& filename);  // Load a MIDI (.mid) file
      void  loadHeader(std::ifstream& file);    // Load first chunk, should be MThd
      void  loadTracks(std::ifstream& file);    // Load MIDI tracks

      void  loadTrack(std::ifstream& file, const Game::Audio::Midi::MidiHeader& header, Game::Audio::Midi::Sequence& sequence, std::size_t track);  // Load MIDI tracks

      void  loadTrackSysexF0(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);  // MIDI system exclusive message
      void  loadTrackSysexF7(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);  // MIDI system exclusive message

      void  loadTrackMeta(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);  // Handle MIDI meta events
      void  loadTrackMetaName(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);              // Specifies the title of the track or sequence
      void  loadTrackMetaCopyright(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);         // Specifies copyright information for the sequence
      void  loadTrackMetaInstrument(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);        // Names the instrument intended to play the contents of this track
      void  loadTrackMetaLyrics(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);            // Lyrics intended to be sung at the given time
      void  loadTrackMetaMarkers(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);           // Points in the sequence which occurs at the given time
      void  loadTrackMetaCues(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);              // Identifies synchronisation point which occurs at the specified time
      void  loadTrackMetaTexts(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);             // Supplies an arbitrary text at the specified time
      void  loadTrackMetaSequence(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);          // Specifies the sequence number
      void  loadTrackMetaTempos(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);            // Number of microseconds per quarter note at the specified time (default 120BPM)
      void  loadTrackMetaSmpte(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);             // Specify the starting point offset from the beginning of the track
      void  loadTrackMetaSignatures(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);        // Time signature of the track at the given time
      void  loadTrackMetaKeys(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);              // Keys signature of the track at the given time
      void  loadTrackMetaProgram(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);           // The name of the program (ie, patch) used to play the track
      void  loadTrackMetaDevices(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);           // The name of the MIDI devices (port) where the track is routed at the specified time
      void  loadTrackMetaChannels(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);          // May be used to associate a MIDI channel with all events which follow (not supported)
      void  loadTrackMetaPorts(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);             // The number of the port where the track is routed at the specified time (not supported)
      void  loadTrackMetaDatas(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);             // Store vendor-proprietary data
      void  loadTrackMetaTags(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);              // Track tags (not standard)
      void  loadTrackMetaEnd(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock);               // Track tags (not standard)

      void  loadTrackMidiNoteOff(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);               // Sent when a note is released (ended)
      void  loadTrackMidiNoteOn(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);                // Sent when a note is depressed (start)
      void  loadTrackMidiPolyphonicKeyPressure(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock); // Most often sent by pressing down on the key after it "bottoms out"
      void  loadTrackMidiControlChange(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);         // Sent when a controller value changes
      void  loadTrackMidiProgramChange(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);         // Sent when the patch number changes
      void  loadTrackMidiChannelPressure(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);       // Most often sent by pressing down on the key after it "bottoms out"
      void  loadTrackMidiPitchWheelChange(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);      // Indicates a change in the pitch wheel

      void  loadTrackUndefined(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock); // Undefined command, specify the number of byte to skip

      uint64_t    loadVariableLengthQuantity(std::ifstream& file);  // Load a Variable Length Quantity number
      std::string loadText(std::ifstream& file);                    // Load a string (length + text)

      float duration(const Game::Audio::Midi::Sequence& sequence, std::size_t tic) const;                     // Translate MIDI clock time to SFML time
      float duration(const Game::Audio::Midi::Sequence& sequence, std::size_t start, std::size_t end) const;  // Translate MIDI clock time interval to SFML time

    public:
      Midi(const std::string& filename);
      ~Midi() = default;

      std::list<Game::Audio::Midi::Sequence>  sequences;
    };
  }
}