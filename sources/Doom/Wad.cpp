#include <cmath>
#include <functional>
#include <iostream>
#include <list>
#include <regex>

#include "Doom/Wad.hpp"
#include "System/Config.hpp"

Game::Wad::Wad() :
  levels(),
  resources()
{}

Game::Wad::~Wad()
{}

bool	Game::Wad::load(std::string const & path)
{
  std::ifstream	file(path, std::ifstream::binary);

  // Check if failed to open file
  if (file.bad() == true)
    return false;

  // WAD file header
  int8_t	identification[4 + 1] = { 0 };
  int32_t	numlumps = 0;
  int32_t	infotableofs = 0;

  // Get header data from file
  file.read((char *)identification, sizeof(int8_t) * 4);
  file.read((char *)&numlumps, sizeof(int32_t));
  file.read((char *)&infotableofs, sizeof(int32_t));

  // Check for valid file data
  if ((std::string((const char *)identification) != "IWAD" && std::string((const char *)identification) != "PWAD") || numlumps < 0 || infotableofs < 0)
    return false;

  // Load base level if file is an extension
  if (std::string((const char *)identification) != "IWAD")
    if (load(Game::Config::ExecutablePath + "/assets/levels/doom.wad") == false)
      return false;

  // Load lumps
  return loadLumps(file, numlumps, infotableofs);
}

bool	Game::Wad::loadLumps(std::ifstream & file, int32_t const numlumps, int32_t const infotableofs)
{
  std::pair<uint8_t, uint8_t>	level = { 0, 0 };

  // Process every lump of file
  for (int32_t iterator = 0; iterator < numlumps; iterator++)
  {
    // Lump header data structure
    Game::Wad::Lump	lump;

    // Read lump header data from file
    file.seekg(infotableofs + iterator * sizeof(Game::Wad::Lump), file.beg);
    file.read((char *)&lump, sizeof(Game::Wad::Lump));

    // Force name format
    uppercase(lump.name);

    // Initialize regex command map
    std::list<std::pair<std::regex, std::function<bool()>>>	commands_regex =
    { 
      // Scoped lumps
      { std::regex("F_START"), std::bind(&Game::Wad::loadResourceFlats, this, std::ref(file), numlumps, infotableofs, std::ref(iterator)) },
      { std::regex("S_START"), std::bind(&Game::Wad::loadResourceSprites, this, std::ref(file), numlumps, infotableofs, std::ref(iterator)) },
      { std::regex("P_START"), std::bind(&Game::Wad::loadResourcePatches, this, std::ref(file), numlumps, infotableofs, std::ref(iterator)) },

      // Resources lumps
      { std::regex("PLAYPAL"), std::bind(&Game::Wad::loadResourcePlaypal, this, std::ref(file), std::ref(lump)) },
      { std::regex("COLORMAP"), std::bind(&Game::Wad::loadResourceColormap, this, std::ref(file), std::ref(lump)) },
      { std::regex("TEXTURE1"), std::bind(&Game::Wad::loadResourceTexture, this, std::ref(file), std::ref(lump)) },
      { std::regex("TEXTURE2"), std::bind(&Game::Wad::loadResourceTexture, this, std::ref(file), std::ref(lump)) },
      { std::regex("PNAMES"), std::bind(&Game::Wad::loadResourcePnames, this, std::ref(file), std::ref(lump)) },
      { std::regex("ENDOOM"), std::bind(&Game::Wad::loadResourceEndoom, this, std::ref(file), std::ref(lump)) },
      { std::regex("GENMIDI"), std::bind(&Game::Wad::loadResourceGenmidi, this, std::ref(file), std::ref(lump)) },
      { std::regex("DMXGUS"), std::bind(&Game::Wad::loadIgnore, this) },
      { std::regex("D_[[:alnum:]]+"), std::bind(&Game::Wad::loadResourceMusic, this, std::ref(file), std::ref(lump)) },
      { std::regex("DS[[:alnum:]]+"), std::bind(&Game::Wad::loadResourceSound, this, std::ref(file), std::ref(lump)) },
      { std::regex("DP[[:alnum:]]+"), std::bind(&Game::Wad::loadIgnore, this) },
      { std::regex("DEMO[[:digit:]]"), std::bind(&Game::Wad::loadResourceDemox, this, std::ref(file), std::ref(lump)) },
      
      // Menu resources lumps
      { std::regex("HELP1"), std::bind(&Game::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
      { std::regex("HELP2"), std::bind(&Game::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
      { std::regex("TITLEPIC"), std::bind(&Game::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
      { std::regex("CREDIT"), std::bind(&Game::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
      { std::regex("VICTORY2"), std::bind(&Game::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
      { std::regex("PFUB1"), std::bind(&Game::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
      { std::regex("PFUB2"), std::bind(&Game::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
      { std::regex("INTERPIC"), std::bind(&Game::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
      { std::regex("ENDPIC"), std::bind(&Game::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
      { std::regex("END[[:digit:]]"), std::bind(&Game::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
      { std::regex("AMMNUM[[:digit:]]"), std::bind(&Game::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
      { std::regex("ST[[:alnum:]]+"), std::bind(&Game::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
      { std::regex("WI[[:alnum:]]+"), std::bind(&Game::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
      { std::regex("M_[[:alnum:]]+"), std::bind(&Game::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
      { std::regex("BRDR_[[:alnum:]]+"), std::bind(&Game::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },

      // Level lumps
      { std::regex("VERTEXES"), std::bind(&Game::Wad::loadLevelVertexes, this, std::ref(file), std::ref(lump), level) },
      { std::regex("SECTORS"), std::bind(&Game::Wad::loadLevelSectors, this, std::ref(file), std::ref(lump), level) },
      { std::regex("SIDEDEFS"), std::bind(&Game::Wad::loadLevelSidedefs, this, std::ref(file), std::ref(lump), level) },
      { std::regex("LINEDEFS"), std::bind(&Game::Wad::loadLevelLinedefs, this, std::ref(file), std::ref(lump), level) },
      { std::regex("SEGS"), std::bind(&Game::Wad::loadLevelSegs, this, std::ref(file), std::ref(lump), level) },
      { std::regex("SSECTORS"), std::bind(&Game::Wad::loadLevelSsectors, this, std::ref(file), std::ref(lump), level) },
      { std::regex("NODES"), std::bind(&Game::Wad::loadLevelNodes, this, std::ref(file), std::ref(lump), level) },
      { std::regex("REJECT"), std::bind(&Game::Wad::loadLevelReject, this, std::ref(file), std::ref(lump), level) },
      { std::regex("BLOCKMAP"), std::bind(&Game::Wad::loadLevelBlockmap, this, std::ref(file), std::ref(lump), level) },
      { std::regex("THINGS"), std::bind(&Game::Wad::loadLevelThings, this, std::ref(file), std::ref(lump), level) },
      { std::regex("E[[:digit:]]M[[:digit:]]"), std::bind(&Game::Wad::loadLevelExmy, this, std::ref(lump), std::ref(level)) },
      { std::regex("MAP[[:digit:]][[:digit:]]"), std::bind(&Game::Wad::loadLevelMapxy, this, std::ref(lump), std::ref(level)) }
    };

    // Remove each non-matching regex
    commands_regex.remove_if([lump](std::pair<std::regex, std::function<bool()>> const & regex) { uint64_t str[] = { lump.name, 0 }; return std::regex_match((char const *)str, regex.first) == false; });

    // Execute matching regex if only one found
    if (commands_regex.size() == 1)
    {
      if (commands_regex.front().second() == false)
	return false;
    }
    else
    {
      uint64_t str[2] = { lump.name, 0 };

      std::cerr << "[Wad::load]: Warning, unknown lump name '" << std::string((char const *)str) << "'." << std::endl;
    }
  }

  return true;
}

bool	Game::Wad::loadResourceFlats(std::ifstream & file, int32_t const numlumps, int32_t const infotableofs, int32_t & iterator)
{
  // Process every lump in scope
  for (; iterator < numlumps; iterator++)
  {
    // Lump header data structure
    Game::Wad::Lump	lump;

    // Read lump header data from file
    file.seekg(infotableofs + iterator * sizeof(Game::Wad::Lump), file.beg);
    file.read((char *)&lump, sizeof(Game::Wad::Lump));

    // Force name format
    uppercase(lump.name);
    
    // Stop if end of scope
    if (lump.name == Game::str_to_key("F_END"))
      return true;

    // Load flat lump
    if (loadResourceFlat(file, lump) == false)
      return false;
  }

  return false;
}

bool	Game::Wad::loadResourceSprites(std::ifstream & file, int32_t const numlumps, int32_t const infotableofs, int32_t & iterator)
{
  // Process every lump in scope
  for (; iterator < numlumps; iterator++)
  {
    // Lump header data structure
    Game::Wad::Lump	lump;

    // Read lump header data from file
    file.seekg(infotableofs + iterator * sizeof(Game::Wad::Lump), file.beg);
    file.read((char *)&lump, sizeof(Game::Wad::Lump));

    // Force name format
    uppercase(lump.name);

    // Stop if end of scope
    if (lump.name == Game::str_to_key("S_END"))
      return true;

    // Load flat lump
    if (loadResourceSprite(file, lump) == false)
      return false;
  }

  return false;
}

bool	Game::Wad::loadResourcePatches(std::ifstream & file, int32_t const numlumps, int32_t const infotableofs, int32_t & iterator)
{
  // Process every lump in scope
  for (; iterator < numlumps; iterator++)
  {
    // Lump header data structure
    Game::Wad::Lump	lump;

    // Read lump header data from file
    file.seekg(infotableofs + iterator * sizeof(Game::Wad::Lump), file.beg);
    file.read((char *)&lump, sizeof(Game::Wad::Lump));

    // Force name format
    uppercase(lump.name);

    // Stop if end of scope
    if (lump.name == Game::str_to_key("P_END"))
      return true;

    // Load patch lump
    if (loadResourcePatch(file, lump) == false)
      return false;
  }

  return false;
}

bool	Game::Wad::loadResourceTexture(std::ifstream & file, Game::Wad::Lump const & lump)
{
  int32_t	numtexture;

  // Read the number of texture in lump from file
  file.seekg(lump.position, file.beg);
  file.read((char *)&numtexture, sizeof(int32_t));

  // Read every texture in lump
  for (int32_t i = 0; i < numtexture; i++)
  {
    int32_t	pointer;

    // Read texture pointer and move to position
    file.seekg(lump.position + sizeof(int32_t) + sizeof(int32_t) * i, file.beg);
    file.read((char *)&pointer, sizeof(int32_t));
    file.seekg(lump.position + pointer, file.beg);

    uint64_t	name;
    
    // Read texture name
    file.read((char *)&name, sizeof(uint64_t));

    // Force name format
    uppercase(name);

    // Skip texture if already defined
    if (resources.textures.find(name) != resources.textures.end())
      continue;

    int16_t	numpatch;

    // Read texture data
    file.seekg(sizeof(int16_t) * 2, file.cur);
    file.read((char *)&resources.textures[name].width, sizeof(int16_t));
    file.read((char *)&resources.textures[name].height, sizeof(int16_t));
    file.seekg(sizeof(int16_t) * 2, file.cur);
    file.read((char *)&numpatch, sizeof(int16_t));

    // Read texture's patches
    for (int16_t j = 0; j < numpatch; j++)
    {
      resources.textures[name].patches.push_back(Game::Wad::RawResources::Texture::Patch());
      file.read((char *)&resources.textures[name].patches.back(), sizeof(Game::Wad::RawResources::Texture::Patch));
    }
  }

  return true;
}

bool	Game::Wad::loadResourceSprite(std::ifstream & file, Game::Wad::Lump const & lump)
{
  // Ignore special delimiters (TODO: dont ignore them ?)
  if (lump.name == Game::str_to_key("S_START") || lump.name == Game::str_to_key("S_END"))
    return true;

  return loadResourcePatch(file, lump, resources.sprites);
}

bool	Game::Wad::loadResourcePatch(std::ifstream & file, Game::Wad::Lump const & lump)
{
  // Ignore special delimiters (TODO: dont ignore them ?)
  if (lump.name == Game::str_to_key("P_START") || lump.name == Game::str_to_key("P_END") ||
    lump.name == Game::str_to_key("P1_START") || lump.name == Game::str_to_key("P1_END") ||
    lump.name == Game::str_to_key("P2_START") || lump.name == Game::str_to_key("P2_END") ||
    lump.name == Game::str_to_key("P3_START") || lump.name == Game::str_to_key("P3_END"))
    return true;

  return loadResourcePatch(file, lump, resources.patches);
}

bool	Game::Wad::loadResourceMenu(std::ifstream & file, Game::Wad::Lump const & lump)
{
  return loadResourcePatch(file, lump, resources.menus);
}

bool	Game::Wad::loadResourcePatch(std::ifstream & file, Game::Wad::Lump const & lump, std::unordered_map<uint64_t, Game::Wad::RawResources::Patch> & target)
{
  // Cancel if patch name already registered
  if (target.find(lump.name) != target.end())
    return true;

  // Read sprite informations
  file.seekg(lump.position, file.beg);
  file.read((char *)&target[lump.name].width, sizeof(int16_t));
  file.read((char *)&target[lump.name].height, sizeof(int16_t));
  file.read((char *)&target[lump.name].left, sizeof(int16_t));
  file.read((char *)&target[lump.name].top, sizeof(int16_t));

  // Get every column of patch
  for (int16_t column = 0; column < target[lump.name].width; column++)
  {
    // Set position to pointer of the column
    file.seekg(lump.position + sizeof(int16_t) * 4 + sizeof(uint32_t) * column, file.beg);

    uint32_t	pointer;

    // Get column pointer from file and move to column position
    file.read((char *)&pointer, sizeof(uint32_t));
    file.seekg(lump.position + pointer, file.beg);

    // Push a new column
    target[lump.name].columns.push_back(Game::Wad::RawResources::Patch::Column());

    // Read each column spans
    while (true)
    {
      Game::Wad::RawResources::Patch::Column::Span	span;
      uint8_t						offset;
      uint8_t						size;

      // Read span offset
      file.read((char *)&offset, sizeof(uint8_t));

      // Stop reading span if reached end of span definition
      if (offset == 255)
	break;

      // Read size of the span
      file.read((char *)&size, sizeof(uint8_t));

      // Skip unused byte
      file.seekg(1, file.cur);

      // Set attribute in file structure
      span.offset = offset;

      // Allocate space for pixel span
      span.pixels.resize(size);

      // Read pixels value from file
      file.read((char *)span.pixels.data(), sizeof(uint8_t) * size);

      // Skip unused byte
      file.seekg(1, file.cur);

      // Push new span in column definition
      target[lump.name].columns.back().spans.push_back(span);
    }
  }

  return true;
}

bool	Game::Wad::loadResourceFlat(std::ifstream & file, Game::Wad::Lump const & lump)
{
  // Ignore special delimiters (TODO: dont ignore them ?)
  if (lump.name == Game::str_to_key("F_START") || lump.name == Game::str_to_key("F_END") ||
    lump.name == Game::str_to_key("F1_START") || lump.name == Game::str_to_key("F1_END") ||
    lump.name == Game::str_to_key("F2_START") || lump.name == Game::str_to_key("F2_END"))
    return true;

  // Check for invalid lump size
  if (lump.size != sizeof(uint8_t) * 64 * 64)
    return false;

  // Read flat data from file
  file.seekg(lump.position, file.beg);
  file.read((char *)resources.flats[lump.name].texture, sizeof(uint8_t) * 64 * 64);

  return true;
}

bool	Game::Wad::loadResourceGenmidi(std::ifstream & file, Game::Wad::Lump const & lump)
{
  // Check for invalid lump size
  if (lump.size != sizeof(int8_t) * 8 + sizeof(Game::Wad::RawResources::Genmidi) * 175)
    return false;

  // Set position to lump data
  file.seekg(lump.position, file.beg);

  int8_t	header[8 + 1] = { 0 };

  // Read the number of texture in lump from file
  file.read((char *)&header, sizeof(int8_t) * 8);

  // Check for valid header
  if (std::string((char const *)header) != "#OPL_II#")
    return false;

  for (int i = 0; i < 175; i++)
  {
    Game::Wad::RawResources::Genmidi	record;

    // Read record data
    file.seekg(lump.position + sizeof(int8_t) * 8 + (sizeof(Game::Wad::RawResources::Genmidi) - sizeof(int8_t) * 32) * i, file.beg);
    file.read((char *)&record, sizeof(Game::Wad::RawResources::Genmidi) - sizeof(int8_t) * 32);

    // Read record name
    file.seekg(lump.position + sizeof(int8_t) * 8 + (sizeof(Game::Wad::RawResources::Genmidi) - sizeof(int8_t) * 32) * 175 + sizeof(int8_t) * 32 * i, file.beg);
    file.read((char *)record.name, sizeof(int8_t) * 32);

    // Push record to record vector
    resources.genmidis.push_back(record);
  }

  return true;
}

bool	Game::Wad::loadResourceMusic(std::ifstream & file, Game::Wad::Lump const & lump)
{
  // Check for invalid lump size
  if (lump.size < sizeof(int8_t) * 4 + sizeof(int16_t) * 7)
    return false;

  // Set position to lump data
  file.seekg(lump.position, file.beg);

  // Skip unused fields
  file.seekg(sizeof(uint8_t) * 4, file.cur);

  uint16_t	datasize;
  uint16_t	offset;

  // Read header data from file
  file.read((char *)&datasize, sizeof(uint16_t));
  file.read((char *)&offset, sizeof(uint16_t));
  file.read((char *)&resources.musics[lump.name].primary, sizeof(int16_t));
  file.read((char *)&resources.musics[lump.name].secondary, sizeof(int16_t));
  file.read((char *)&resources.musics[lump.name].instrument, sizeof(int16_t));

  // Skip unused fields
  file.seekg(sizeof(int16_t), file.cur);

  // Read header data from file
  file.read((char *)&resources.musics[lump.name].patch, sizeof(int16_t));

  // Set position to music data
  file.seekg(lump.position + offset, file.beg);

  // Allocate audio data buffer
  resources.musics[lump.name].data.resize(datasize);

  // Read audio data from lump
  file.read((char *)resources.musics[lump.name].data.data(), datasize);

  return true;
}

bool	Game::Wad::loadResourceSound(std::ifstream & file, Game::Wad::Lump const & lump)
{
  // Check for invalid lump size
  if (lump.size < sizeof(int16_t) * 4)
    return false;

  // Set position to lump data
  file.seekg(lump.position, file.beg);

  // Skip unused fields
  file.seekg(sizeof(int16_t), file.cur);

  // Read sound header
  file.read((char *)&resources.sounds[lump.name].rate, sizeof(int16_t));
  file.read((char *)&resources.sounds[lump.name].samples, sizeof(int16_t));

  // Skip unused field
  file.seekg(sizeof(int16_t), file.cur);

  // Allocate sound buffer
  resources.sounds[lump.name].buffer.resize(resources.sounds[lump.name].samples);

  // Read sound data to buffer
  file.read((char *)resources.sounds[lump.name].buffer.data(), sizeof(uint8_t) * resources.sounds[lump.name].samples);

  return true;
}

bool	Game::Wad::loadResourceDemox(std::ifstream & file, Game::Wad::Lump const & lump)
{
  // Check for invalid lump size
  if (lump.size < sizeof(int8_t) * 7)
    return false;

  // Set position to lump data
  file.seekg(lump.position, file.beg);

  int8_t	byte;

  // Read lump header first byte
  file.read((char *)&byte, sizeof(int8_t));

  // Load 7 bytes header
  if (byte >= 0 && byte <= 4)
  {
    // Reallocate demos vector
    if (resources.demos.size() <  (lump.name << 32 & 0xFF) - '0' + 1)
      resources.demos.resize((lump.name << 32 & 0xFF) - '0' + 1);

    // Read header data
    resources.demos[(lump.name << 32 & 0xFF) - '0'].skill = byte;
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].episode, sizeof(int8_t));
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].mission, sizeof(int8_t));
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].player1, sizeof(int8_t));
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].player2, sizeof(int8_t));
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].player3, sizeof(int8_t));
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].player4, sizeof(int8_t));

    // Complete data structure
    resources.demos[(lump.name << 32 & 0xFF) - '0'].mode = Game::Wad::RawResources::Demo::Mode::Single;
    resources.demos[(lump.name << 32 & 0xFF) - '0'].respawn = 0;
    resources.demos[(lump.name << 32 & 0xFF) - '0'].fast = 0;
    resources.demos[(lump.name << 32 & 0xFF) - '0'].nomonster = 0;
    resources.demos[(lump.name << 32 & 0xFF) - '0'].viewpoint = Game::Wad::RawResources::Demo::ViewPoint::Player1;
  }

  // Load 13 bytes header
  else if (byte >= 104 && byte <= 106)
  {
    // Reallocate demos vector
    if (resources.demos.size() < (lump.name << 32 & 0xFF) - '0' + 1)
      resources.demos.resize((lump.name << 32 & 0xFF) - '0' + 1);

    // Read header data
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].skill, sizeof(int8_t));
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].episode, sizeof(int8_t));
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].mission, sizeof(int8_t));
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].mode, sizeof(int8_t));
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].respawn, sizeof(int8_t));
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].fast, sizeof(int8_t));
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].nomonster, sizeof(int8_t));
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].viewpoint, sizeof(int8_t));
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].player1, sizeof(int8_t));
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].player2, sizeof(int8_t));
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].player3, sizeof(int8_t));
    file.read((char *)&resources.demos[(lump.name << 32 & 0xFF) - '0'].player4, sizeof(int8_t));
  }

  // Unsupported version
  else
    return true;

  while (true)
  {
    // Read first record byte
    file.read((char *)&byte, sizeof(int8_t));

    // Quit if quit byte encountered
    if (byte == 0x80)
      return true;

    // Move read cursor backward
    file.seekg(-(int)sizeof(int8_t), file.cur);

    Game::Wad::RawResources::Demo::Record	record;

    // Read whole record
    file.read((char *)&record, sizeof(Game::Wad::RawResources::Demo::Record));

    // Push record to player move vector
    resources.demos[(lump.name << 32 & 0xFF) - '0'].records.push_back(record);
  }
}

bool	Game::Wad::loadResourcePlaypal(std::ifstream & file, Game::Wad::Lump const & lump)
{
  return loadLump<Game::Wad::RawResources::Palette>(file, lump, resources.palettes);
}

bool	Game::Wad::loadResourceColormap(std::ifstream & file, Game::Wad::Lump const & lump)
{
  return loadLump<Game::Wad::RawResources::Colormap>(file, lump, resources.colormaps);
}

bool	Game::Wad::loadResourcePnames(std::ifstream & file, Game::Wad::Lump const & lump)
{
  // Check for invalid lump size
  if ((lump.size - 4) % sizeof(uint64_t) != 0)
    return false;

  // Reset data container
  resources.pnames.clear();

  int32_t	number = 0;

  // Load number of names in lump
  file.seekg(lump.position, file.beg);
  file.read((char *)&number, sizeof(int32_t));

  // Check for errors
  if (number < 0)
    return false;

  // Read names from file
  for (int i = 0; i < number; i++)
  {
    resources.pnames.push_back(0);
    file.read((char *)&resources.pnames.back(), sizeof(uint64_t));

    // Force name format
    uppercase(resources.pnames.back());
  }

  return true;
}

bool	Game::Wad::loadResourceEndoom(std::ifstream & file, Game::Wad::Lump const & lump)
{
  // Check for invalid lump size
  if (lump.size != sizeof(Game::Wad::RawResources::Endoom))
    return false;

  // Read header from file
  file.seekg(lump.position, file.beg);
  file.read((char *)&resources.endoom, sizeof(Game::Wad::RawResources::Endoom));

  return true;
}

bool	Game::Wad::loadLevelExmy(Game::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> & level)
{
  // Get episode and mission number from name
  level = { (uint8_t)((lump.name >> 8) & 0xFF) - '0', (uint8_t)((lump.name >> 24) & 0xFF) - '0' };

  return true;
}

bool	Game::Wad::loadLevelMapxy(Game::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> & level)
{
  // Get episode and mission number from name
  level = { (uint8_t)((lump.name >> 24) & 0xFF) - '0', (uint8_t)((lump.name >> 32) & 0xFF) - '0' };

  return true;
}

bool	Game::Wad::loadLevelThings(std::ifstream & file, Game::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  return loadLump<Game::Wad::RawLevel::Thing>(file, lump, levels[level].things);
}

bool	Game::Wad::loadLevelLinedefs(std::ifstream & file, Game::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  return loadLump<Game::Wad::RawLevel::Linedef>(file, lump, levels[level].linedefs);
}

bool	Game::Wad::loadLevelSidedefs(std::ifstream & file, Game::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  // Load sidedefs from file
  if (loadLump<Game::Wad::RawLevel::Sidedef>(file, lump, levels[level].sidedefs) == false)
    return false;

  // Convert name in sidedefs to uppercase
  for (Game::Wad::RawLevel::Sidedef & sidedef : levels[level].sidedefs)
  {
    uppercase(sidedef.upper);
    uppercase(sidedef.lower);
    uppercase(sidedef.middle);
  }

  return true;
}

bool	Game::Wad::loadLevelVertexes(std::ifstream & file, Game::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  return loadLump<Game::Wad::RawLevel::Vertex>(file, lump, levels[level].vertexes);
}

bool	Game::Wad::loadLevelSegs(std::ifstream & file, Game::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  return loadLump<Game::Wad::RawLevel::Segment>(file, lump, levels[level].segments);
}

bool	Game::Wad::loadLevelSsectors(std::ifstream & file, Game::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  return loadLump<Game::Wad::RawLevel::Subsector>(file, lump, levels[level].subsectors);
}

bool	Game::Wad::loadLevelNodes(std::ifstream & file, Game::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  return loadLump<Game::Wad::RawLevel::Node>(file, lump, levels[level].nodes);
}

bool	Game::Wad::loadLevelSectors(std::ifstream & file, Game::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  if (loadLump<Game::Wad::RawLevel::Sector>(file, lump, levels[level].sectors) == false)
    return false;

  // Convert name in sectors to uppercase
  for (Game::Wad::RawLevel::Sector & sector : levels[level].sectors)
  {
    uppercase(sector.floor_texture);
    uppercase(sector.ceiling_texture);
  }

  return true;
}

bool	Game::Wad::loadLevelReject(std::ifstream & file, Game::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  return loadLump<uint8_t>(file, lump, levels[level].reject.rejects);
}

bool	Game::Wad::loadLevelBlockmap(std::ifstream & file, Game::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  // Check for invalid lump size
  if (lump.size < sizeof(int16_t) * 4)
    return false;

  // Read header from file
  file.seekg(lump.position, file.beg);
  file.read((char *)&levels[level].blockmap, sizeof(int16_t) * 4);

  // Read blockmap offsets
  levels[level].blockmap.offset.resize(levels[level].blockmap.column * levels[level].blockmap.row);
  file.read((char *)levels[level].blockmap.offset.data(), sizeof(int16_t) * (levels[level].blockmap.column * levels[level].blockmap.row));

  // Read blocklists
  levels[level].blockmap.blocklist.resize((lump.size - (sizeof(int16_t) * 4 + sizeof(int16_t) * (levels[level].blockmap.column * levels[level].blockmap.row))) / sizeof(int16_t));
  file.read((char *)levels[level].blockmap.blocklist.data(), lump.size - (sizeof(int16_t) * 4 + sizeof(int16_t) * (levels[level].blockmap.column * levels[level].blockmap.row)));

  return true;
}

bool	Game::Wad::loadIgnore()
{
  // Ignore lump
  return true;
}