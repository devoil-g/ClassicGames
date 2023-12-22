#pragma once

#include <filesystem>
#include <fstream>
#include <exception>
#include <string>
#include <list>
#include <unordered_map>
#include <vector>

#include "System/Utilities.hpp"

namespace Game
{
  namespace Audio
  {
    class Soundfont
    {
#pragma pack(push, 1)
    private:
      struct Sf2Header
      {
        uint32_t  magic;  // SF2 file magic, should be "RIFF" (Resource Interchange File Format)
        uint32_t  size;   // File size -8 (magic & size)
        uint32_t  type;   // File type, should be "sfbk" (Soundfont Bank)
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

      union Sf2Amount
      {
        struct
        {
          uint8_t low;      // Low value of range
          uint8_t high;     // High value of range
        }         range;    // Amount is a range

        int16_t   s_amount; // Signed 16bit amount [-32768:+32767]
        uint16_t  u_amount; // Unsigned 16bit amount [0:+65535]
      };

    public:
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
        VelocityRange = 44,
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

    private:
      struct Sf2GenList
      {
        Game::Audio::Soundfont::Sf2Generator  operation;
        Game::Audio::Soundfont::Sf2Amount     amount;
      };

      struct Sf2Modulator
      {
      private:
        uint16_t  _data; // Raw data of the modulator

      public:
        enum Type : uint8_t
        {
          Linear = 0, // The Soundfont modulator controller moves linearly from the minimum to the maximum value in the direction and with the polarity specified by the 'D' and 'P' bits.
          Concave = 1 // The Soundfont modulator controller moves in a concave fashion from the minimum to the maximum value in the direction and with the polarity specified by the 'D' and 'P' bits.
        };

        enum Index : uint8_t
        {
          NoController = 0,           // No controller is to be used. The output of this controller module should be treated as if its value were set to '1'. It should not be a means to turn off a modulator.
          NoteOnVelocity = 2,         // The controller source to be used is the velocity value which is sent from the MIDI note-on command which generated the given sound.
          NoteOnKeyNumber = 3,        // The controller source to be used is the key number value which was sent from the MIDI note-on command which generated the given sound.
          PolyPressure = 10,          // The controller source to be used is the poly-pressure amount that is sent from the MIDI poly-pressure command.
          ChannelPressure = 13,       // The controller source to be used is the channel pressure amount that is sent from the MIDI channel-pressure command.
          PitchWheel = 14,            // The controller source to be used is the pitch wheel amount which is sent from the MIDI pitch wheel command.
          PitchWheelSensitivity = 16  // The controller source to be used is the pitch wheel sensitivity amount which is sent from the MIDI RPN 0 pitch wheel sensitivity command.
        };

        Game::Audio::Soundfont::Sf2Modulator::Type  type() { return (Game::Audio::Soundfont::Sf2Modulator::Type)((((uint8_t*)&_data)[0] & 0b11111100) >> 2); }
        bool                                        polarity() { return (((uint8_t*)&_data)[0] & 0b00000010) ? true : false; }
        bool                                        direction() { return (((uint8_t*)&_data)[0] & 0b00000001) ? true : false; }
        bool                                        continuous() { return (((uint8_t*)&_data)[1] & 0b10000000) ? true : false; }
        Game::Audio::Soundfont::Sf2Modulator::Index index() { return (Game::Audio::Soundfont::Sf2Modulator::Index)(((uint8_t*)&_data)[1] & 0b01111111); }
      };

      enum Sf2Transform : uint16_t
      {
        Linear  // The output value of the multiplier is to be fed directly to the summing node of the given destination
      };

      struct Sf2ModList
      {
        Game::Audio::Soundfont::Sf2Modulator  source;       // The source of data for the modulator
        Game::Audio::Soundfont::Sf2Generator  destination;  // The destination of the modulator
        int16_t                               amount;       // The degree to which the source modulates the destination
        Game::Audio::Soundfont::Sf2Modulator  degree;       // The degree to which the source modulates the destination is to be controlled by the specified modulation source
        Game::Audio::Soundfont::Sf2Transform  transform;    // A transform of the specified type will be applied to the modulation source before application to the modulator
      };

      struct Sf2Sample
      {
        enum Sf2SampleLink : uint16_t
        {
          Mono = 1,         // Only one channel
          Right = 2,        // Right channel, left in sampleLink sample
          Left = 4,         // Left channel, right in sampleLink sample
          Linked = 8,       // Unused, not currently fully defined in the Soundfont2 specification
          RomMono = 32769,  // Not supported
          RomRight = 32770, // Not supported
          RomLeft = 32772,  // Not supported
          RomLinked = 32776 // Not supported
        };

        int8_t                                            name[20];         // Name of the sample
        uint32_t                                          sampleStart;      // Index of the first sample in sample section
        uint32_t                                          sampleEnd;        // Index of the last sample in sample section
        uint32_t                                          loopStart;        // Index of the first sample of loop in sample section
        uint32_t                                          loopEnd;          // Index of the last sample of loop in sample section
        uint32_t                                          sampleRate;       // Sample rate of the sample
        uint8_t                                           pitchOriginal;    // The MIDI key number of the recorded pitch of the sample
        int8_t                                            pitchCorrection;  // Pitch correction in cents that should be applied to the sample on playback
        uint16_t                                          sampleLink;       // Associated sample (for stereo)
        Game::Audio::Soundfont::Sf2Sample::Sf2SampleLink  sampleType;       // Type of sample link
      };
#pragma pack(pop)

      void  load(const std::filesystem::path& filename);
      void  loadSections(std::ifstream& file, const Game::Audio::Soundfont::Sf2Header& header, std::vector<float>& samplesFloat);
      void  loadSectionList(std::ifstream& file, const Game::Audio::Soundfont::Sf2Header& header, const Game::Audio::Soundfont::Sf2SectionHeader& section, std::vector<float>& samplesFloat, std::streampos position);
      void  loadSectionListInfo(std::ifstream& file, const Game::Audio::Soundfont::Sf2Header& header, const Game::Audio::Soundfont::Sf2SectionHeader& section, std::streampos position);
      void  loadSectionListInfoIfil(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoIsng(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoInam(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoIrom(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoVer(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoIcrd(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoIeng(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoIprd(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoIcop(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoIcmt(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info);
      void  loadSectionListInfoIsft(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info);
      void  loadSectionListSdta(std::ifstream& file, const Game::Audio::Soundfont::Sf2Header& header, const Game::Audio::Soundfont::Sf2SectionHeader& section, std::vector<float>& samples, std::streampos position);
      void  loadSectionListPdta(std::ifstream& file, const Game::Audio::Soundfont::Sf2Header& header, const Game::Audio::Soundfont::Sf2SectionHeader& section, std::vector<float>& samples, std::streampos position);

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
        Game::Utilities::read(file, datas.data(), datas.size());
      };

      struct Info
      {
        std::pair<uint16_t, uint16_t> version;    // [ifil] Soundfont version (major/minor)
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

    public:
      struct Generator : public std::array<Game::Audio::Soundfont::Sf2Amount, Game::Audio::Soundfont::Sf2Generator::Count>
      {
        Generator();
        ~Generator() = default;
      };

      struct Modulator
      {
        // TODO
      };

      struct Preset
      {
        struct Instrument
        {
          struct Bag
          {
            Soundfont::Generator  generator;  // Instrument (local) generator
            Soundfont::Modulator  modulator;  // Modulator of the instrument
          };

          std::string                 name;       // Instrument name
          Soundfont::Generator        generator;  // Preset (global) generator
          std::list<Instrument::Bag>  bags;       // Bags of the instrument
        };

        std::string                   name;         // Name of the preset
        std::list<Preset::Instrument> instruments;  // Instruments of the preset
      };

      struct Sample
      {
        enum Link : uint16_t
        {
          Mono = Sf2Sample::Sf2SampleLink::Mono,    // Only one channel
          Right = Sf2Sample::Sf2SampleLink::Right,  // Right channel, left at link index
          Left = Sf2Sample::Sf2SampleLink::Left     // Left channel, right at link index
        };

        std::string         name;       // Name of the sample
        std::vector<float>  samples;    // Samples stored in float format [-1:+1[
        std::size_t         start;      // Index of the first sample of the loop
        std::size_t         end;        // Index of the first sample after the loop
        std::size_t         rate;       // Sample rate of the sample
        uint8_t             key;        // The MIDI key number of the recorded pitch of the sample
        int8_t              correction; // Pitch correction in cents that should be applied to the sample on playback
        Sample::Link        type;       // Type of link
        std::size_t         link;       // Index of other channel for stereo samples
      };

      Soundfont(const std::filesystem::path& filename);
      ~Soundfont() = default;

      Soundfont::Info                                                             info;     // General informations
      std::unordered_map<uint8_t, std::unordered_map<uint8_t, Soundfont::Preset>> presets;  // Presets ordered by bank/MIDI preset number
      std::vector<Soundfont::Sample>                                              samples;  // Samples used by presets
    };
  }
}