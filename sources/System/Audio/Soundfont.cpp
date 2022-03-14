#include <fstream>
#include <array>
#include <vector>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <iomanip>
#include <tuple>
#include <algorithm>
#include <future>
#include <set>

#include "System/Audio/Soundfont.hpp"
#include "System/Config.hpp"
#include "System/Utilities.hpp"

Game::Audio::Soundfont::Soundfont(const std::string& filename) :
  info(),
  presets(),
  samples()
{
  // Load file
  load(filename);
}

void  Game::Audio::Soundfont::load(const std::string& filename)
{
  std::ifstream file(filename, std::ifstream::binary);

  // Check that file is correctly open
  if (file.good() == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  Game::Audio::Soundfont::Soundfont::Sf2Header  header = {};

  // Read RIFF header
  Game::Utilities::read(file, &header);

  // TODO: remove this
#ifdef _DEBUG
  std::cout
    << "[SOUNDFONT] filename: " << filename << std::endl
    << std::endl
    << "  [HEADER] magic: " << Game::Utilities::key_to_str(header.magic) << std::endl
    << "  [HEADER] size:  " << header.size << std::endl
    << "  [HEADER] type:  " << Game::Utilities::key_to_str(header.type) << std::endl
    ;
#endif

  // Check header data
  if (std::memcmp(&header.magic, "RIFF", sizeof(header.magic)) != 0 ||
    std::memcmp(&header.type, "sfbk", sizeof(header.magic)) != 0)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Buffer for samples
  std::vector<float>  samplesFloat;

  // Load sections of Soundfont
  loadSections(file, header, samplesFloat);
}

void  Game::Audio::Soundfont::loadSections(std::ifstream& file, const Game::Audio::Soundfont::Sf2Header& header, std::vector<float>& samplesFloat)
{
  Game::Audio::Soundfont::Soundfont::Sf2SectionHeader section = {};

  for (std::streampos section_position = sizeof(header); section_position < header.size + 8; section_position += section.size + 8)
  {
    section = {};

    // Read section header
    file.seekg(section_position, file.beg);
    Game::Utilities::read(file, &section);

    // TODO: remove this
#ifdef _DEBUG
    std::cout
      << std::endl
      << "  [SECTION] type: " << Game::Utilities::key_to_str(section.type) << std::endl
      << "  [SECTION] size: " << section.size << std::endl
      << "  [SECTION] name: " << Game::Utilities::key_to_str(section.name) << std::endl
      ;
#endif

    // Matching sections
    std::unordered_map<uint32_t, std::function<void()>> section_commands = {
      { Game::Utilities::str_to_key<uint32_t>("LIST"), std::bind(&Game::Audio::Soundfont::loadSectionList, this, std::ref(file), std::ref(header), std::ref(section), std::ref(samplesFloat), section_position) }
    };

    auto command = section_commands.find(section.type);

    // Execute matching command
    if (command != section_commands.end())
      command->second();
    else
      std::cerr << "[Game::Soundfont::load]: Warning, unknow section type '" << Game::Utilities::key_to_str(section.type) << "' (section ignored)." << std::endl;
  }
}

void  Game::Audio::Soundfont::loadSectionList(std::ifstream& file, const Game::Audio::Soundfont::Sf2Header& header, const Game::Audio::Soundfont::Sf2SectionHeader& section, std::vector<float>& samplesFloat, std::streampos position)
{
  // Matching sections
  const std::unordered_map<uint32_t, std::function<void()>> list_commands = {
    { Game::Utilities::str_to_key<uint32_t>("INFO"), std::bind(&Game::Audio::Soundfont::loadSectionListInfo, this, std::ref(file), std::ref(header), std::ref(section), position) },
    { Game::Utilities::str_to_key<uint32_t>("sdta"), std::bind(&Game::Audio::Soundfont::loadSectionListSdta, this, std::ref(file), std::ref(header), std::ref(section), std::ref(samplesFloat), position) },
    { Game::Utilities::str_to_key<uint32_t>("pdta"), std::bind(&Game::Audio::Soundfont::loadSectionListPdta, this, std::ref(file), std::ref(header), std::ref(section), std::ref(samplesFloat), position) }
  };

  auto command = list_commands.find(section.name);

  // Execute matching command
  if (command != list_commands.end())
    command->second();
  else
    std::cerr << "[Game::Soundfont::load]: Warning, unknow section name '" << Game::Utilities::key_to_str(section.name) << "' (section ignored)." << std::endl;
}

void  Game::Audio::Soundfont::loadSectionListInfo(std::ifstream& file, const Game::Audio::Soundfont::Sf2Header& header, const Game::Audio::Soundfont::Sf2SectionHeader& section, std::streampos position)
{
  Game::Audio::Soundfont::Soundfont::Sf2SubsectionHeader  info = {};

  for (std::streampos info_position = sizeof(section); info_position < section.size + 8; info_position += sizeof(info) + info.size)
  {
    info = {};

    // Read info header
    file.seekg(position + info_position, file.beg);
    Game::Utilities::read(file, &info);
    
    // Matching subsections
    std::unordered_map<uint32_t, std::function<void()>> info_commands = {
      { Game::Utilities::str_to_key<uint32_t>("ifil"), std::bind(&Game::Audio::Soundfont::loadSectionListInfoIfil, this, std::ref(file), std::ref(info)) },
      { Game::Utilities::str_to_key<uint32_t>("isng"), std::bind(&Game::Audio::Soundfont::loadSectionListInfoIsng, this, std::ref(file), std::ref(info)) },
      { Game::Utilities::str_to_key<uint32_t>("INAM"), std::bind(&Game::Audio::Soundfont::loadSectionListInfoInam, this, std::ref(file), std::ref(info)) },
      { Game::Utilities::str_to_key<uint32_t>("IROM"), std::bind(&Game::Audio::Soundfont::loadSectionListInfoIrom, this, std::ref(file), std::ref(info)) },
      { Game::Utilities::str_to_key<uint32_t>("VER"), std::bind(&Game::Audio::Soundfont::loadSectionListInfoVer, this, std::ref(file), std::ref(info)) },
      { Game::Utilities::str_to_key<uint32_t>("ICRD"), std::bind(&Game::Audio::Soundfont::loadSectionListInfoIcrd, this, std::ref(file), std::ref(info)) },
      { Game::Utilities::str_to_key<uint32_t>("IENG"), std::bind(&Game::Audio::Soundfont::loadSectionListInfoIeng, this, std::ref(file), std::ref(info)) },
      { Game::Utilities::str_to_key<uint32_t>("IPRD"), std::bind(&Game::Audio::Soundfont::loadSectionListInfoIprd, this, std::ref(file), std::ref(info)) },
      { Game::Utilities::str_to_key<uint32_t>("ICOP"), std::bind(&Game::Audio::Soundfont::loadSectionListInfoIcop, this, std::ref(file), std::ref(info)) },
      { Game::Utilities::str_to_key<uint32_t>("ICMT"), std::bind(&Game::Audio::Soundfont::loadSectionListInfoIcmt, this, std::ref(file), std::ref(info)) },
      { Game::Utilities::str_to_key<uint32_t>("ISFT"), std::bind(&Game::Audio::Soundfont::loadSectionListInfoIsft, this, std::ref(file), std::ref(info)) }
    };

    auto  command = info_commands.find(info.name);

    // Execute matching command
    if (command != info_commands.end())
      command->second();
    else
      std::cerr << "[Game::Soundfont::load]: Warning, unknow info name '" << Game::Utilities::key_to_str(info.name) << "' (info ignored)." << std::endl;
  }

  // TODO: remove this
#ifdef _DEBUG
  std::cout
    << "    [INFO] version:     " << this->info.version.first << "." << this->info.version.second << std::endl
    << "    [INFO] engine:      " << this->info.engine << std::endl
    << "    [INFO] name:        " << this->info.name << std::endl
    << "    [INFO] rom:         " << this->info.rom << std::endl
    << "    [INFO] rom version: " << this->info.romVersion.first << "." << this->info.romVersion.second << std::endl
    << "    [INFO] creation:    " << this->info.creation << std::endl
    << "    [INFO] author:      " << this->info.author << std::endl
    << "    [INFO] product:     " << this->info.product << std::endl
    << "    [INFO] copyright:   " << this->info.copyright << std::endl
    << "    [INFO] comment:     " << this->info.comment << std::endl
    << "    [INFO] tool:        " << this->info.tool << std::endl
    ;
#endif
}

void  Game::Audio::Soundfont::loadSectionListInfoIfil(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info)
{
  // Soundfont version (major/minor)
  Game::Utilities::read(file, &this->info.version.first);
  Game::Utilities::read(file, &this->info.version.second);
}

void  Game::Audio::Soundfont::loadSectionListInfoIsng(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info)
{
  // Optimized audio engine
  this->info.engine = loadString(file, info.size);
}

void  Game::Audio::Soundfont::loadSectionListInfoInam(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info)
{
  // Name of sound bank
  this->info.name = loadString(file, info.size);
}

void  Game::Audio::Soundfont::loadSectionListInfoIrom(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info)
{
  // Name of ROM to use
  this->info.rom = loadString(file, info.size);
}

void  Game::Audio::Soundfont::loadSectionListInfoVer(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info)
{
  // ROM version to use
  Game::Utilities::read(file, &this->info.romVersion.first);
  Game::Utilities::read(file, &this->info.romVersion.second);
}

void  Game::Audio::Soundfont::loadSectionListInfoIcrd(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info)
{
  // Date of creation
  this->info.creation = loadString(file, info.size);
}

void  Game::Audio::Soundfont::loadSectionListInfoIeng(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info)
{
  // Name of the author(s)
  this->info.author = loadString(file, info.size);
}

void  Game::Audio::Soundfont::loadSectionListInfoIprd(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info)
{
  // Name of the target product
  this->info.author = loadString(file, info.size);
}

void  Game::Audio::Soundfont::loadSectionListInfoIcop(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info)
{
  // Copyright license
  this->info.copyright = loadString(file, info.size);
}

void  Game::Audio::Soundfont::loadSectionListInfoIcmt(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info)
{
  // Comment
  this->info.comment = loadString(file, info.size);
}

void  Game::Audio::Soundfont::loadSectionListInfoIsft(std::ifstream& file, const Game::Audio::Soundfont::Sf2SubsectionHeader& info)
{
  // Tool used to create the sound bank
  this->info.tool = loadString(file, info.size);
}

std::string Game::Audio::Soundfont::loadString(std::ifstream& file, std::size_t size)
{
  std::vector<char> str(size + 1, '\0');
  
  // Read string
  Game::Utilities::read(file, str.data(), str.size() - 1);

  return str.data();
}

void  Game::Audio::Soundfont::loadSectionListSdta(std::ifstream& file, const Game::Audio::Soundfont::Sf2Header& header, const Game::Audio::Soundfont::Sf2SectionHeader& section, std::vector<float>& samplesFloat, std::streampos position)
{
  Game::Audio::Soundfont::Sf2SubsectionHeader sdta = {};
  std::vector<int16_t>                        samples16;
  std::vector<int8_t>                         samples24;

  for (std::streampos sdta_position = sizeof(section); sdta_position < section.size + 8; sdta_position += sizeof(sdta) + sdta.size)
  {
    sdta = {};

    // Read info header
    file.seekg(position + sdta_position, file.beg);
    Game::Utilities::read(file, &sdta);

    // Matching subsections
    std::unordered_map<uint32_t, std::function<void()>> sdta_commands = {
      { Game::Utilities::str_to_key<uint32_t>("smpl"), std::bind(&Game::Audio::Soundfont::loadSubsection<int16_t>, this, std::ref(file), sdta.size, std::ref(samples16)) },
      { Game::Utilities::str_to_key<uint32_t>("sm24"), std::bind(&Game::Audio::Soundfont::loadSubsection<int8_t>, this, std::ref(file), sdta.size, std::ref(samples24)) }
    };

    auto command = sdta_commands.find(sdta.name);

    // Execute matching regex if only one found
    if (command != sdta_commands.end())
      command->second();
    else
      std::cerr << "[Game::Soundfont::load]: Warning, unknow sdta name '" << Game::Utilities::key_to_str(sdta.name) << "' (sdta ignored)." << std::endl;
  }

  // TODO: remove this
#ifdef _DEBUG
  std::cout
    << "    [SDTA] samples16: " << samples16.size() << std::endl
    << "    [SDTA] samples24: " << samples24.size() << std::endl
    ;
#endif

  // Set 24 bits complementary to 0 if absent or incomplete
  if (samples24.size() < samples16.size())
    samples24.resize(samples16.size(), 0);

  // Allocate sample buffer
  samplesFloat.resize(samples16.size(), 0.f);

  // Convert RAW 24 bits samples to float
  for (auto index = 0; index < samples16.size(); index++)
    samplesFloat[index] = ((float)samples16[index] * 256.f + (float)samples24[index]) / (float)std::pow(2, 23);
}

void  Game::Audio::Soundfont::loadSectionListPdta(std::ifstream& file, const Game::Audio::Soundfont::Sf2Header& header, const Game::Audio::Soundfont::Sf2SectionHeader& section, std::vector<float>& samplesFloat, std::streampos position)
{
  Game::Audio::Soundfont::Sf2SubsectionHeader pdta = {};

  std::vector<Game::Audio::Soundfont::Sf2Preset>      presetHeaders;        // Header of each preset
  std::vector<Game::Audio::Soundfont::Sf2Bag>         presetBags;           // List of preset bags
  std::vector<Game::Audio::Soundfont::Sf2GenList>     presetGenerators;     // List of preset generators
  std::vector<Game::Audio::Soundfont::Sf2ModList>     presetModulators;     // List of preset modulators
  std::vector<Game::Audio::Soundfont::Sf2Instrument>  instrumentHeaders;    // Header of each instrument
  std::vector<Game::Audio::Soundfont::Sf2Bag>         instrumentBags;       // List of instrument bags
  std::vector<Game::Audio::Soundfont::Sf2GenList>     instrumentGenerators; // List of instrument generators
  std::vector<Game::Audio::Soundfont::Sf2ModList>     instrumentModulators; // List of instrument modulators
  std::vector<Game::Audio::Soundfont::Sf2Sample>      sampleHeaders;        // Description of each sample

  for (std::streampos pdta_position = sizeof(section); pdta_position < section.size + 8; pdta_position += sizeof(pdta) + pdta.size)
  {
    pdta = {};

    // Read info header
    file.seekg(position + pdta_position, file.beg);
    Game::Utilities::read(file, &pdta);

    // Matching subsections
    std::unordered_map<uint32_t, std::function<void()>> pdta_commands = {
      { Game::Utilities::str_to_key<uint32_t>("phdr"), std::bind(&Game::Audio::Soundfont::loadSubsection<Game::Audio::Soundfont::Sf2Preset>, this, std::ref(file), pdta.size, std::ref(presetHeaders)) },
      { Game::Utilities::str_to_key<uint32_t>("pbag"), std::bind(&Game::Audio::Soundfont::loadSubsection<Game::Audio::Soundfont::Sf2Bag>, this, std::ref(file), pdta.size, std::ref(presetBags)) },
      { Game::Utilities::str_to_key<uint32_t>("pmod"), std::bind(&Game::Audio::Soundfont::loadSubsection<Game::Audio::Soundfont::Sf2ModList>, this, std::ref(file), pdta.size, std::ref(presetModulators)) },
      { Game::Utilities::str_to_key<uint32_t>("pgen"), std::bind(&Game::Audio::Soundfont::loadSubsection<Game::Audio::Soundfont::Sf2GenList>, this, std::ref(file), pdta.size, std::ref(presetGenerators)) },
      { Game::Utilities::str_to_key<uint32_t>("inst"), std::bind(&Game::Audio::Soundfont::loadSubsection<Game::Audio::Soundfont::Sf2Instrument>, this, std::ref(file), pdta.size, std::ref(instrumentHeaders)) },
      { Game::Utilities::str_to_key<uint32_t>("ibag"), std::bind(&Game::Audio::Soundfont::loadSubsection<Game::Audio::Soundfont::Sf2Bag>, this, std::ref(file), pdta.size, std::ref(instrumentBags)) },
      { Game::Utilities::str_to_key<uint32_t>("imod"), std::bind(&Game::Audio::Soundfont::loadSubsection<Game::Audio::Soundfont::Sf2ModList>, this, std::ref(file), pdta.size, std::ref(instrumentModulators)) },
      { Game::Utilities::str_to_key<uint32_t>("igen"), std::bind(&Game::Audio::Soundfont::loadSubsection<Game::Audio::Soundfont::Sf2GenList>, this, std::ref(file), pdta.size, std::ref(instrumentGenerators)) },
      { Game::Utilities::str_to_key<uint32_t>("shdr"), std::bind(&Game::Audio::Soundfont::loadSubsection<Game::Audio::Soundfont::Sf2Sample>, this, std::ref(file), pdta.size, std::ref(sampleHeaders)) }
    };

    auto command = pdta_commands.find(pdta.name);

    // Execute matching command
    if (command != pdta_commands.end())
      command->second();
    else
      std::cerr << "[Game::Soundfont::load]: Warning, unknow pdta name '" << Game::Utilities::key_to_str(pdta.name) << "' (sdta ignored)." << std::endl;
  }

  // TODO: remove this
#ifdef _DEBUG
  std::cout
    << "    [PDTA] presets:               " << presetHeaders.size() << std::endl
    << "    [PDTA] preset bags:           " << presetBags.size() << std::endl
    << "    [PDTA] preset modulators:     " << presetModulators.size() << std::endl
    << "    [PDTA] preset generators:     " << presetGenerators.size() << std::endl
    << "    [PDTA] instruments:           " << instrumentHeaders.size() << std::endl
    << "    [PDTA] instrument bags:       " << instrumentBags.size() << std::endl
    << "    [PDTA] instrument modulators: " << instrumentModulators.size() << std::endl
    << "    [PDTA] instrument generators: " << instrumentGenerators.size() << std::endl
    << "    [PDTA] samples:               " << sampleHeaders.size() << std::endl
    << std::endl;
#endif

  // Build presets
  for (int preset_index = 0; preset_index < presetHeaders.size() - 1; preset_index++) {
    Game::Audio::Soundfont::Preset&   preset = presets[(uint8_t)presetHeaders[preset_index].bank][(uint8_t)presetHeaders[preset_index].midi];
    Game::Audio::Soundfont::Generator preset_generator_global;
    int                               preset_bag_start = presetHeaders[preset_index + 0].bag;
    int                               preset_bag_end = presetHeaders[preset_index + 1].bag;

    // Get name of the preset
    preset.name = (const char*)presetHeaders[preset_index].name;

    // Get preset bags
    for (int preset_bag_index = preset_bag_start; preset_bag_index < preset_bag_end; preset_bag_index++) {
      Game::Audio::Soundfont::Soundfont::Generator  preset_generator_local = preset_generator_global;

      // Get generators of preset bag
      for (int preset_generator_index = presetBags[preset_bag_index].generator; preset_generator_index < presetBags[preset_bag_index + 1].generator; preset_generator_index++) {
        switch (presetGenerators[preset_generator_index].operation) {
        case Game::Audio::Soundfont::Sf2Generator::StartAddrsOffset:
        case Game::Audio::Soundfont::Sf2Generator::EndAddrsOffset:
        case Game::Audio::Soundfont::Sf2Generator::StartLoopAddrsOffset:
        case Game::Audio::Soundfont::Sf2Generator::EndLoopAddrsOffset:
        case Game::Audio::Soundfont::Sf2Generator::StartAddrsCoarseOffset:
        case Game::Audio::Soundfont::Sf2Generator::EndAddrsCoarseOffset:
        case Game::Audio::Soundfont::Sf2Generator::StartLoopAddrsCoarseOffset:
        case Game::Audio::Soundfont::Sf2Generator::EndLoopAddrsCoarseOffset:
        case Game::Audio::Soundfont::Sf2Generator::Keynum:
        case Game::Audio::Soundfont::Sf2Generator::Velocity:
        case Game::Audio::Soundfont::Sf2Generator::SampleMode:
        case Game::Audio::Soundfont::Sf2Generator::ExclusiveClass:
        case Game::Audio::Soundfont::Sf2Generator::OverridingRootKey:
          std::cerr << "[Game::Soundfont::load]: Warning, invalid generator in preset zone (" << presetGenerators[preset_generator_index].operation << "), ignored." << std::endl;
          break;

        default:
          preset_generator_local[presetGenerators[preset_generator_index].operation] = presetGenerators[preset_generator_index].amount;
          break;
        }
      }

      // TODO: get modulators
      for (int preset_modulator_index = presetBags[preset_bag_index].modulator; preset_modulator_index < presetBags[preset_bag_index + 1].modulator; preset_modulator_index++)
        ;

      // First bag without instrument is a global generator, other bags without instrument are ignored
      if (preset_generator_local[Sf2Generator::Instrument].s_amount == -1) {
        if (preset_bag_index == preset_bag_start)
          preset_generator_global = preset_generator_local;
        else
          std::cerr << "[Game::Soundfont::load]: Warning, invalid preset zone (no instrument), ignored." << std::endl;
        continue;
      }

      Game::Audio::Soundfont::Soundfont::Preset::Instrument instrument;
      int                                                   instrument_bag_start = instrumentHeaders[preset_generator_local[Game::Audio::Soundfont::Sf2Generator::Instrument].s_amount + 0].bag;
      int                                                   instrument_bag_end = instrumentHeaders[preset_generator_local[Game::Audio::Soundfont::Sf2Generator::Instrument].s_amount + 1].bag;

      // Name of the instrument
      instrument.name = (const char*)instrumentHeaders[preset_generator_local[Game::Audio::Soundfont::Sf2Generator::Instrument].s_amount].name;

      // Save current preset generators in instrument
      instrument.generator = preset_generator_local;

      Game::Audio::Soundfont::Soundfont::Generator  instrument_generator_global;

      // Get instrument bags
      for (int instrument_bag_index = instrument_bag_start; instrument_bag_index < instrument_bag_end; instrument_bag_index++) {
        Game::Audio::Soundfont::Soundfont::Preset::Instrument::Bag  bag;

        // Sync with global instrument generator
        bag.generator = instrument_generator_global;

        // Get generators of instrument bag
        for (int instrument_generator_index = instrumentBags[instrument_bag_index].generator; instrument_generator_index < instrumentBags[instrument_bag_index + 1].generator; instrument_generator_index++)
          bag.generator[instrumentGenerators[instrument_generator_index].operation] = instrumentGenerators[instrument_generator_index].amount;

        // TODO: get modulators
        for (int instrument_modulator_index = instrumentBags[instrument_bag_index].modulator; instrument_modulator_index < instrumentBags[instrument_bag_index + 1].modulator; instrument_modulator_index++)
          ;

        // First bag without sample ID is a global generator, other bags without sample ID are ignored
        if (bag.generator[Game::Audio::Soundfont::Sf2Generator::SampleId].s_amount == -1) {
          if (instrument_bag_index == instrument_bag_start)
            instrument_generator_global = bag.generator;
          else
            std::cerr << "[Game::Soundfont::load]: Warning, invalid instrument zone (no sample ID), ignored." << std::endl;
          continue;
        }

        // Add bag to instrument
        instrument.bags.push_back(std::move(bag));
      }

      // Add new instrument to preset
      preset.instruments.push_back(std::move(instrument));
    }
  }

  // Build samples
  for (const auto& sample : sampleHeaders)
  {
    // Ignore last sample
    if (&sample == &sampleHeaders.back())
      break;

    // Add a new sample
    samples.emplace_back();

    // Default values
    samples.back().name = "";
    samples.back().samples = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
    samples.back().start = 2;
    samples.back().end = 6;
    samples.back().rate = 22050;
    samples.back().key = 60;
    samples.back().correction = 0;
    samples.back().type = Game::Audio::Soundfont::Sample::Link::Mono;
    samples.back().link = 0;

    // Only support stereo
    if (sample.sampleType != Game::Audio::Soundfont::Sf2Sample::Sf2SampleLink::Mono &&
      sample.sampleType != Game::Audio::Soundfont::Sf2Sample::Sf2SampleLink::Left &&
      sample.sampleType != Game::Audio::Soundfont::Sf2Sample::Sf2SampleLink::Right) {
      std::cerr << "[Game::Soundfont::load]: Warning, unsupported sample type (" << (int)sample.sampleType << "), ignored." << std::endl;
      continue;
    }

    // Loop should be contained in samples and at least one sample long
    if (sample.loopStart < sample.sampleStart || sample.loopStart > sample.loopEnd ||
      sample.loopEnd < sample.sampleStart || sample.loopEnd > sample.loopEnd ||
      sample.loopStart >= sample.loopEnd ||
      sample.sampleStart >= sample.sampleEnd ||
      sample.sampleStart >= samplesFloat.size() || sample.sampleEnd >= samplesFloat.size()) {
      std::cerr << "[Game::Soundfont::load]: Warning, invalid sample points, ignored." << std::endl;
      continue;
    }

    // Copy attributes
    samples.back().name = (const char*)sample.name;
    samples.back().start = sample.loopStart - sample.sampleStart;
    samples.back().end = sample.loopEnd - sample.sampleStart;
    samples.back().rate = sample.sampleRate;
    samples.back().key = sample.pitchOriginal;
    samples.back().correction = sample.pitchCorrection;
    samples.back().type = (Game::Audio::Soundfont::Sample::Link)sample.sampleType;
    samples.back().link = sample.sampleLink;

    // Copy samples
    samples.back().samples.resize(sample.sampleEnd - sample.sampleStart, 0.f);
    for (auto index = sample.sampleStart; index < sample.sampleEnd; index++)
      samples.back().samples[index - sample.sampleStart] = samplesFloat[index];
  }
}

Game::Audio::Soundfont::Generator::Generator() :
  std::array<Game::Audio::Soundfont::Sf2Amount, Game::Audio::Soundfont::Sf2Generator::Count>()
{
  // Default values of generators
  (*this)[Game::Audio::Soundfont::Sf2Generator::StartAddrsOffset].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::EndAddrsOffset].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::StartLoopAddrsOffset].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::EndLoopAddrsOffset].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::StartAddrsCoarseOffset].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::ModLfoToPitch].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::VibLfoToPitch].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::ModEnvToPitch].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::InitialFilterFc].u_amount = 13500;
  (*this)[Game::Audio::Soundfont::Sf2Generator::InitialFilterQ].u_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::ModLfoToFilterFc].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::ModEnvToFilterFc].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::EndAddrsCoarseOffset].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::ModLfoToVolume].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::Unused1].u_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::ChorusEffectsSend].u_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::ReverbEffectsSend].u_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::Pan].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::Unused2].u_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::Unused3].u_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::Unused4].u_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::DelayModLfo].s_amount = -12000;
  (*this)[Game::Audio::Soundfont::Sf2Generator::FreqModLfo].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::DelayVibLfo].s_amount = -12000;
  (*this)[Game::Audio::Soundfont::Sf2Generator::FreqVibLfo].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::DelayModEnv].s_amount = -12000;
  (*this)[Game::Audio::Soundfont::Sf2Generator::AttackModEnv].s_amount = -12000;
  (*this)[Game::Audio::Soundfont::Sf2Generator::HoldModEnv].s_amount = -12000;
  (*this)[Game::Audio::Soundfont::Sf2Generator::DecayModEnv].s_amount = -12000;
  (*this)[Game::Audio::Soundfont::Sf2Generator::SustainModEnv].u_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::ReleaseModEnv].s_amount = -12000;
  (*this)[Game::Audio::Soundfont::Sf2Generator::KeynumToModEnvHold].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::KeynumToModEnvDecay].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::DelayVolEnv].s_amount = -12000;
  (*this)[Game::Audio::Soundfont::Sf2Generator::AttackVolEnv].s_amount = -12000;
  (*this)[Game::Audio::Soundfont::Sf2Generator::HoldVolEnv].s_amount = -12000;
  (*this)[Game::Audio::Soundfont::Sf2Generator::DecayVolEnv].s_amount = -12000;
  (*this)[Game::Audio::Soundfont::Sf2Generator::SustainVolEnv].u_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::ReleaseVolEnv].s_amount = -12000;
  (*this)[Game::Audio::Soundfont::Sf2Generator::KeynumToVolEnvHold].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::KeynumToVolEnvDecay].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::Instrument].s_amount = -1;
  (*this)[Game::Audio::Soundfont::Sf2Generator::Reserved1].u_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::KeyRange].range = { 0, 127 };
  (*this)[Game::Audio::Soundfont::Sf2Generator::VelocityRange].range = { 0, 127 };
  (*this)[Game::Audio::Soundfont::Sf2Generator::StartLoopAddrsCoarseOffset].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::Keynum].s_amount = -1;
  (*this)[Game::Audio::Soundfont::Sf2Generator::Velocity].s_amount = -1;
  (*this)[Game::Audio::Soundfont::Sf2Generator::InitialAttenuation].u_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::Reserved2].u_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::EndLoopAddrsCoarseOffset].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::CoarseTune].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::FineTune].s_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::SampleId].u_amount = -1;
  (*this)[Game::Audio::Soundfont::Sf2Generator::SampleMode].u_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::Reserved3].u_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::ScaleTuning].u_amount = 100;
  (*this)[Game::Audio::Soundfont::Sf2Generator::ExclusiveClass].u_amount = 0;
  (*this)[Game::Audio::Soundfont::Sf2Generator::OverridingRootKey].s_amount = -1;
  (*this)[Game::Audio::Soundfont::Sf2Generator::Unused5].u_amount = 0;
}