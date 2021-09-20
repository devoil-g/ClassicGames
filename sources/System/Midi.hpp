#pragma once

#include <fstream>
#include <cstdint>
#include <string>
#include <vector>

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace Game
{
  class Midi
  {
  private:
    static constexpr uint32_t str_to_key(std::string_view str)
    {
      uint32_t  key = 0;

      // Copy string in key
      for (unsigned int i = 0; i < std::min(sizeof(uint32_t), str.length()); i++)
        ((char*)&key)[i] = str.at(i);

      return key;
    }

    static inline std::string key_to_str(const uint32_t key)
    {
      char  str[sizeof(uint32_t) + 1] = { 0 };

      // Copy key in string (ignore warning of std::strncpy)
#pragma warning(suppress:4996; suppress:6053)
      return std::strncpy(str, (const char*)&key, sizeof(uint32_t));
    }

    template<typename Type>
    static inline std::istream& read(std::istream& stream, Type* ptr, std::size_t number = 1)
    {
      // Safe read, throw exception in case of error
      if (stream.read((char*)ptr, number * sizeof(Type)).good() == false)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      return stream;
    }

    template<typename Type>
    static inline Type  swap_endianness(const Type& value)
    {
      Type  result = value;

      // Swap bytes of value
      for (int index = 0; index < sizeof(Type) / 2; index++)
        std::swap(((uint8_t*)&result)[index], ((uint8_t*)&result)[sizeof(Type) - index - 1]);
      return result;
    }

  public:
    class SoundFont
    {
    private:
      
#pragma pack(push, 1)
    private:
      struct Sf2Header
      {
        uint32_t  magic;  // SF2 file magic, should be "RIFF" (Resource Interchange File Format)
        uint32_t  size;   // File size -8 (magic & size)
        uint32_t  type;   // File type, should be "sfbk" (SoundFont Bank)
      };

      struct Sf2SectionHeader
      {
        uint32_t  type; // Section type
        uint32_t  size; // Section size -8 (name & size)
        uint32_t  name; // Section name
      };

      struct Sf2SubsectionHeader
      {
        uint32_t  name; // Name of the Info
        uint32_t  size; // Size of the info -8
      };

      struct Sf2Preset
      {
        int8_t    name[20];   // Name of the preset
        uint16_t  midi;       // MIDI Preset Number
        uint16_t  bank;       // MIDI BankNumber which apply to this preset
        uint16_t  bag;        // Index to the preset's zone list in the PBAG sub-chunk
        uint32_t  library;    // Unused, reserved for future implementation
        uint32_t  genre;      // Unused, reserved for future implementation
        uint32_t  morphology; // Unused, reserved for future implementation
      };

      struct Sf2Instrument
      {
        int8_t    name[20]; // Name of the instrument
        uint16_t  bag;      // Index to the instrument's zone list in the IBAG sub-chunk
      };

      struct Sf2Bag
      {
        uint16_t  generator;
        uint16_t  modulator;
      };

      union Sf2GenAmountType
      {
        struct {
          uint8_t low;      // Low value of range
          uint8_t high;     // High value of range
        }         range;    // Amount is a range

        int16_t   s_amount; // Signed 16bit amount [-32768:+32767]
        uint16_t  u_amount; // Unsigned 16bit amount [0:+65535]
      };

      enum Sf2Generator : uint16_t
      {
        StartAddrsOffset = 0,
        EndAddrsOffset = 1,
        StartLoopAddrsOffset = 2,
        EndLoopAddrsOffset = 3,
        StartAddrsCoarseOffset = 4,
        ModLfoToPitch = 5,
        VibLfoToPitch = 6,
        ModEnvToPitch = 7,
        InitialFilterFc = 8,
        InitialFilterQ = 9,
        ModLfoToFilterFc = 10,
        ModEnvToFilterFc = 11,
        EndAddrsCoarseOffset = 12,
        ModLfoToVolume = 13,
        Unused1 = 14,
        ChorusEffectsSend = 15,
        ReverbEffectsSend = 16,
        Pan = 17,
        Unused2 = 18,
        Unused3 = 19,
        Unused4 = 20,
        DelayModLfo = 21,
        FreqModLfo = 22,
        DelayVibLfo = 23,
        FreqVibLfo = 24,
        DelayModEnv = 25,
        AttackModEnv = 26,
        HoldModEnv = 27,
        DecayModEnv = 28,
        SustainModEnv = 29,
        ReleaseModEnv = 30,
        KeynumToModEnvHold = 31,
        KeynumToModEnvDecay = 32,
        DelayVolEnv = 33,
        AttackVolEnv = 34,
        HoldVolEnv = 35,
        DecayVolEnv = 36,
        SustainVolEnv = 37,
        ReleaseVolEnv = 38,
        KeynumToVolEnvHold = 39,
        KeynumToVolEnvDecay = 40,
        Instrument = 41,
        Reserved1 = 42,
        KeyRange = 43,
        VelRange = 44,
        StartLoopAddrsCoarseOffset = 45,
        Keynum = 46,
        Velocity = 47,
        InitialAttenuation = 48,
        Reserved2 = 49,
        EndLoopAddrsCoarseOffset = 50,
        CoarseTune = 51,
        FineTune = 52,
        SampleId = 53,
        SampleMode = 54,
        Reserved3 = 55,
        ScaleTuning = 56,
        ExclusiveClass = 57,
        OverridingRootKey = 58,
        Unused5 = 59,
        Unused6 = 60,

        Count = 61
      };

      struct Sf2GenList
      {
        Sf2Generator      operation;
        Sf2GenAmountType  amount;
      };

      struct Sf2Modulator
      {
        uint16_t  data; // Raw data of the modulator

        uint8_t type() { return (((uint8_t*)&data)[0] & 0b11111100) >> 2; }
        bool    polarity() { return (((uint8_t*)&data)[0] & 0b00000010) ? true : false; }
        bool    direction() { return (((uint8_t*)&data)[0] & 0b00000001) ? true : false; }
        bool    continuous() { return (((uint8_t*)&data)[1] & 0b10000000) ? true : false; }
        uint8_t index() { return ((uint8_t*)&data)[1] & 0b01111111; }
      };

      enum Sf2Transform : uint16_t
      {
        Linear  // The output value of the multiplier is to be fed directly to the summing node of the given destination
      };

      struct Sf2ModList
      {
        Sf2Modulator  source;       // The source of data for the modulator
        Sf2Generator  destination;  // The destination of the modulator
        int16_t       amount;       // The degree to which the source modulates the destination
        Sf2Modulator  degree;       // The degree to which the source modulates the destination is to be controlled by the specified modulation source
        Sf2Transform  transform;    // A transform of the specified type will be applied to the modulation source before application to the modulator
      };

      struct Sf2Sample
      {
        enum Sf2SampleLink : uint16_t
        {
          Mono = 1,         // Only one channel
          Right = 2,        // Right channel, left in sampleLink sample
          Left = 4,         // Left channel, right in sampleLink sample
          Linked = 8,       // Unused, not currently fully defined in the SoundFont2 specification
          RomMono = 32769,  // Not supported
          RomRight = 32770, // Not supported
          RomLeft = 32772,  // Not supported
          RomLinked = 32776 // Not supported
        };

        int8_t        name[20];
        uint32_t      sampleStart;
        uint32_t      sampleEnd;
        uint32_t      loopStart;
        uint32_t      loopEnd;
        uint32_t      sampleRate;
        uint8_t       pitchOriginal;
        int8_t        pitchCorrection;
        uint16_t      sampleLink;
        Sf2SampleLink sampleType;
      };
#pragma pack(pop)

      void  load(const std::string& filename);
      void  loadSections(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header);
      void  loadSectionList(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header, const Game::Midi::SoundFont::Sf2SectionHeader& section, std::streampos position);
      void  loadSectionListInfo(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header, const Game::Midi::SoundFont::Sf2SectionHeader& section, std::streampos position);
      void  loadSectionListInfoIfil(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoIsng(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoInam(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoIrom(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoVer(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoIcrd(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoIeng(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoIprd(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoIcop(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoIcmt(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoIsft(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info);
      void  loadSectionListSdta(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header, const Game::Midi::SoundFont::Sf2SectionHeader& section, std::streampos position);
      void  loadSectionListPdta(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header, const Game::Midi::SoundFont::Sf2SectionHeader& section, std::streampos position);

      std::string loadString(std::ifstream& file, std::size_t size);

      template <typename Data>
      void  loadSubsection(std::ifstream& file, std::size_t size, std::vector<Data>& datas)
      {
        // Check for invalid subsection size
        if (size % sizeof(Data) != 0)
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

        // Reset data container
        datas.resize(size / sizeof(Data));

        // Load datas
        read(file, datas.data(), datas.size());
      };

      struct Info
      {
        std::pair<uint16_t, uint16_t> version;    // [ifil] SoundFont version (major/minor)
        std::string                   engine;     // [isng] Optimized audio engine
        std::string                   name;       // [INAM] Name of sound bank
        std::string                   rom;        // [IROM] Name of ROM to use
        std::pair<uint16_t, uint16_t> romVersion; // [VER] ROM version to use
        std::string                   creation;   // [ICRD] Date of creation
        std::string                   author;     // [IENG] Name of the author(s)
        std::string                   product;    // [IPRD] Name of the target product
        std::string                   copyright;  // [ICOP] Copyright license
        std::string                   comment;    // [ICMT] Comment
        std::string                   tool;       // [ISFT] Tool used to create the sound bank
      };

      struct Modulator
      {
        uint8_t type;       // 6-bit value specifying the continuity of the controller
        bool    polarity;   // Polarity
        bool    direction;  // Direction
        bool    controller; // MIDI Continuous Controller Flag
        uint8_t index;      // 7 bit value specifying the controller source
      };

      union GeneratorAmount
      {
        struct
        {
          uint8_t byLo;
          uint8_t byHi;
        }         ranges;

        int16_t   shAmount;
        uint16_t  wAmount;
      };

      struct Preset
      {

      };

    public:
      SoundFont(const std::string& filename);
      ~SoundFont() = default;

      Game::Midi::SoundFont::Info                                                             info;       // General informations
      std::vector<int16_t>                                                                    samples16;  // Raw audio samples, 16bits
      std::vector<int8_t>                                                                     samples24;  // Additional byte to transform raw audio samples from 16bits to 24bits
      std::unordered_map<uint8_t, std::unordered_map<uint8_t, Game::Midi::SoundFont::Preset>> presets;    // Preset organized by bank/midi number
    };

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

        std::string                                             name;       // Specifies the title of the sequence
        std::string                                             copyright;  // Specifies copyright information for the sequence
        std::string                                             instrument; // Names the instrument intended to play the contents of this track
        std::list<std::pair<std::size_t, std::string>>          lyrics;     // Lyrics intended to be sung at the given time
        std::list<std::pair<std::size_t, std::string>>          markers;    // Points in the sequence which occurs at the given time
        std::list<std::pair<std::size_t, std::string>>          cues;       // Identifies synchronisation point which occurs at the specified time
        std::list<std::pair<std::size_t, std::string>>          texts;      // Supplies an arbitrary text at the specified time
        uint16_t                                                sequence;   // Specifies the sequence number
        std::list<std::pair<std::size_t, uint32_t>>             tempos;     // Number of microseconds per quarter note at the specified time (default 120BPM)
        MetaData::Smpte                                         smpte;      // Specify the starting point offset from the beginning of the track
        std::list<std::pair<std::size_t, MetaData::Signature>>  signatures; // Time signature of the track at the given time
        std::list<std::pair<std::size_t, MetaData::Key>>        keys;       // Keys signature of the track at the given time
        std::string                                             program;    // The name of the program (ie, patch) used to play the track
        std::list<std::pair<std::size_t, std::string>>          devices;    // The name of the MIDI devices (port) where the track is routed at the specified time
        std::vector<uint8_t>                                    datas;      // Store vendor-proprietary data
        std::unordered_map<MetaData::Tag, std::string>          tags;       // Music tags (not standard)
        std::size_t                                             end;        // Clock value at end of sequence
      };

      struct Track
      {
        struct Channel
        {
          struct Key
          {
            uint8_t key;      // Key number
            int8_t  velocity; // Velocity of press/release
          };

          struct PolyphonicKey
          {
            uint8_t key;      // Key number
            uint8_t pressure; // Pressure applied to the key
          };

          enum Controller : uint8_t
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
            ControllerPolyhonicOperation = 127
          };

          std::list<std::pair<std::size_t, Channel::Key>>                                     note;       // Note pressed/released, null or negative velocity means release
          std::list<std::pair<std::size_t, Channel::PolyphonicKey>>                           polyphonic; // Polyphonic key pressure
          std::list<std::pair<std::size_t, uint8_t>>                                          program;    // Patch number
          std::list<std::pair<std::size_t, uint16_t>>                                         pitch;      // Pitch wheel modifier (center: 0x2000)
          std::unordered_map<Channel::Controller, std::list<std::pair<std::size_t, uint8_t>>> controller; // Controller modifying the sound
        };

        std::string                     name;     // Name of the track
        std::array<Track::Channel, 16>  channel;  // Instruction for each channel
      };

      Game::Midi::Sequence::MetaData                                metadata;   // Informations of the track
      std::unordered_map<std::size_t, Game::Midi::Sequence::Track>  tracks;  // Subtracks of the track
    };

    Game::Midi::SoundFont       _soundfont;   // Sound library
    Game::Midi::MidiTrackFormat _format;      // MIDI file format 
    uint16_t                    _trackNumber; // Number of tracks in the file
    Game::Midi::MidiTimeFormat  _timeFormat;  // Track time format
    uint16_t                    _timeValue;   // Track time format value

    void  load(const std::string& filename);  // Load a MIDI (.mid) file
    void  loadHeader(std::ifstream& file);    // Load first chunk, should be MThd
    void  loadTracks(std::ifstream& file);    // Load MIDI tracks

    void  loadTrack(std::ifstream& file, const Game::Midi::MidiHeader& header, Game::Midi::Sequence& sequence, std::size_t track);  // Load MIDI tracks

    void  loadTrackSysexF0(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);  // MIDI system exclusive message
    void  loadTrackSysexF7(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);  // MIDI system exclusive message

    void  loadTrackMeta(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock); // Handle MIDI meta events
    void  loadTrackMetaName(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);             // Specifies the title of the track or sequence
    void  loadTrackMetaCopyright(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);        // Specifies copyright information for the sequence
    void  loadTrackMetaInstrument(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);       // Names the instrument intended to play the contents of this track
    void  loadTrackMetaLyrics(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);           // Lyrics intended to be sung at the given time
    void  loadTrackMetaMarkers(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);          // Points in the sequence which occurs at the given time
    void  loadTrackMetaCues(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);             // Identifies synchronisation point which occurs at the specified time
    void  loadTrackMetaTexts(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);            // Supplies an arbitrary text at the specified time
    void  loadTrackMetaSequence(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);         // Specifies the sequence number
    void  loadTrackMetaTempos(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);           // Number of microseconds per quarter note at the specified time (default 120BPM)
    void  loadTrackMetaSmpte(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);            // Specify the starting point offset from the beginning of the track
    void  loadTrackMetaSignatures(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);       // Time signature of the track at the given time
    void  loadTrackMetaKeys(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);             // Keys signature of the track at the given time
    void  loadTrackMetaProgram(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);          // The name of the program (ie, patch) used to play the track
    void  loadTrackMetaDevices(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);          // The name of the MIDI devices (port) where the track is routed at the specified time
    void  loadTrackMetaChannels(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);         // May be used to associate a MIDI channel with all events which follow (not supported)
    void  loadTrackMetaPorts(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);            // The number of the port where the track is routed at the specified time (not supported)
    void  loadTrackMetaDatas(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);            // Store vendor-proprietary data
    void  loadTrackMetaTags(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);             // Track tags (not standard)
    void  loadTrackMetaEnd(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock);              // Track tags (not standard)

    void  loadTrackMidiNoteOff(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);                // Sent when a note is released (ended)
    void  loadTrackMidiNoteOn(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);                 // Sent when a note is depressed (start)
    void  loadTrackMidiPolyphonicKeyPressure(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);  // Most often sent by pressing down on the key after it "bottoms out"
    void  loadTrackMidiControlChange(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);          // Sent when a controller value changes
    void  loadTrackMidiProgramChange(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);          // Sent when the patch number changes
    void  loadTrackMidiChannelPressure(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);        // Most often sent by pressing down on the key after it "bottoms out"
    void  loadTrackMidiPitchWheelChange(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);       // Indicates a change in the pitch wheel

    void  loadTrackUndefined(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock);  // Undefined command, specify the number of byte to skip

    uint64_t    loadVariableLengthQuantity(std::ifstream& file);  // Load a Variable Length Quantity number
    std::string loadText(std::ifstream& file);                    // Load a string (length + text)

    sf::Time    duration(const Game::Midi::Sequence& sequence, std::size_t clock);  // Translate MIDI clock time to SFML time

    // TODO: remove this
    void  generate(const Game::Midi::Sequence& sequence);

  public:
    Midi(const std::string& filename);
    ~Midi() = default;

    std::list<Game::Midi::Sequence>  sequences;
  };
}