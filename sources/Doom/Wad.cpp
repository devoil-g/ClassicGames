#include <cmath>
#include <functional>
#include <iostream>
#include <list>
#include <regex>

#include "Doom/Wad.hpp"
#include "System/Config.hpp"

DOOM::Wad::Wad() :
  levels(),
  resources()
{}

DOOM::Wad::~Wad()
{}

bool	DOOM::Wad::load(std::string const & path)
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

bool	DOOM::Wad::loadLumps(std::ifstream & file, int32_t const numlumps, int32_t const infotableofs)
{
  std::pair<uint8_t, uint8_t>	level = { 0, 0 };

  // Process every lump of file
  for (int32_t iterator = 0; iterator < numlumps; iterator++)
  {
    // Lump header data structure
    DOOM::Wad::Lump	lump;

    // Read lump header data from file
    file.seekg(infotableofs + iterator * sizeof(DOOM::Wad::Lump), file.beg);
    file.read((char *)&lump, sizeof(DOOM::Wad::Lump));

    // Force name format
    uppercase(lump.name);

    // Initialize regex command map
    std::list<std::pair<std::regex, std::function<bool()>>>	commands_regex =
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
      { std::regex("DMXGUS"), std::bind(&DOOM::Wad::loadIgnore, this) },
      { std::regex("D_[[:alnum:]]+"), std::bind(&DOOM::Wad::loadResourceMusic, this, std::ref(file), std::ref(lump)) },
      { std::regex("DS[[:alnum:]]+"), std::bind(&DOOM::Wad::loadResourceSound, this, std::ref(file), std::ref(lump)) },
      { std::regex("DP[[:alnum:]]+"), std::bind(&DOOM::Wad::loadIgnore, this) },
      { std::regex("DEMO[[:digit:]]"), std::bind(&DOOM::Wad::loadResourceDemox, this, std::ref(file), std::ref(lump)) },
      
      // Menu resources lumps
      { std::regex("HELP1"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
      { std::regex("HELP2"), std::bind(&DOOM::Wad::loadResourceMenu, this, std::ref(file), std::ref(lump)) },
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

      // Level lumps
      { std::regex("VERTEXES"), std::bind(&DOOM::Wad::loadLevelVertexes, this, std::ref(file), std::ref(lump), level) },
      { std::regex("SECTORS"), std::bind(&DOOM::Wad::loadLevelSectors, this, std::ref(file), std::ref(lump), level) },
      { std::regex("SIDEDEFS"), std::bind(&DOOM::Wad::loadLevelSidedefs, this, std::ref(file), std::ref(lump), level) },
      { std::regex("LINEDEFS"), std::bind(&DOOM::Wad::loadLevelLinedefs, this, std::ref(file), std::ref(lump), level) },
      { std::regex("SEGS"), std::bind(&DOOM::Wad::loadLevelSegs, this, std::ref(file), std::ref(lump), level) },
      { std::regex("SSECTORS"), std::bind(&DOOM::Wad::loadLevelSsectors, this, std::ref(file), std::ref(lump), level) },
      { std::regex("NODES"), std::bind(&DOOM::Wad::loadLevelNodes, this, std::ref(file), std::ref(lump), level) },
      { std::regex("REJECT"), std::bind(&DOOM::Wad::loadLevelReject, this, std::ref(file), std::ref(lump), level) },
      { std::regex("BLOCKMAP"), std::bind(&DOOM::Wad::loadLevelBlockmap, this, std::ref(file), std::ref(lump), level) },
      { std::regex("THINGS"), std::bind(&DOOM::Wad::loadLevelThings, this, std::ref(file), std::ref(lump), level) },
      { std::regex("E[[:digit:]]M[[:digit:]]"), std::bind(&DOOM::Wad::loadLevelExmy, this, std::ref(lump), std::ref(level)) },
      { std::regex("MAP[[:digit:]][[:digit:]]"), std::bind(&DOOM::Wad::loadLevelMapxy, this, std::ref(lump), std::ref(level)) }
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

bool	DOOM::Wad::loadResourceFlats(std::ifstream & file, int32_t const numlumps, int32_t const infotableofs, int32_t & iterator)
{
  // Process every lump in scope
  for (; iterator < numlumps; iterator++)
  {
    // Lump header data structure
    DOOM::Wad::Lump	lump;

    // Read lump header data from file
    file.seekg(infotableofs + iterator * sizeof(DOOM::Wad::Lump), file.beg);
    file.read((char *)&lump, sizeof(DOOM::Wad::Lump));

    // Force name format
    uppercase(lump.name);
    
    // Stop if end of scope
    if (lump.name == DOOM::str_to_key("F_END"))
      return true;

    // Load flat lump
    if (loadResourceFlat(file, lump) == false)
      return false;
  }

  return false;
}

bool	DOOM::Wad::loadResourceSprites(std::ifstream & file, int32_t const numlumps, int32_t const infotableofs, int32_t & iterator)
{
  // Process every lump in scope
  for (; iterator < numlumps; iterator++)
  {
    // Lump header data structure
    DOOM::Wad::Lump	lump;

    // Read lump header data from file
    file.seekg(infotableofs + iterator * sizeof(DOOM::Wad::Lump), file.beg);
    file.read((char *)&lump, sizeof(DOOM::Wad::Lump));

    // Force name format
    uppercase(lump.name);

    // Stop if end of scope
    if (lump.name == DOOM::str_to_key("S_END"))
      return true;

    // Load flat lump
    if (loadResourceSprite(file, lump) == false)
      return false;
  }

  return false;
}

bool	DOOM::Wad::loadResourcePatches(std::ifstream & file, int32_t const numlumps, int32_t const infotableofs, int32_t & iterator)
{
  // Process every lump in scope
  for (; iterator < numlumps; iterator++)
  {
    // Lump header data structure
    DOOM::Wad::Lump	lump;

    // Read lump header data from file
    file.seekg(infotableofs + iterator * sizeof(DOOM::Wad::Lump), file.beg);
    file.read((char *)&lump, sizeof(DOOM::Wad::Lump));

    // Force name format
    uppercase(lump.name);

    // Stop if end of scope
    if (lump.name == DOOM::str_to_key("P_END"))
      return true;

    // Load patch lump
    if (loadResourcePatch(file, lump) == false)
      return false;
  }

  return false;
}

bool	DOOM::Wad::loadResourceTexture(std::ifstream & file, DOOM::Wad::Lump const & lump)
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
      resources.textures[name].patches.push_back(DOOM::Wad::RawResources::Texture::Patch());
      file.read((char *)&resources.textures[name].patches.back(), sizeof(DOOM::Wad::RawResources::Texture::Patch));
    }
  }

  return true;
}

bool	DOOM::Wad::loadResourceSprite(std::ifstream & file, DOOM::Wad::Lump const & lump)
{
  // Ignore special delimiters (TODO: dont ignore them ?)
  if (lump.name == DOOM::str_to_key("S_START") || lump.name == DOOM::str_to_key("S_END"))
    return true;

  return loadResourcePatch(file, lump, resources.sprites);
}

bool	DOOM::Wad::loadResourcePatch(std::ifstream & file, DOOM::Wad::Lump const & lump)
{
  // Ignore special delimiters (TODO: dont ignore them ?)
  if (lump.name == DOOM::str_to_key("P_START") || lump.name == DOOM::str_to_key("P_END") ||
    lump.name == DOOM::str_to_key("P1_START") || lump.name == DOOM::str_to_key("P1_END") ||
    lump.name == DOOM::str_to_key("P2_START") || lump.name == DOOM::str_to_key("P2_END") ||
    lump.name == DOOM::str_to_key("P3_START") || lump.name == DOOM::str_to_key("P3_END"))
    return true;

  return loadResourcePatch(file, lump, resources.patches);
}

bool	DOOM::Wad::loadResourceMenu(std::ifstream & file, DOOM::Wad::Lump const & lump)
{
  return loadResourcePatch(file, lump, resources.menus);
}

bool	DOOM::Wad::loadResourcePatch(std::ifstream & file, DOOM::Wad::Lump const & lump, std::unordered_map<uint64_t, DOOM::Wad::RawResources::Patch> & target)
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
    target[lump.name].columns.push_back(DOOM::Wad::RawResources::Patch::Column());

    // Read each column spans
    while (true)
    {
      DOOM::Wad::RawResources::Patch::Column::Span	span;
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

bool	DOOM::Wad::loadResourceFlat(std::ifstream & file, DOOM::Wad::Lump const & lump)
{
  // Ignore special delimiters (TODO: dont ignore them ?)
  if (lump.name == DOOM::str_to_key("F_START") || lump.name == DOOM::str_to_key("F_END") ||
    lump.name == DOOM::str_to_key("F1_START") || lump.name == DOOM::str_to_key("F1_END") ||
    lump.name == DOOM::str_to_key("F2_START") || lump.name == DOOM::str_to_key("F2_END"))
    return true;

  // Check for invalid lump size
  if (lump.size != sizeof(uint8_t) * 64 * 64)
    return false;

  // Read flat data from file
  file.seekg(lump.position, file.beg);
  file.read((char *)resources.flats[lump.name].texture, sizeof(uint8_t) * 64 * 64);

  return true;
}

bool	DOOM::Wad::loadResourceGenmidi(std::ifstream & file, DOOM::Wad::Lump const & lump)
{
  // Check for invalid lump size
  if (lump.size != sizeof(int8_t) * 8 + sizeof(DOOM::Wad::RawResources::Genmidi) * 175)
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
    DOOM::Wad::RawResources::Genmidi	record;

    // Read record data
    file.seekg(lump.position + sizeof(int8_t) * 8 + (sizeof(DOOM::Wad::RawResources::Genmidi) - sizeof(int8_t) * 32) * i, file.beg);
    file.read((char *)&record, sizeof(DOOM::Wad::RawResources::Genmidi) - sizeof(int8_t) * 32);

    // Read record name
    file.seekg(lump.position + sizeof(int8_t) * 8 + (sizeof(DOOM::Wad::RawResources::Genmidi) - sizeof(int8_t) * 32) * 175 + sizeof(int8_t) * 32 * i, file.beg);
    file.read((char *)record.name, sizeof(int8_t) * 32);

    // Push record to record vector
    resources.genmidis.push_back(record);
  }

  return true;
}

bool	DOOM::Wad::loadResourceMusic(std::ifstream & file, DOOM::Wad::Lump const & lump)
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

bool	DOOM::Wad::loadResourceSound(std::ifstream & file, DOOM::Wad::Lump const & lump)
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

bool	DOOM::Wad::loadResourceDemox(std::ifstream & file, DOOM::Wad::Lump const & lump)
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

    DOOM::Wad::RawResources::Demo::Record	record;

    // Read whole record
    file.read((char *)&record, sizeof(DOOM::Wad::RawResources::Demo::Record));

    // Push record to player move vector
    resources.demos[(lump.name << 32 & 0xFF) - '0'].records.push_back(record);
  }
}

bool	DOOM::Wad::loadResourcePlaypal(std::ifstream & file, DOOM::Wad::Lump const & lump)
{
  return loadLump<DOOM::Wad::RawResources::Palette>(file, lump, resources.palettes);
}

bool	DOOM::Wad::loadResourceColormap(std::ifstream & file, DOOM::Wad::Lump const & lump)
{
  return loadLump<DOOM::Wad::RawResources::Colormap>(file, lump, resources.colormaps);
}

bool	DOOM::Wad::loadResourcePnames(std::ifstream & file, DOOM::Wad::Lump const & lump)
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

bool	DOOM::Wad::loadResourceEndoom(std::ifstream & file, DOOM::Wad::Lump const & lump)
{
  // Check for invalid lump size
  if (lump.size != sizeof(DOOM::Wad::RawResources::Endoom))
    return false;

  // Read header from file
  file.seekg(lump.position, file.beg);
  file.read((char *)&resources.endoom, sizeof(DOOM::Wad::RawResources::Endoom));

  return true;
}

bool	DOOM::Wad::loadLevelExmy(DOOM::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> & level)
{
  // Get episode and mission number from name
  level = { (uint8_t)((lump.name >> 8) & 0xFF) - '0', (uint8_t)((lump.name >> 24) & 0xFF) - '0' };

  return true;
}

bool	DOOM::Wad::loadLevelMapxy(DOOM::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> & level)
{
  // Get episode and mission number from name
  level = { (uint8_t)((lump.name >> 24) & 0xFF) - '0', (uint8_t)((lump.name >> 32) & 0xFF) - '0' };

  return true;
}

bool	DOOM::Wad::loadLevelThings(std::ifstream & file, DOOM::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  return loadLump<DOOM::Wad::RawLevel::Thing>(file, lump, levels[level].things);
}

bool	DOOM::Wad::loadLevelLinedefs(std::ifstream & file, DOOM::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  return loadLump<DOOM::Wad::RawLevel::Linedef>(file, lump, levels[level].linedefs);
}

bool	DOOM::Wad::loadLevelSidedefs(std::ifstream & file, DOOM::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  // Load sidedefs from file
  if (loadLump<DOOM::Wad::RawLevel::Sidedef>(file, lump, levels[level].sidedefs) == false)
    return false;

  // Convert name in sidedefs to uppercase
  for (DOOM::Wad::RawLevel::Sidedef & sidedef : levels[level].sidedefs)
  {
    uppercase(sidedef.upper);
    uppercase(sidedef.lower);
    uppercase(sidedef.middle);
  }

  return true;
}

bool	DOOM::Wad::loadLevelVertexes(std::ifstream & file, DOOM::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  return loadLump<DOOM::Wad::RawLevel::Vertex>(file, lump, levels[level].vertexes);
}

bool	DOOM::Wad::loadLevelSegs(std::ifstream & file, DOOM::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  return loadLump<DOOM::Wad::RawLevel::Segment>(file, lump, levels[level].segments);
}

bool	DOOM::Wad::loadLevelSsectors(std::ifstream & file, DOOM::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  return loadLump<DOOM::Wad::RawLevel::Subsector>(file, lump, levels[level].subsectors);
}

bool	DOOM::Wad::loadLevelNodes(std::ifstream & file, DOOM::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  return loadLump<DOOM::Wad::RawLevel::Node>(file, lump, levels[level].nodes);
}

bool	DOOM::Wad::loadLevelSectors(std::ifstream & file, DOOM::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  if (loadLump<DOOM::Wad::RawLevel::Sector>(file, lump, levels[level].sectors) == false)
    return false;

  // Convert name in sectors to uppercase
  for (DOOM::Wad::RawLevel::Sector & sector : levels[level].sectors)
  {
    uppercase(sector.floor_texture);
    uppercase(sector.ceiling_texture);
  }

  return true;
}

bool	DOOM::Wad::loadLevelReject(std::ifstream & file, DOOM::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
{
  return loadLump<uint8_t>(file, lump, levels[level].reject.rejects);
}

bool	DOOM::Wad::loadLevelBlockmap(std::ifstream & file, DOOM::Wad::Lump const & lump, std::pair<uint8_t, uint8_t> level)
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

bool	DOOM::Wad::loadIgnore()
{
  // Ignore lump
  return true;
}