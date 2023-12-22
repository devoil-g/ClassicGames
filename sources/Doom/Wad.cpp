#include <cmath>
#include <functional>
#include <iostream>
#include <list>
#include <regex>

#include "Doom/Wad.hpp"
#include "System/Config.hpp"
#include "System/Utilities.hpp"

void  DOOM::Wad::load(const std::filesystem::path& path)
{
  std::ifstream file(path, std::ifstream::binary);

  // Check if failed to open file
  if (file.bad() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // WAD file header
  int8_t  identification[4 + 1] = { 0 };
  int32_t numlumps = 0;
  int32_t infotableofs = 0;

  // Get header data from file
  Game::Utilities::read(file, (char*)identification, sizeof(int8_t) * 4);
  Game::Utilities::read(file, &numlumps);
  Game::Utilities::read(file, &infotableofs);

  // Check for valid file data
  if ((std::string((const char*)identification) != "IWAD" && std::string((const char*)identification) != "PWAD") || numlumps < 0 || infotableofs < 0)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Load base level if file is an extension
  if (std::string((const char*)identification) != "IWAD")
    load(Game::Config::ExecutablePath / "assets" / "levels" / "doom.wad");

  // Load lumps
  loadLumps(file, numlumps, infotableofs);
}

void  DOOM::Wad::loadLumps(std::ifstream& file, int32_t const numlumps, int32_t const infotableofs)
{
  std::pair<uint8_t, uint8_t> level = { 0, 0 };
  DOOM::Wad::Lump	      lump = {};
  int32_t		      iterator = 0;

  // Initialize regex command map
  const std::list<std::pair<std::regex, std::function<void()>>> commands_regex =
  {
    // Scoped lumps
    { std::regex("F_START"), std::bind(&DOOM::Wad::loadResourceFlats, this, std::ref(file), numlumps, infotableofs, std::ref(iterator)) },
    { std::regex("S_START"), std::bind(&DOOM::Wad::loadResourceSprites, this, std::ref(file), numlumps, infotableofs, std::ref(iterator)) },
    { std::regex("P_START"), std::bind(&DOOM::Wad::loadResourcePatches, this, std::ref(file), numlumps, infotableofs, std::ref(iterator)) },

    // Resources lumps
    { std::regex("PLAYPAL"), std::bind(&DOOM::Wad::loadResourcePlaypal, this, std::ref(file), std::ref(lump)) },
    { std::regex("COLORMAP"), std::bind(&DOOM::Wad::loadResourceColormap, this, std::ref(file), std::ref(lump)) },
    { std::regex("TEXTURE1"), std::bind(&DOOM::Wad::loadResourceTexture, this, std::ref(file), std::ref(lump)) },
    { std::regex("TEXTURE2"), std::bind(&DOOM::Wad::loadResourceTexture, this, std::ref(file), std::ref(lump)) },
    { std::regex("PNAMES"), std::bind(&DOOM::Wad::loadResourcePnames, this, std::ref(file), std::ref(lump)) },
    { std::regex("ENDOOM"), std::bind(&DOOM::Wad::loadResourceEndoom, this, std::ref(file), std::ref(lump)) },
    { std::regex("GENMIDI"), std::bind(&DOOM::Wad::loadResourceGenmidi, this, std::ref(file), std::ref(lump)) },
    { std::regex("DMXGUS[[:alnum:]]?"), std::bind(&DOOM::Wad::loadIgnore, this) },
    { std::regex("D_[_[:alnum:]]+"), std::bind(&DOOM::Wad::loadResourceMusic, this, std::ref(file), std::ref(lump)) },
    { std::regex("DS[_[:alnum:]]+"), std::bind(&DOOM::Wad::loadResourceSound, this, std::ref(file), std::ref(lump)) },
    { std::regex("DP[_[:alnum:]]+"), std::bind(&DOOM::Wad::loadIgnore, this) },
    { std::regex("DEMO[[:digit:]]"), std::bind(&DOOM::Wad::loadResourceDemox, this, std::ref(file), std::ref(lump)) },

    // Menu resources lumps
    { std::regex("HELP[[:digit:]]?"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
    { std::regex("TITLEPIC"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
    { std::regex("CREDIT"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
    { std::regex("VICTORY2"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
    { std::regex("PFUB1"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
    { std::regex("PFUB2"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
    { std::regex("INTERPIC"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
    { std::regex("ENDPIC"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
    { std::regex("END[[:digit:]]"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
    { std::regex("AMMNUM[[:digit:]]"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
    { std::regex("ST[[:alnum:]]+"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
    { std::regex("WI[[:alnum:]]+"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
    { std::regex("M_[[:alnum:]]+"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
    { std::regex("BRDR_[[:alnum:]]+"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
    { std::regex("CWILV[[:digit:]]+"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
    { std::regex("BOSSBACK"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },

    // Level lumps
    { std::regex("VERTEXES"), std::bind(&DOOM::Wad::loadLevelVertexes, this, std::ref(file), std::ref(lump), std::cref(level)) },
    { std::regex("SECTORS"), std::bind(&DOOM::Wad::loadLevelSectors, this, std::ref(file), std::ref(lump), std::cref(level)) },
    { std::regex("SIDEDEFS"), std::bind(&DOOM::Wad::loadLevelSidedefs, this, std::ref(file), std::ref(lump), std::cref(level)) },
    { std::regex("LINEDEFS"), std::bind(&DOOM::Wad::loadLevelLinedefs, this, std::ref(file), std::ref(lump), std::cref(level)) },
    { std::regex("SEGS"), std::bind(&DOOM::Wad::loadLevelSegs, this, std::ref(file), std::ref(lump), std::cref(level)) },
    { std::regex("SSECTORS"), std::bind(&DOOM::Wad::loadLevelSsectors, this, std::ref(file), std::ref(lump), std::cref(level)) },
    { std::regex("NODES"), std::bind(&DOOM::Wad::loadLevelNodes, this, std::ref(file), std::ref(lump), std::cref(level)) },
    { std::regex("REJECT"), std::bind(&DOOM::Wad::loadLevelReject, this, std::ref(file), std::ref(lump), std::cref(level)) },
    { std::regex("BLOCKMAP"), std::bind(&DOOM::Wad::loadLevelBlockmap, this, std::ref(file), std::ref(lump), std::cref(level)) },
    { std::regex("THINGS"), std::bind(&DOOM::Wad::loadLevelThings, this, std::ref(file), std::ref(lump), std::cref(level)) },
    { std::regex("E[[:digit:]]M[[:digit:]]"), std::bind(&DOOM::Wad::loadLevelExmy, this, std::ref(lump), std::ref(level)) },
    { std::regex("MAP[[:digit:]][[:digit:]]"), std::bind(&DOOM::Wad::loadLevelMapxy, this, std::ref(lump), std::ref(level)) }
  };

  // Process every lump of file
  for (iterator = 0; iterator < numlumps; iterator++)
  {
    // Read lump header data from file
    file.seekg(infotableofs + iterator * sizeof(DOOM::Wad::Lump), file.beg);
    Game::Utilities::read(file, &lump);

    // Force name format
    uppercase(lump.name);

    // Remove each non-matching regex
    auto command = std::find_if(commands_regex.begin(), commands_regex.end(), [lump](const auto& regex) { return std::regex_match(Game::Utilities::key_to_str(lump.name).c_str(), regex.first) == true; });

    // Execute matching regex
    if (command == commands_regex.end())
      std::cerr << "[Wad::load]: Warning, unknown lump name '" << Game::Utilities::key_to_str(lump.name) << "'." << std::endl;
    else
      command->second();
  }
}

void  DOOM::Wad::loadResourceFlats(std::ifstream& file, int32_t const numlumps, int32_t const infotableofs, int32_t& iterator)
{
  // Process every lump in scope
  for (; iterator < numlumps; iterator++)
  {
    // Lump header data structure
    DOOM::Wad::Lump lump;

    // Read lump header data from file
    file.seekg(infotableofs + iterator * sizeof(DOOM::Wad::Lump), file.beg);
    Game::Utilities::read(file, &lump);

    // Force name format
    uppercase(lump.name);

    // Stop if end of scope
    if (lump.name == Game::Utilities::str_to_key<uint64_t>("F_END"))
      return;

    // Load flat lump
    loadResourceFlat(file, lump);
  }

  // End of flat scope not reached
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  DOOM::Wad::loadResourceSprites(std::ifstream& file, int32_t const numlumps, int32_t const infotableofs, int32_t& iterator)
{
  // Process every lump in scope
  for (; iterator < numlumps; iterator++)
  {
    // Lump header data structure
    DOOM::Wad::Lump lump;

    // Read lump header data from file
    file.seekg(infotableofs + iterator * sizeof(DOOM::Wad::Lump), file.beg);
    Game::Utilities::read(file, &lump);

    // Force name format
    uppercase(lump.name);

    // Stop if end of scope
    if (lump.name == Game::Utilities::str_to_key<uint64_t>("S_END"))
      return;

    // Load flat lump
    loadResourceSprite(file, lump);
  }

  // End of sprite scope not reached
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  DOOM::Wad::loadResourcePatches(std::ifstream& file, int32_t const numlumps, int32_t const infotableofs, int32_t& iterator)
{
  // Process every lump in scope
  for (; iterator < numlumps; iterator++)
  {
    // Lump header data structure
    DOOM::Wad::Lump lump;

    // Read lump header data from file
    file.seekg(infotableofs + iterator * sizeof(DOOM::Wad::Lump), file.beg);
    Game::Utilities::read(file, &lump);

    // Force name format
    uppercase(lump.name);

    // Stop if end of scope
    if (lump.name == Game::Utilities::str_to_key<uint64_t>("P_END"))
      return;

    // Load patch lump
    loadResourcePatch(file, lump);
  }

  // End of patches scope not reached
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  DOOM::Wad::loadResourceTexture(std::ifstream& file, const DOOM::Wad::Lump& lump)
{
  int32_t numtexture;

  // Read the number of texture in lump from file
  file.seekg(lump.position, file.beg);
  Game::Utilities::read(file, &numtexture);

  // Read every texture in lump
  for (int32_t i = 0; i < numtexture; i++)
  {
    int32_t pointer;

    // Read texture pointer and move to position
    file.seekg(lump.position + sizeof(int32_t) + sizeof(int32_t) * i, file.beg);
    Game::Utilities::read(file, &pointer);
    file.seekg(lump.position + pointer, file.beg);

    uint64_t  name;

    // Read texture name
    Game::Utilities::read(file, &name);

    // Force name format
    uppercase(name);

    // Reset texture
    resources.textures.erase(name);

    int16_t numpatch;

    // Read texture data
    file.seekg(sizeof(int16_t) * 2, file.cur);
    Game::Utilities::read(file, &resources.textures[name].width);
    Game::Utilities::read(file, &resources.textures[name].height);
    file.seekg(sizeof(int16_t) * 2, file.cur);
    Game::Utilities::read(file, &numpatch);

    // Read texture's patches
    for (int16_t j = 0; j < numpatch; j++)
    {
      resources.textures[name].patches.push_back(DOOM::Wad::RawResources::Texture::Patch());
      Game::Utilities::read(file, &resources.textures[name].patches.back());
    }
  }
}

void  DOOM::Wad::loadResourceSprite(std::ifstream& file, const DOOM::Wad::Lump& lump)
{
  // Ignore special delimiters (TODO: dont ignore them ?)
  if (lump.name == Game::Utilities::str_to_key<uint64_t>("S_START") || lump.name == Game::Utilities::str_to_key<uint64_t>("S_END"))
    return;

  loadResourcePatch(file, lump, resources.sprites);
}

void  DOOM::Wad::loadResourcePatch(std::ifstream& file, const DOOM::Wad::Lump& lump)
{
  // Ignore special delimiters (TODO: dont ignore them ?)
  if (lump.name == Game::Utilities::str_to_key<uint64_t>("P_START") || lump.name == Game::Utilities::str_to_key<uint64_t>("P_END") ||
    lump.name == Game::Utilities::str_to_key<uint64_t>("P1_START") || lump.name == Game::Utilities::str_to_key<uint64_t>("P1_END") ||
    lump.name == Game::Utilities::str_to_key<uint64_t>("P2_START") || lump.name == Game::Utilities::str_to_key<uint64_t>("P2_END") ||
    lump.name == Game::Utilities::str_to_key<uint64_t>("P3_START") || lump.name == Game::Utilities::str_to_key<uint64_t>("P3_END"))
    return;

  loadResourcePatch(file, lump, resources.patches);
}

void  DOOM::Wad::loadResourceMenu(std::ifstream& file, const DOOM::Wad::Lump& lump)
{
  loadResourcePatch(file, lump, resources.menus);
}

void  DOOM::Wad::loadResourcePatch(std::ifstream& file, const DOOM::Wad::Lump& lump, std::unordered_map<uint64_t, DOOM::Wad::RawResources::Patch>& target)
{
  // Reset patch
  target.erase(lump.name);

  // Read sprite informations
  file.seekg(lump.position, file.beg);
  Game::Utilities::read(file, &target[lump.name].width);
  Game::Utilities::read(file, &target[lump.name].height);
  Game::Utilities::read(file, &target[lump.name].left);
  Game::Utilities::read(file, &target[lump.name].top);

  // Get every column of patch
  for (int16_t column = 0; column < target[lump.name].width; column++)
  {
    // Set position to pointer of the column
    file.seekg(lump.position + sizeof(int16_t) * 4 + sizeof(uint32_t) * column, file.beg);

    uint32_t  pointer;

    // Get column pointer from file and move to column position
    Game::Utilities::read(file, &pointer);
    file.seekg(lump.position + pointer, file.beg);

    // Push a new column
    target[lump.name].columns.push_back(DOOM::Wad::RawResources::Patch::Column());

    // Read each column spans
    while (true)
    {
      DOOM::Wad::RawResources::Patch::Column::Span  span;
      uint8_t					    offset;
      uint8_t					    size;

      // Read span offset
      Game::Utilities::read(file, &offset);

      // Stop reading span if reached end of span definition
      if (offset == 255)
	break;

      // Read size of the span
      Game::Utilities::read(file, &size);

      // Skip unused byte
      file.seekg(1, file.cur);

      // Set attribute in file structure
      span.offset = offset;

      // Allocate space for pixel span
      span.pixels.resize(size);

      // Read pixels value from file
      Game::Utilities::read(file, span.pixels.data(), span.pixels.size());

      // Skip unused byte
      file.seekg(1, file.cur);

      // Push new span in column definition
      target[lump.name].columns.back().spans.push_back(span);
    }
  }
}

void  DOOM::Wad::loadResourceFlat(std::ifstream& file, const DOOM::Wad::Lump& lump)
{
  // Ignore special delimiters (TODO: dont ignore them ?)
  if (lump.name == Game::Utilities::str_to_key<uint64_t>("F_START") || lump.name == Game::Utilities::str_to_key<uint64_t>("F_END") ||
    lump.name == Game::Utilities::str_to_key<uint64_t>("F1_START") || lump.name == Game::Utilities::str_to_key<uint64_t>("F1_END") ||
    lump.name == Game::Utilities::str_to_key<uint64_t>("F2_START") || lump.name == Game::Utilities::str_to_key<uint64_t>("F2_END") ||
    lump.name == Game::Utilities::str_to_key<uint64_t>("F3_START") || lump.name == Game::Utilities::str_to_key<uint64_t>("F3_END"))
    return;

  // Check for invalid lump size
  if (lump.size != sizeof(uint8_t) * 64 * 64)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Read flat data from file
  file.seekg(lump.position, file.beg);
  Game::Utilities::read(file, (uint8_t*)resources.flats[lump.name].texture, sizeof(uint8_t) * 64 * 64);
}

void  DOOM::Wad::loadResourceGenmidi(std::ifstream& file, const DOOM::Wad::Lump& lump)
{
  // Check for invalid lump size
  if (lump.size != sizeof(int8_t) * 8 + sizeof(DOOM::Wad::RawResources::Genmidi) * 175)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Clear container
  resources.genmidis.clear();

  // Set position to lump data
  file.seekg(lump.position, file.beg);

  int8_t  header[8 + 1] = { 0 };

  // Read the number of texture in lump from file
  Game::Utilities::read(file, (char*)header, 8);

  // Check for valid header
  if (std::string((char const*)header) != "#OPL_II#")
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  for (int i = 0; i < 175; i++)
  {
    DOOM::Wad::RawResources::Genmidi  record;

    // Read record data
    file.seekg(lump.position + sizeof(int8_t) * 8 + (sizeof(DOOM::Wad::RawResources::Genmidi) - sizeof(int8_t) * 32) * i, file.beg);
    Game::Utilities::read(file, &record.flag);
    Game::Utilities::read(file, &record.tuning);
    Game::Utilities::read(file, &record.note);
    Game::Utilities::read(file, &record.voice0);
    Game::Utilities::read(file, &record.voice1);
    
    // Read record name
    file.seekg(lump.position + sizeof(int8_t) * 8 + (sizeof(DOOM::Wad::RawResources::Genmidi) - sizeof(int8_t) * 32) * 175 + sizeof(int8_t) * 32 * i, file.beg);
    Game::Utilities::read(file, (char*)record.name, 32);

    // Push record to record vector
    resources.genmidis.push_back(record);
  }
}

void  DOOM::Wad::loadResourceMusic(std::ifstream& file, const DOOM::Wad::Lump& lump)
{
  // Check for invalid lump size
  if (lump.size < sizeof(int8_t) * 4 + sizeof(int16_t) * 7)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Set position to lump data
  file.seekg(lump.position, file.beg);

  // Skip unused fields
  file.seekg(sizeof(uint8_t) * 4, file.cur);

  uint16_t  datasize;
  uint16_t  offset;

  // Read header data from file
  Game::Utilities::read(file, &datasize);
  Game::Utilities::read(file, &offset);
  Game::Utilities::read(file, &resources.musics[lump.name].primary);
  Game::Utilities::read(file, &resources.musics[lump.name].secondary);
  Game::Utilities::read(file, &resources.musics[lump.name].instrument);

  // Skip unused fields
  file.seekg(sizeof(int16_t), file.cur);

  // Read header data from file
  Game::Utilities::read(file, &resources.musics[lump.name].patch);

  // Set position to music data
  file.seekg(lump.position + offset, file.beg);

  // Allocate audio data buffer
  resources.musics[lump.name].data.resize(datasize);

  // Read audio data from lump
  Game::Utilities::read(file, resources.musics[lump.name].data.data(), resources.musics[lump.name].data.size());
}

void  DOOM::Wad::loadResourceSound(std::ifstream& file, const DOOM::Wad::Lump& lump)
{
  // Check for invalid lump size
  if (lump.size < sizeof(int16_t) * 4)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Set position to lump data
  file.seekg(lump.position, file.beg);

  // Skip unused fields
  file.seekg(sizeof(int16_t), file.cur);

  // Read sound header
  Game::Utilities::read(file, &resources.sounds[lump.name].rate);
  Game::Utilities::read(file, &resources.sounds[lump.name].samples);

  // Skip unused field
  file.seekg(sizeof(int16_t), file.cur);

  // Allocate sound buffer
  resources.sounds[lump.name].buffer.resize(resources.sounds[lump.name].samples);

  // Read sound data to buffer
  Game::Utilities::read(file, resources.sounds[lump.name].buffer.data(), resources.sounds[lump.name].buffer.size());
}

void  DOOM::Wad::loadResourceDemox(std::ifstream& file, const DOOM::Wad::Lump& lump)
{
  // Check for invalid lump size
  if (lump.size < sizeof(int8_t) * 7)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Set position to lump data
  file.seekg(lump.position, file.beg);

  int8_t  byte;

  // Read lump header first byte
  Game::Utilities::read(file, &byte);

  // Load 7 bytes header
  if (byte >= 0 && byte <= 4)
  {
    // Reallocate demos vector
    if (resources.demos.size() < (lump.name << 32 & 0xFF) - '0' + 1)
      resources.demos.resize((lump.name << 32 & 0xFF) - '0' + 1);

    // Read header data
    resources.demos[(lump.name << 32 & 0xFF) - '0'].skill = byte;
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].episode);
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].mission);
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].player1);
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].player2);
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].player3);
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].player4);

    // Complete data structure
    resources.demos[(lump.name << 32 & 0xFF) - '0'].mode = DOOM::Wad::RawResources::Demo::Mode::Single;
    resources.demos[(lump.name << 32 & 0xFF) - '0'].respawn = 0;
    resources.demos[(lump.name << 32 & 0xFF) - '0'].fast = 0;
    resources.demos[(lump.name << 32 & 0xFF) - '0'].nomonster = 0;
    resources.demos[(lump.name << 32 & 0xFF) - '0'].viewpoint = DOOM::Wad::RawResources::Demo::ViewPoint::Player1;
  }

  // Load 13 bytes header
  else if (byte >= 104 && byte <= 106)
  {
    // Reallocate demos vector
    if (resources.demos.size() < (lump.name << 32 & 0xFF) - '0' + 1)
      resources.demos.resize((lump.name << 32 & 0xFF) - '0' + 1);

    // Read header data
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].skill);
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].episode);
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].mission);
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].mode);
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].respawn);
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].fast);
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].nomonster);
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].viewpoint);
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].player1);
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].player2);
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].player3);
    Game::Utilities::read(file, &resources.demos[(lump.name << 32 & 0xFF) - '0'].player4);
  }

  // Unsupported version
  else
    return;

  // Erase existing demo records
  resources.demos[(lump.name << 32 & 0xFF) - '0'].records.clear();

  while (true)
  {
    // Read first record byte
    Game::Utilities::read(file, &byte);

    // Quit if quit byte encountered
    if (byte == 0x80)
      return;

    // Move read cursor backward
    file.seekg(-(int)sizeof(int8_t), file.cur);

    DOOM::Wad::RawResources::Demo::Record record;

    // Read whole record
    Game::Utilities::read(file, &record);

    // Push record to player move vector
    resources.demos[(lump.name << 32 & 0xFF) - '0'].records.push_back(record);
  }
}

void  DOOM::Wad::loadResourcePlaypal(std::ifstream& file, const DOOM::Wad::Lump& lump)
{
  loadLump<DOOM::Wad::RawResources::Palette>(file, lump, resources.palettes);
}

void  DOOM::Wad::loadResourceColormap(std::ifstream& file, const DOOM::Wad::Lump& lump)
{
  loadLump<DOOM::Wad::RawResources::Colormap>(file, lump, resources.colormaps);
}

void  DOOM::Wad::loadResourcePnames(std::ifstream& file, const DOOM::Wad::Lump& lump)
{
  // Check for invalid lump size
  if ((lump.size - 4) % sizeof(uint64_t) != 0)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Reset data container
  resources.pnames.clear();

  int32_t number = 0;

  // Load number of names in lump
  file.seekg(lump.position, file.beg);
  Game::Utilities::read(file, &number);

  // Check for errors
  if (number < 0)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Read names from file
  for (int i = 0; i < number; i++)
  {
    resources.pnames.push_back(0);
    Game::Utilities::read(file, &resources.pnames.back());

    // Force name format
    uppercase(resources.pnames.back());
  }
}

void  DOOM::Wad::loadResourceEndoom(std::ifstream& file, const DOOM::Wad::Lump& lump)
{
  // Check for invalid lump size
  if (lump.size != sizeof(DOOM::Wad::RawResources::Endoom))
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Read header from file
  file.seekg(lump.position, file.beg);
  Game::Utilities::read(file, &resources.endoom);
}

void  DOOM::Wad::loadLevelExmy(const DOOM::Wad::Lump& lump, std::pair<uint8_t, uint8_t>& level)
{
  // Get episode and mission number from name
  level = { (uint8_t)((lump.name >> 8) & 0xFF) - '0', (uint8_t)((lump.name >> 24) & 0xFF) - '0' };

  // Remove previously loaded level
  levels.erase(level);
}

void  DOOM::Wad::loadLevelMapxy(const DOOM::Wad::Lump& lump, std::pair<uint8_t, uint8_t>& level)
{
  // Get episode and mission number from name
  level = { 1,
    ((uint8_t)((lump.name >> 24) & 0xFF) - '0') * 10
    + ((uint8_t)((lump.name >> 32) & 0xFF) - '0') * 1
  };

  // Remove previously loaded level
  levels.erase(level);
}

void  DOOM::Wad::loadLevelThings(std::ifstream& file, const DOOM::Wad::Lump& lump, std::pair<uint8_t, uint8_t> level)
{
  loadLump<DOOM::Wad::RawLevel::Thing>(file, lump, levels[level].things);
}

void  DOOM::Wad::loadLevelLinedefs(std::ifstream& file, const DOOM::Wad::Lump& lump, std::pair<uint8_t, uint8_t> level)
{
  loadLump<DOOM::Wad::RawLevel::Linedef>(file, lump, levels[level].linedefs);
}

void  DOOM::Wad::loadLevelSidedefs(std::ifstream& file, const DOOM::Wad::Lump& lump, std::pair<uint8_t, uint8_t> level)
{
  // Load sidedefs from file
  loadLump<DOOM::Wad::RawLevel::Sidedef>(file, lump, levels[level].sidedefs);

  // Convert name in sidedefs to uppercase
  for (DOOM::Wad::RawLevel::Sidedef& sidedef : levels[level].sidedefs)
  {
    uppercase(sidedef.upper);
    uppercase(sidedef.lower);
    uppercase(sidedef.middle);
  }
}

void  DOOM::Wad::loadLevelVertexes(std::ifstream& file, const DOOM::Wad::Lump& lump, std::pair<uint8_t, uint8_t> level)
{
  loadLump<DOOM::Wad::RawLevel::Vertex>(file, lump, levels[level].vertexes);
}

void  DOOM::Wad::loadLevelSegs(std::ifstream& file, const DOOM::Wad::Lump& lump, std::pair<uint8_t, uint8_t> level)
{
  loadLump<DOOM::Wad::RawLevel::Segment>(file, lump, levels[level].segments);
}

void  DOOM::Wad::loadLevelSsectors(std::ifstream& file, const DOOM::Wad::Lump& lump, std::pair<uint8_t, uint8_t> level)
{
  loadLump<DOOM::Wad::RawLevel::Subsector>(file, lump, levels[level].subsectors);
}

void  DOOM::Wad::loadLevelNodes(std::ifstream& file, const DOOM::Wad::Lump& lump, std::pair<uint8_t, uint8_t> level)
{
  loadLump<DOOM::Wad::RawLevel::Node>(file, lump, levels[level].nodes);
}

void  DOOM::Wad::loadLevelSectors(std::ifstream& file, const DOOM::Wad::Lump& lump, std::pair<uint8_t, uint8_t> level)
{
  loadLump<DOOM::Wad::RawLevel::Sector>(file, lump, levels[level].sectors);

  // Convert name in sectors to uppercase
  for (DOOM::Wad::RawLevel::Sector& sector : levels[level].sectors)
  {
    uppercase(sector.floor_texture);
    uppercase(sector.ceiling_texture);
  }
}

void  DOOM::Wad::loadLevelReject(std::ifstream& file, const DOOM::Wad::Lump& lump, std::pair<uint8_t, uint8_t> level)
{
  loadLump<uint8_t>(file, lump, levels[level].reject.rejects);
}

void  DOOM::Wad::loadLevelBlockmap(std::ifstream& file, const DOOM::Wad::Lump& lump, std::pair<uint8_t, uint8_t> level)
{
  // Check for invalid lump size
  if (lump.size < sizeof(int16_t) * 4)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Read header from file
  file.seekg(lump.position, file.beg);
  Game::Utilities::read(file, &levels[level].blockmap.x);
  Game::Utilities::read(file, &levels[level].blockmap.y);
  Game::Utilities::read(file, &levels[level].blockmap.column);
  Game::Utilities::read(file, &levels[level].blockmap.row);

  // Read blockmap offsets
  levels[level].blockmap.offset.resize(levels[level].blockmap.column * levels[level].blockmap.row);
  Game::Utilities::read(file, levels[level].blockmap.offset.data(), levels[level].blockmap.offset.size());

  // Read blocklists
  levels[level].blockmap.blocklist.resize((lump.size - (sizeof(int16_t) * 4 + sizeof(int16_t) * (levels[level].blockmap.column * levels[level].blockmap.row))) / sizeof(int16_t));
  Game::Utilities::read(file, levels[level].blockmap.blocklist.data(), levels[level].blockmap.blocklist.size());
}

void  DOOM::Wad::loadIgnore()
{}