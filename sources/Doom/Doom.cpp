#include <iostream>

#include "Doom/Doom.hpp"
#include "Doom/Flat/AbstractFlat.hpp"
#include "Doom/Linedef/AbstractLinedef.hpp"
#include "Doom/Sector/AbstractSector.hpp"
#include "Doom/Thing/AbstractThing.hpp"

sf::Time const	DOOM::Doom::Tic = sf::seconds(1.f / 35.f);

DOOM::Doom::Resources::Texture const	DOOM::Doom::Resources::NullTexture = { 0,0,0,0,{} };

DOOM::Doom::Doom() :
  resources(),
  level()
{}

DOOM::Doom::~Doom()
{
  // Delete every resources loaded
  clear();
}

void	DOOM::Doom::load(std::string const & path)
{
  // Clear resources
  clear();
  
  // Load WAD file
  wad.load(path);

  // Build resources of WAD file
  buildResources();
}

void	DOOM::Doom::update(sf::Time elapsed)
{
  // Update components
  resources.update(elapsed);
  level.update(elapsed);
}

std::list<std::pair<uint8_t, uint8_t>>	DOOM::Doom::getLevel() const
{
  std::list<std::pair<uint8_t, uint8_t>>	list;

  // Build list of available levels in WAD file
  for (std::pair<std::pair<uint8_t, uint8_t>, DOOM::Wad::RawLevel> const & level : wad.levels)
    list.push_back(level.first);

  return list;
}

void	DOOM::Doom::setLevel(std::pair<uint8_t, uint8_t> const & level)
{
  // Build level
  buildLevel(level);
}

void	DOOM::Doom::clear()
{
  // Clear resources and current level
  clearResources();
  clearLevel();
}

void	DOOM::Doom::clearResources()
{
  // Clear level to avoid reference to deleted datas
  clearLevel();

  // Clear resources data containers
  resources.palettes.clear();
  resources.colormaps.clear();
  resources.textures.clear();
  resources.sprites.clear();
  resources.menus.clear();
  resources.flats.clear();
  resources.sounds.clear();
}

void	DOOM::Doom::clearLevel()
{
  // Set default level identifier
  level.episode = { 0, 0 };
  level.sky = DOOM::Doom::Resources::NullTexture;

  // Clear level data containers
  level.things.clear();
  level.linedefs.clear();
  level.sidedefs.clear();
  level.vertexes.clear();
  level.segments.clear();
  level.subsectors.clear();
  level.nodes.clear();
  level.sectors.clear();
  
  // Reset blockmap
  level.blockmap = DOOM::Doom::Level::Blockmap();
}

void	DOOM::Doom::buildResources()
{
  // Remove old resources and level
  clear();

  // Build every component of resources
  try
  {
    buildResourcesPalettes();
    buildResourcesColormaps();
    buildResourcesTextures();
    buildResourcesSprites();
    buildResourcesMenus();
    buildResourcesFlats();
    buildResourcesSounds();
  }
  catch (std::exception e)
  {
    clear();
    throw std::runtime_error(e.what());
  }

  // Update components (might be useful for initializations)
  resources.update(sf::seconds(0.f));
}

void	DOOM::Doom::buildResourcesPalettes()
{
  // Load palettes from WAD resources
  for (DOOM::Wad::RawResources::Palette const & palette : wad.resources.palettes)
  {
    // Push new palette in resources
    resources.palettes.push_back(DOOM::Doom::Resources::Palette());

    // Convert WAD color to sf::Color
    for (DOOM::Wad::RawResources::Palette::Color const & color : palette.colors)
      resources.palettes.back().push_back(sf::Color(color.r, color.g, color.b));
  }

  // Check palettes data
  if (resources.palettes.size() != 14)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void	DOOM::Doom::buildResourcesColormaps()
{
  // Load color maps from WAD resources
  for (DOOM::Wad::RawResources::Colormap const & colormap : wad.resources.colormaps)
  {
    // Push new color map in resources
    resources.colormaps.push_back(DOOM::Doom::Resources::Colormap());

    // Convert color map indexes
    for (uint8_t const & index : colormap.index)
      resources.colormaps.back().push_back(index);
  }

  // Check color maps data
  if (resources.colormaps.size() != 34)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void	DOOM::Doom::buildResourcesTextures()
{
  // Load textures from WAD resources
  for (std::pair<uint64_t, DOOM::Wad::RawResources::Texture> const & texture : wad.resources.textures)
  {
    // Copy texture size
    resources.textures[texture.first].width = texture.second.width;
    resources.textures[texture.first].height = texture.second.height;

    // Force texture position to 0
    resources.textures[texture.first].left = 0;
    resources.textures[texture.first].top = 0;

    // Initialize full texture map (-1 for transparency)
    std::vector<std::vector<int>>	texture_map(resources.textures[texture.first].width, std::vector<int>(resources.textures[texture.first].height, -1));

    // Build full texture map from texture patches
    for (DOOM::Wad::RawResources::Texture::Patch const & texture_patch : texture.second.patches)
    {
      // Check patch datas
      if (texture_patch.colormap != 0 ||
	texture_patch.stepdir != 1 ||
	texture_patch.pname >= wad.resources.pnames.size() ||
	wad.resources.patches.find(wad.resources.pnames[texture_patch.pname]) == wad.resources.patches.end())
	throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

      DOOM::Wad::RawResources::Patch const &	patch = wad.resources.patches[wad.resources.pnames[texture_patch.pname]];

      // Print patch on full texture map
      for (int x = std::max(0, -patch.left); x < std::min((int)patch.width, resources.textures[texture.first].width - texture_patch.x); x++)
	for (DOOM::Wad::RawResources::Patch::Column::Span span : patch.columns[x].spans)
	  for (int y = std::max(0, -(texture_patch.y + span.offset)); y < std::min((int)span.pixels.size(), resources.textures[texture.first].height - (texture_patch.y + span.offset)); y++)
	    if (x + texture_patch.x >= 0 && x + texture_patch.x < resources.textures[texture.first].width &&
	      y + texture_patch.y + span.offset >= 0 && y + texture_patch.y + span.offset < resources.textures[texture.first].height)
	      texture_map[x + texture_patch.x][y + texture_patch.y + span.offset] = span.pixels[y];
    }

    // Allocate number of column of texture according to its width
    resources.textures[texture.first].columns.resize(resources.textures[texture.first].width);

    // Iterate through every pixel of the screen
    for (int x = 0; x < resources.textures[texture.first].width; x++)
      for (int y = 0; y < resources.textures[texture.first].height;)
      {
	// Ignore pixel if transparent
	if (texture_map[x][y] == -1)
	  y++;

	// Add column span if not transparent
	else
	{
	  resources.textures[texture.first].columns[x].spans.push_back(DOOM::Doom::Resources::Texture::Column::Span());
	  resources.textures[texture.first].columns[x].spans.back().offset = y;

	  // Push whole span of pixels in column span
	  for (; y < resources.textures[texture.first].height && texture_map[x][y] != -1; y++)
	    resources.textures[texture.first].columns[x].spans.back().pixels.push_back(texture_map[x][y]);
	}
      }
  }
}

void	DOOM::Doom::buildResourcesSprites()
{
  // Load sprites textures from WAD resources
  for (std::pair<uint64_t, DOOM::Wad::RawResources::Patch> const & patch : wad.resources.sprites)
  {
    // Copy texture size and position
    resources.sprites[patch.first].width = patch.second.width;
    resources.sprites[patch.first].height = patch.second.height;
    resources.sprites[patch.first].left = patch.second.left;
    resources.sprites[patch.first].top = patch.second.top;

    // Copy texture data structures
    for (DOOM::Wad::RawResources::Patch::Column const & column : patch.second.columns)
    {
      resources.sprites[patch.first].columns.push_back(DOOM::Doom::Resources::Texture::Column());
      for (DOOM::Wad::RawResources::Patch::Column::Span const & span : column.spans)
      {
	resources.sprites[patch.first].columns.back().spans.push_back(DOOM::Doom::Resources::Texture::Column::Span());
	resources.sprites[patch.first].columns.back().spans.back().offset = span.offset;
	for (uint8_t const & pixel : span.pixels)
	  resources.sprites[patch.first].columns.back().spans.back().pixels.push_back(pixel);
      }
    }
  }
}

void	DOOM::Doom::buildResourcesMenus()
{
  // Load menus textures from WAD resources
  for (std::pair<uint64_t, DOOM::Wad::RawResources::Patch> const & patch : wad.resources.menus)
  {
    // Copy texture size and position
    resources.menus[patch.first].width = patch.second.width;
    resources.menus[patch.first].height = patch.second.height;
    resources.menus[patch.first].left = patch.second.left;
    resources.menus[patch.first].top = patch.second.top;

    // Copy texture data structures
    for (DOOM::Wad::RawResources::Patch::Column const & column : patch.second.columns)
    {
      resources.menus[patch.first].columns.push_back(DOOM::Doom::Resources::Texture::Column());
      for (DOOM::Wad::RawResources::Patch::Column::Span const & span : column.spans)
      {
	resources.menus[patch.first].columns.back().spans.push_back(DOOM::Doom::Resources::Texture::Column::Span());
	resources.menus[patch.first].columns.back().spans.back().offset = span.offset;
	for (uint8_t const & pixel : span.pixels)
	  resources.menus[patch.first].columns.back().spans.back().pixels.push_back(pixel);
      }
    }
  }
}

void	DOOM::Doom::buildResourcesFlats()
{
  // Load flats from WAD resources
  for (std::pair<uint64_t, DOOM::Wad::RawResources::Flat> const & flat : wad.resources.flats)
  {
    // Convert flat from WAD
    DOOM::AbstractFlat *	converted = DOOM::AbstractFlat::factory(wad, flat.first, flat.second);

    // Check for error
    if (converted == nullptr)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    else
      resources.flats[flat.first] = std::unique_ptr<DOOM::AbstractFlat>(converted);
  }
}

void	DOOM::Doom::buildResourcesSounds()
{
  // Load sounds from WAD resources
  for (std::pair<uint64_t, DOOM::Wad::RawResources::Sound> const & sound : wad.resources.sounds)
  {
    std::vector<int16_t>	buffer;

    // Convert uint8 format to int16
    for (uint8_t sample : sound.second.buffer)
      buffer.push_back((int32_t)sample * 256 - 32768);

    // Load sound buffer (mono)
    if (resources.sounds[sound.first].buffer.loadFromSamples(buffer.data(), sound.second.samples, 1, sound.second.rate) == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    // Attribute soundbuffer to sound
    resources.sounds[sound.first].sound.setBuffer(resources.sounds[sound.first].buffer);
  }
}

void	DOOM::Doom::buildLevel(std::pair<uint8_t, uint8_t> const & level)
{
  // Remove old level
  clearLevel();

  // Check if level exist in WAD file
  if (wad.levels.find(level) == wad.levels.end())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Check for sky texture
  if (resources.textures.find(0x0000000000594B53 | (((int64_t)level.first + '0') << 24)) == resources.textures.end())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  this->level.episode = level;
  this->level.sky = resources.textures[0x0000000000594B53 | (((int64_t)level.first + '0') << 24)];

  // Build every component of resources
  try
  {
    buildLevelVertexes(level);
    buildLevelSectors(level);
    buildLevelLinedefs(level);
    buildLevelSidedefs(level);
    buildLevelSubsectors(level);
    buildLevelThings(level);
    buildLevelSegments(level);
    buildLevelNodes(level);
    buildLevelBlockmap(level);
  }
  catch (std::exception e)
  {
    clearLevel();
    throw std::runtime_error(e.what());
  }

  // Update components (might be useful for initializations)
  this->level.update(sf::seconds(0.f));
}

void	DOOM::Doom::buildLevelVertexes(std::pair<uint8_t, uint8_t> const & level)
{
  // Load level's vertexes from WAD
  for (DOOM::Wad::RawLevel::Vertex const & vertex : wad.levels[level].vertexes)
    this->level.vertexes.push_back(DOOM::Doom::Level::Vertex((float)vertex.x, (float)vertex.y));
}

void	DOOM::Doom::buildLevelSectors(std::pair<uint8_t, uint8_t> const & level)
{
  // Load level's sectors from WAD
  for (DOOM::Wad::RawLevel::Sector const & sector : wad.levels[level].sectors) {
    DOOM::AbstractSector *	ptr = DOOM::AbstractSector::factory(*this, sector);

    if (ptr == nullptr)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    this->level.sectors.push_back(std::unique_ptr<DOOM::AbstractSector>(ptr));
  }
}

void	DOOM::Doom::buildLevelSubsectors(std::pair<uint8_t, uint8_t> const & level)
{
  // Load level's sectors from WAD
  for (DOOM::Wad::RawLevel::Subsector const & subsector : wad.levels[level].subsectors)
  {
    // Check for errors
    if (subsector.index < 0 || subsector.count < 0 ||
      subsector.index + subsector.count > wad.levels[level].segments.size())
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    // Convert data structure
    this->level.subsectors.push_back(DOOM::Doom::Level::Subsector{
      subsector.count,
      subsector.index,

      wad.levels[level].sidedefs[
	wad.levels[level].segments[subsector.index].direction == 0 ?
	  wad.levels[level].linedefs[wad.levels[level].segments[subsector.index].linedef].front :
	  wad.levels[level].linedefs[wad.levels[level].segments[subsector.index].linedef].back
      ].sector
    });
  }
}

void	DOOM::Doom::buildLevelLinedefs(std::pair<uint8_t, uint8_t> const & level)
{
  // Load level's linedefs from WAD
  for (DOOM::Wad::RawLevel::Linedef const & linedef : wad.levels[level].linedefs)
  {
    // Check for errors
    if (linedef.start < 0 || linedef.start >= wad.levels[level].vertexes.size() ||
      linedef.end < 0 || linedef.end >= wad.levels[level].vertexes.size() ||
      linedef.front < 0 || linedef.front >= wad.levels[level].sidedefs.size() ||
      linedef.back != -1 && (linedef.back < 0 || linedef.back >= wad.levels[level].sidedefs.size()))
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    // Convert linedef from WAD
    DOOM::AbstractLinedef *	converted = DOOM::AbstractLinedef::factory(*this, linedef);

    // Check for error
    if (converted == nullptr)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    
    // Push linedef in vector
    this->level.linedefs.push_back(std::unique_ptr<DOOM::AbstractLinedef>(converted));
  }
}

void	DOOM::Doom::buildLevelSidedefs(std::pair<uint8_t, uint8_t> const & level)
{
  // Load level's sidedefs from WAD
  for (DOOM::Wad::RawLevel::Sidedef const & sidedef : wad.levels[level].sidedefs)
    this->level.sidedefs.push_back(DOOM::Doom::Level::Sidedef(*this, sidedef));
}

void	DOOM::Doom::buildLevelSegments(std::pair<uint8_t, uint8_t> const & level)
{
  // Load level's segments from WAD
  for (DOOM::Wad::RawLevel::Segment const & segment : wad.levels[level].segments)
  {
    // Check for errors
    if (segment.start < 0 || segment.start >= wad.levels[level].segments.size() ||
      segment.end < 0 || segment.end >= wad.levels[level].segments.size() ||
      segment.linedef < 0 || segment.linedef >= wad.levels[level].linedefs.size() ||
      (segment.direction != 0 && segment.direction != 1))
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    // Convert data structure
    this->level.segments.push_back(DOOM::Doom::Level::Segment{
      segment.start,
      segment.end,
      segment.angle / 32768.f * Math::Pi,
      segment.linedef,
      segment.direction,
      segment.offset
    });
  }
}

void	DOOM::Doom::buildLevelNodes(std::pair<uint8_t, uint8_t> const & level)
{
  // Load level's nodes from WAD
  for (DOOM::Wad::RawLevel::Node const & node : wad.levels[level].nodes)
  {
    // Check for errors
    if ((node.direction_x == 0 && node.direction_y == 0) ||
      node.right_bb.top < node.right_bb.bottom || node.right_bb.right < node.right_bb.left ||
      node.left_bb.top < node.left_bb.bottom || node.left_bb.right < node.left_bb.left ||
      ((node.right_ss & 0b1000000000000000) ? ((node.right_ss & 0b0111111111111111) >= wad.levels[level].subsectors.size()) : (node.right_ss >= wad.levels[level].nodes.size())) ||
      ((node.left_ss & 0b1000000000000000) ? ((node.left_ss & 0b0111111111111111) >= wad.levels[level].subsectors.size()) : (node.left_ss >= wad.levels[level].nodes.size())))
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    // Convert data structure
    this->level.nodes.push_back(DOOM::Doom::Level::Node{
      Math::Vector<2>((float)node.origin_x, (float)node.origin_y),
      Math::Vector<2>((float)node.direction_x, (float)node.direction_y),
      {
	node.right_bb.top,
	node.right_bb.bottom,
	node.right_bb.left,
	node.right_bb.right
      },
      {
	node.left_bb.top,
	node.left_bb.bottom,
	node.left_bb.left,
	node.left_bb.right
      },
      node.right_ss,
      node.left_ss
    });
  }
}

void	DOOM::Doom::buildLevelThings(std::pair<uint8_t, uint8_t> const & level)
{
  // Load level's things from WAD
  for (DOOM::Wad::RawLevel::Thing const & thing : wad.levels[level].things)
  {
    // Convert thing from WAD
    DOOM::AbstractThing *	converted = DOOM::AbstractThing::factory(*this, thing);

    // Check for error
    if (converted == nullptr)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    this->level.things.push_back(std::unique_ptr<DOOM::AbstractThing>(converted));
  }
}

void	DOOM::Doom::buildLevelBlockmap(std::pair<uint8_t, uint8_t> const & level)
{
  // Copy basic informations
  this->level.blockmap.x = wad.levels[level].blockmap.x;
  this->level.blockmap.y = wad.levels[level].blockmap.y;
  this->level.blockmap.column = wad.levels[level].blockmap.column;
  this->level.blockmap.row = wad.levels[level].blockmap.row;

  // Load blockmap from WAD
  for (uint16_t offset : wad.levels[level].blockmap.offset)
  {
    uint16_t	index = (uint16_t)(offset - (4 + wad.levels[level].blockmap.offset.size()));

    // Check first blocklist delimiter
    if (index >= wad.levels[level].blockmap.blocklist.size() || wad.levels[level].blockmap.blocklist[index] != 0x0000)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    // Skip blocklist delimiter
    index += 1;

    // Push new block in blockmap
    this->level.blockmap.blocks.push_back(DOOM::Doom::Level::Blockmap::Block());

    // Iterate over blocklist
    for (; index < wad.levels[level].blockmap.blocklist.size() && wad.levels[level].blockmap.blocklist[index] != (int16_t)0xFFFF; index++)
      this->level.blockmap.blocks.back().linedefs.push_back(wad.levels[level].blockmap.blocklist[index]);

    // Check last blocklist delimiter
    if (index >= wad.levels[level].blockmap.blocklist.size() || wad.levels[level].blockmap.blocklist[index] != (int16_t)0xFFFF)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
}

std::pair<int16_t, int16_t>	DOOM::Doom::Level::sector(Math::Vector<2> const & position, int16_t index) const
{
  // Start to search sector from top node
  if (index == -1)
    return sector(position, (int16_t)nodes.size() - 1);

  // Draw subsector if node ID has subsector mask
  if (index & 0b1000000000000000)
    return { subsectors[index & 0b0111111111111111].sector, index & 0b0111111111111111 };

  DOOM::Doom::Level::Node const &	node(nodes[index]);
  
  // Use derterminant to find on which side the position is
  if (Math::Vector<2>::determinant(node.direction, position - node.origin) > 0.f)
    return sector(position, node.leftchild);
  else
    return sector(position, node.rightchild);
}

void	DOOM::Doom::Resources::update(sf::Time elapsed)
{
  // Update resources flats
  for (const std::pair<const uint64_t, std::unique_ptr<DOOM::AbstractFlat>> & flat : flats)
    flat.second->update(elapsed);
}

void	DOOM::Doom::Level::update(sf::Time elapsed)
{
  // Update level things
  for (const std::unique_ptr<DOOM::AbstractThing> & thing : things)
    thing->update(elapsed);

  // Update level sectors
  for (const std::unique_ptr<DOOM::AbstractSector> & sector : sectors)
    sector->update(elapsed);

  // Update level linedef
  for (const std::unique_ptr<DOOM::AbstractLinedef> & linedef : linedefs)
    linedef->update(elapsed);

  // Update level sidedef
  for (DOOM::Doom::Level::Sidedef & sidedef : sidedefs)
    sidedef.update(elapsed);
}

DOOM::Doom::Level::Sidedef::Sidedef(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sidedef & sidedef) :
  x(sidedef.x), y(sidedef.y), sector(sidedef.sector), _elapsed(),
  _upper(animation(doom, sidedef.upper)),
  _lower(animation(doom, sidedef.lower)),
  _middle(animation(doom, sidedef.middle))
{
  // Check for errors
  if (sidedef.sector < 0 || sidedef.sector >= doom.level.sectors.size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

DOOM::Doom::Level::Sidedef::~Sidedef()
{}

std::vector<const DOOM::Doom::Resources::Texture *>	DOOM::Doom::Level::Sidedef::animation(const DOOM::Doom & doom, uint64_t name) const
{
  // Check for null texture
  if (DOOM::key_to_str(name) == "-")
    return { &DOOM::Doom::Resources::NullTexture };

  // List of registered animations
  const static std::vector<std::vector<uint64_t>>	animations = {
    { DOOM::str_to_key("BLODGR1"), DOOM::str_to_key("BLODGR2"), DOOM::str_to_key("BLODGR3"), DOOM::str_to_key("BLODGR4") },
    { DOOM::str_to_key("BLODRIP1"), DOOM::str_to_key("BLODRIP2"), DOOM::str_to_key("BLODRIP3"), DOOM::str_to_key("BLODRIP4") },
    { DOOM::str_to_key("FIREBLU1"), DOOM::str_to_key("FIREBLU2") },
    { DOOM::str_to_key("FIRLAV3"), DOOM::str_to_key("FIRLAVA") },
    { DOOM::str_to_key("FIREMAG1"), DOOM::str_to_key("FIREMAG2"), DOOM::str_to_key("FIREMAG3") },
    { DOOM::str_to_key("FIREWALA"), DOOM::str_to_key("FIREWALB"), DOOM::str_to_key("FIREWALL") },
    { DOOM::str_to_key("GSTFONT1"), DOOM::str_to_key("GSTFONT2"), DOOM::str_to_key("GSTFONT3") },
    { DOOM::str_to_key("ROCKRED1"), DOOM::str_to_key("ROCKRED2"), DOOM::str_to_key("ROCKRED3") },
    { DOOM::str_to_key("SLADRIP1"), DOOM::str_to_key("SLADRIP2"), DOOM::str_to_key("SLADRIP3") },
    { DOOM::str_to_key("BFALL1"), DOOM::str_to_key("BFALL2"), DOOM::str_to_key("BFALL3"), DOOM::str_to_key("BFALL4") },
    { DOOM::str_to_key("SFALL1"), DOOM::str_to_key("SFALL2"), DOOM::str_to_key("SFALL3"), DOOM::str_to_key("SFALL4") },
    { DOOM::str_to_key("WFALL1"), DOOM::str_to_key("WFALL2"), DOOM::str_to_key("WFALL3"), DOOM::str_to_key("WFALL4") },
    { DOOM::str_to_key("DBRAIN1"), DOOM::str_to_key("DBRAIN2"), DOOM::str_to_key("DBRAIN3"), DOOM::str_to_key("DBRAIN4") }
  };

  std::vector<uint64_t>	animation = { name };

  // Find if frame is part of an animation
  for (const std::vector<uint64_t> & frames : animations)
    for (uint64_t frame : frames)
      if (name == frame)
	animation = frames;

  std::vector<const DOOM::Doom::Resources::Texture *>	result;

  // Check if frames are registered in resources
  for (uint64_t frame : animation) {
    if (doom.resources.textures.find(frame) == doom.resources.textures.end()) {
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }
    result.push_back(&doom.resources.textures.find(frame)->second);
  }
  
  // Return animation
  return result;
}

void	DOOM::Doom::Level::Sidedef::update(sf::Time elapsed)
{
  // Add elapsed time to total
  _elapsed += elapsed;
}

const DOOM::Doom::Resources::Texture &	DOOM::Doom::Level::Sidedef::upper() const
{
  // Return upper frame
  return *_upper[_elapsed.asMicroseconds() / (DOOM::Doom::Tic.asMicroseconds() * DOOM::Doom::Level::Sidedef::FrameDuration) % _upper.size()];
}

const DOOM::Doom::Resources::Texture &	DOOM::Doom::Level::Sidedef::lower() const
{
  // Return lower frame
  return *_lower[_elapsed.asMicroseconds() / (DOOM::Doom::Tic.asMicroseconds() * DOOM::Doom::Level::Sidedef::FrameDuration) % _lower.size()];
}

const DOOM::Doom::Resources::Texture &	DOOM::Doom::Level::Sidedef::middle() const
{
  // Return middle frame
  return *_middle[_elapsed.asMicroseconds() / (DOOM::Doom::Tic.asMicroseconds() * DOOM::Doom::Level::Sidedef::FrameDuration) % _middle.size()];
}
