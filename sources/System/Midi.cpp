#include <fstream>
#include <vector>
#include <iostream>
#include <functional>
#include <unordered_map>

#include "System/Midi.hpp"

void  toWave(const std::string& filename, const std::vector<int16_t>& samples16)
{
  std::ofstream  out(filename, std::ofstream::binary);

  uint32_t byte4;
  uint16_t byte2;

  out.write("RIFF", 4);
  byte4 = (int)samples16.size() * 2 + 36;
  out.write((char*)&byte4, sizeof(byte4));
  out.write("WAVE", 4);
  out.write("fmt ", 4);
  byte4 = 16;
  out.write((char*)&byte4, sizeof(byte4));
  byte2 = 1;
  out.write((char*)&byte2, sizeof(byte2));
  byte2 = 1;
  out.write((char*)&byte2, sizeof(byte2));
  byte4 = 22050;
  out.write((char*)&byte4, sizeof(byte4));
  byte4 = 22050 * 2;
  out.write((char*)&byte4, sizeof(byte4));
  byte2 = 2;
  out.write((char*)&byte2, sizeof(byte2));
  byte2 = 16;
  out.write((char*)&byte2, sizeof(byte2));
  out.write("data", 4);
  byte4 = (int)samples16.size() * 2;
  out.write((char*)&byte4, sizeof(byte4));
  out.write((char*)samples16.data(), samples16.size() * 2);
}

Game::Midi::Midi()
{}

Game::Midi::SoundFont::SoundFont(const std::string& filename) :
  info(),
  samples16(),
  samples24()
{
  // Load file
  load(filename);
}

void  Game::Midi::SoundFont::load(const std::string& filename)
{
  std::ifstream file(filename, std::ifstream::binary);

  // Check that file is correctly open
  if (file.good() == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  Sf2Header header = { 0 };

  // Read RIFF header
  file.read((char*)&header, sizeof(header));

  // TODO: remove this
  std::cout
    << "Filename: " << filename << std::endl
    << std::endl
    << "[HEADER] magic: " << SoundFont::key_to_str(header.magic) << std::endl
    << "[HEADER] size:  " << header.size << std::endl
    << "[HEADER] type:  " << SoundFont::key_to_str(header.type) << std::endl
    ;

  // Check header data
  if (std::memcmp(&header.magic, "RIFF", sizeof(header.magic)) != 0 ||
    std::memcmp(&header.type, "sfbk", sizeof(header.magic)) != 0)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Load sections of SoundFont
  loadSections(file, header);
}

void  Game::Midi::SoundFont::loadSections(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header)
{
  Game::Midi::SoundFont::Sf2SectionHeader section = { 0 };

  for (std::streampos section_position = sizeof(header); section_position < header.size + 8; section_position += section.size + 8)
  {
    section = { 0 };

    // Read section header
    file.seekg(section_position, file.beg);
    file.read((char*)&section, sizeof(section));

    // TODO: remove this
    std::cout
      << std::endl
      << "[SECTION] type: " << SoundFont::key_to_str(section.type) << std::endl
      << "[SECTION] size: " << section.size << std::endl
      << "[SECTION] name: " << SoundFont::key_to_str(section.name) << std::endl
      ;

    // Matching sections
    std::unordered_map<uint32_t, std::function<void()>> section_commands = {
      { SoundFont::str_to_key("LIST"), std::bind(&Game::Midi::SoundFont::loadSectionList, this, std::ref(file), std::ref(header), std::ref(section), section_position) }
    };

    auto command = section_commands.find(section.type);

    // Execute matching command
    if (command != section_commands.end())
      command->second();
    else
      std::cerr << "[Game::Midi::SoundFont::Load]: Warning, unknow section type '" << SoundFont::key_to_str(section.type) << "' (section ignored)." << std::endl;  
  }
}

void  Game::Midi::SoundFont::loadSectionList(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header, const Game::Midi::SoundFont::Sf2SectionHeader& section, std::streampos position)
{
  // Matching sections
  std::unordered_map<uint32_t, std::function<void()>> list_commands = {
    { SoundFont::str_to_key("INFO"), std::bind(&SoundFont::loadSectionListInfo, this, std::ref(file), std::ref(header), std::ref(section), position) },
    { SoundFont::str_to_key("sdta"), std::bind(&SoundFont::loadSectionListSdta, this, std::ref(file), std::ref(header), std::ref(section), position) },
    { SoundFont::str_to_key("pdta"), std::bind(&SoundFont::loadSectionListPdta, this, std::ref(file), std::ref(header), std::ref(section), position) }
  };

  auto command = list_commands.find(section.name);

  // Execute matching command
  if (command != list_commands.end())
    command->second();
  else
    std::cerr << "[Game::Midi::SoundFont::load]: Warning, unknow section name '" << SoundFont::key_to_str(section.name) << "' (section ignored)." << std::endl;
}

void  Game::Midi::SoundFont::loadSectionListInfo(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header, const Game::Midi::SoundFont::Sf2SectionHeader& section, std::streampos position)
{
  Game::Midi::SoundFont::Sf2SubsectionHeader  info = { 0 };

  for (std::streampos info_position = sizeof(section); info_position < section.size + 8; info_position += sizeof(info) + info.size)
  {
    info = { 0 };

    // Read info header
    file.seekg(position + info_position, file.beg);
    file.read((char*)&info, sizeof(info));
    
    // Matching subsections
    std::unordered_map<uint32_t, std::function<void()>> info_commands = {
      { SoundFont::str_to_key("ifil"), std::bind(&SoundFont::loadSectionListInfoIfil, this, std::ref(file), std::ref(info)) },
      { SoundFont::str_to_key("isng"), std::bind(&SoundFont::loadSectionListInfoIsng, this, std::ref(file), std::ref(info)) },
      { SoundFont::str_to_key("INAM"), std::bind(&SoundFont::loadSectionListInfoInam, this, std::ref(file), std::ref(info)) },
      { SoundFont::str_to_key("IROM"), std::bind(&SoundFont::loadSectionListInfoIrom, this, std::ref(file), std::ref(info)) },
      { SoundFont::str_to_key("VER"), std::bind(&SoundFont::loadSectionListInfoVer, this, std::ref(file), std::ref(info)) },
      { SoundFont::str_to_key("ICRD"), std::bind(&SoundFont::loadSectionListInfoIcrd, this, std::ref(file), std::ref(info)) },
      { SoundFont::str_to_key("IENG"), std::bind(&SoundFont::loadSectionListInfoIeng, this, std::ref(file), std::ref(info)) },
      { SoundFont::str_to_key("IPRD"), std::bind(&SoundFont::loadSectionListInfoIprd, this, std::ref(file), std::ref(info)) },
      { SoundFont::str_to_key("ICOP"), std::bind(&SoundFont::loadSectionListInfoIcop, this, std::ref(file), std::ref(info)) },
      { SoundFont::str_to_key("ICMT"), std::bind(&SoundFont::loadSectionListInfoIcmt, this, std::ref(file), std::ref(info)) },
      { SoundFont::str_to_key("ISFT"), std::bind(&SoundFont::loadSectionListInfoIsft, this, std::ref(file), std::ref(info)) }
    };

    auto  command = info_commands.find(info.name);

    // Execute matching command
    if (command != info_commands.end())
      command->second();
    else
      std::cerr << "[Game::Midi::SoundFont::load]: Warning, unknow info name '" << SoundFont::key_to_str(info.name) << "' (info ignored)." << std::endl;
  }

  // TODO: remove this
  std::cout
    << "  [INFO] version: " << this->info.version.first << "." << this->info.version.second << std::endl
    << "  [INFO] engine: " << this->info.engine << std::endl
    << "  [INFO] name: " << this->info.name << std::endl
    << "  [INFO] rom: " << this->info.rom << std::endl
    << "  [INFO] rom version: " << this->info.romVersion.first << "." << this->info.romVersion.second << std::endl
    << "  [INFO] creation: " << this->info.creation << std::endl
    << "  [INFO] author: " << this->info.author << std::endl
    << "  [INFO] product: " << this->info.product << std::endl
    << "  [INFO] copyright: " << this->info.copyright << std::endl
    << "  [INFO] comment: " << this->info.comment << std::endl
    << "  [INFO] tool: " << this->info.tool << std::endl
    ;
}

void  Game::Midi::SoundFont::loadSectionListInfoIfil(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // SoundFont version (major/minor)
  file.read((char*)&this->info.version.first, sizeof(this->info.version.first));
  file.read((char*)&this->info.version.second, sizeof(this->info.version.second));
}

void  Game::Midi::SoundFont::loadSectionListInfoIsng(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Optimized audio engine
  this->info.engine = loadString(file, info.size);
}

void  Game::Midi::SoundFont::loadSectionListInfoInam(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Name of sound bank
  this->info.name = loadString(file, info.size);
}

void  Game::Midi::SoundFont::loadSectionListInfoIrom(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Name of ROM to use
  this->info.rom = loadString(file, info.size);
}

void  Game::Midi::SoundFont::loadSectionListInfoVer(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // ROM version to use
  file.read((char*)&this->info.romVersion.first, sizeof(this->info.romVersion.first));
  file.read((char*)&this->info.romVersion.second, sizeof(this->info.romVersion.second));
}

void  Game::Midi::SoundFont::loadSectionListInfoIcrd(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Date of creation
  this->info.creation = loadString(file, info.size);
}

void  Game::Midi::SoundFont::loadSectionListInfoIeng(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Name of the author(s)
  this->info.author = loadString(file, info.size);
}

void  Game::Midi::SoundFont::loadSectionListInfoIprd(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Name of the target product
  this->info.author = loadString(file, info.size);
}

void  Game::Midi::SoundFont::loadSectionListInfoIcop(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Copyright license
  this->info.copyright = loadString(file, info.size);
}

void  Game::Midi::SoundFont::loadSectionListInfoIcmt(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Comment
  this->info.comment = loadString(file, info.size);
}

void  Game::Midi::SoundFont::loadSectionListInfoIsft(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Tool used to create the sound bank
  this->info.tool = loadString(file, info.size);
}

std::string Game::Midi::SoundFont::loadString(std::ifstream& file, std::size_t size)
{
  std::vector<char> str(size + 1, '\0');
  
  // Read string
  file.read(str.data(), size);

  return str.data();
}

void  Game::Midi::SoundFont::loadSectionListSdta(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header, const Game::Midi::SoundFont::Sf2SectionHeader& section, std::streampos position)
{
  Game::Midi::SoundFont::Sf2SubsectionHeader  sdta = { 0 };

  for (std::streampos sdta_position = sizeof(section); sdta_position < section.size + 8; sdta_position += sizeof(sdta) + sdta.size)
  {
    sdta = { 0 };

    // Read info header
    file.seekg(position + sdta_position, file.beg);
    file.read((char*)&sdta, sizeof(sdta));

    // Matching subsections
    std::unordered_map<uint32_t, std::function<void()>> sdta_commands = {
      { SoundFont::str_to_key("smpl"), std::bind(&Game::Midi::SoundFont::loadSubsection<int16_t>, this, std::ref(file), sdta.size, std::ref(samples16)) },
      { SoundFont::str_to_key("sm24"), std::bind(&Game::Midi::SoundFont::loadSubsection<int8_t>, this, std::ref(file), sdta.size, std::ref(samples24)) }
    };

    auto command = sdta_commands.find(sdta.name);

    // Execute matching regex if only one found
    if (command != sdta_commands.end())
      command->second();
    else
      std::cerr << "[Game::Midi::SoundFont::load]: Warning, unknow sdta name '" << SoundFont::key_to_str(sdta.name) << "' (sdta ignored)." << std::endl;
  }

  // TODO: remove this
  std::cout
    << "  [SDTA] samples16: " << samples16.size() << std::endl
    << "  [SDTA] samples24: " << samples24.size() << std::endl
    ;
}

void  Game::Midi::SoundFont::loadSectionListPdta(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header, const Game::Midi::SoundFont::Sf2SectionHeader& section, std::streampos position)
{
  SoundFont::Sf2SubsectionHeader  pdta = { 0 };

  std::vector<SoundFont::Sf2Preset>     presetHeaders;        // Header of each preset
  std::vector<SoundFont::Sf2Bag>        presetBags;           // List of preset bags
  std::vector<SoundFont::Sf2GenList>    presetGenerators;     // List of preset generators
  std::vector<SoundFont::Sf2ModList>    presetModulators;     // List of preset modulators
  std::vector<SoundFont::Sf2Instrument> instrumentHeaders;    // Header of each instrument
  std::vector<SoundFont::Sf2Bag>        instrumentBags;       // List of instrument bags
  std::vector<SoundFont::Sf2GenList>    instrumentGenerators; // List of instrument generators
  std::vector<SoundFont::Sf2ModList>    instrumentModulators; // List of instrument modulators
  std::vector<SoundFont::Sf2Sample>     sampleHeaders;        // Description of each sample

  for (std::streampos pdta_position = sizeof(section); pdta_position < section.size + 8; pdta_position += sizeof(pdta) + pdta.size)
  {
    pdta = { 0 };

    // Read info header
    file.seekg(position + pdta_position, file.beg);
    file.read((char*)&pdta, sizeof(pdta));

    // Matching subsections
    std::unordered_map<uint32_t, std::function<void()>> pdta_commands = {
      { SoundFont::str_to_key("phdr"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2Preset>, this, std::ref(file), pdta.size, std::ref(presetHeaders)) },
      { SoundFont::str_to_key("pbag"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2Bag>, this, std::ref(file), pdta.size, std::ref(presetBags)) },
      { SoundFont::str_to_key("pmod"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2ModList>, this, std::ref(file), pdta.size, std::ref(presetModulators)) },
      { SoundFont::str_to_key("pgen"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2GenList>, this, std::ref(file), pdta.size, std::ref(presetGenerators)) },
      { SoundFont::str_to_key("inst"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2Instrument>, this, std::ref(file), pdta.size, std::ref(instrumentHeaders)) },
      { SoundFont::str_to_key("ibag"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2Bag>, this, std::ref(file), pdta.size, std::ref(instrumentBags)) },
      { SoundFont::str_to_key("imod"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2ModList>, this, std::ref(file), pdta.size, std::ref(instrumentModulators)) },
      { SoundFont::str_to_key("igen"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2GenList>, this, std::ref(file), pdta.size, std::ref(instrumentGenerators)) },
      { SoundFont::str_to_key("shdr"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2Sample>, this, std::ref(file), pdta.size, std::ref(sampleHeaders)) }
    };

    auto command = pdta_commands.find(pdta.name);

    // Execute matching command
    if (command != pdta_commands.end())
      command->second();
    else
      std::cerr << "[Game::Midi::SoundFont::load]: Warning, unknow pdta name '" << SoundFont::key_to_str(pdta.name) << "' (sdta ignored)." << std::endl;
  }

  // TODO: remove this
  std::cout
    << "  [PDTA] presets:               " << presetHeaders.size() << std::endl
    << "  [PDTA] preset bags:           " << presetBags.size() << std::endl
    << "  [PDTA] preset modulators:     " << presetModulators.size() << std::endl
    << "  [PDTA] preset generators:     " << presetGenerators.size() << std::endl
    << "  [PDTA] instruments:           " << instrumentHeaders.size() << std::endl
    << "  [PDTA] instrument bags:       " << instrumentBags.size() << std::endl
    << "  [PDTA] instrument modulators: " << instrumentModulators.size() << std::endl
    << "  [PDTA] instrument generators: " << instrumentGenerators.size() << std::endl
    << "  [PDTA] samples:               " << sampleHeaders.size() << std::endl
    ;

  // TODO: build presets, instruments and samples
}