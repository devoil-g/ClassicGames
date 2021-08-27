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
  public:
    class SoundFont
    {
    private:
      constexpr uint32_t str_to_key(std::string_view str)
      {
        uint32_t  key = 0;

        // Copy string in key
        for (unsigned int i = 0; i < std::min(sizeof(uint32_t), str.length()); i++)
          ((char*)&key)[i] = str.at(i);

        return key;
      }

      inline std::string  key_to_str(const uint32_t key)
      {
        char  str[sizeof(uint32_t) + 1] = { 0 };

        // Copy key in string (ignore warning of std::strncpy)

        

#pragma warning(suppress:6053)
#pragma warning(suppress:4996)
        return std::strncpy(str, (const char*)&key, sizeof(uint32_t));
      }

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
        uint32_t  gorphology; // Unused, reserved for future implementation
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

      typedef uint16_t Sf2GenAmountType;

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

        Count = 60
      };

      struct Sf2GenList
      {
        Sf2Generator      operation;
        Sf2GenAmountType  amount;
      };

      typedef uint16_t Sf2Modulator;

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
        // Check for invalid lump size
        if (size % sizeof(Data) != 0) {
          std::cout << size << "/" << sizeof(Data) << std::endl;
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
        }

        // Reset data container
        datas.resize(size / sizeof(Data));

        // Load datas
        file.read((char*)datas.data(), size);
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

    public:
      SoundFont(const std::string& filename);
      ~SoundFont() = default;

      Game::Midi::SoundFont::Info                       info;                 // General informations
      std::vector<int16_t>                              samples16;            // Raw audio samples, 16bits
      std::vector<int8_t>                               samples24;            // Additional byte to transform raw audio samples from 16bits to 24bits
    };

    sf::SoundBuffer _buffer;

  public:
    Midi();
    ~Midi() = default;
  };
}