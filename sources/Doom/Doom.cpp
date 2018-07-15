#include <iostream>

#include "Doom/Doom.hpp"
#include "Doom/Flat/AbstractFlat.hpp"
#include "Doom/Linedef/AbstractLinedef.hpp"
#include "Doom/Sector/BlinkLightingAction.hpp"
#include "Doom/Sector/DoorLevelingAction.hpp"
#include "Doom/Sector/FlickerLightingAction.hpp"
#include "Doom/Sector/OscillateLightingAction.hpp"
#include "Doom/Sector/RandomLightingAction.hpp"
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
    std::unique_ptr<DOOM::AbstractFlat>	converted = DOOM::AbstractFlat::factory(wad, flat.first, flat.second);

    // Check for error
    if (converted.get() == nullptr)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    else
      resources.flats[flat.first] = std::move(converted);
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
  for (DOOM::Wad::RawLevel::Sector const & sector : wad.levels[level].sectors)
    this->level.sectors.emplace_back(*this, sector);
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
    std::unique_ptr<DOOM::AbstractLinedef>	converted = DOOM::AbstractLinedef::factory(*this, linedef);

    // Check for error
    if (converted.get() == nullptr)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    
    // Push linedef in vector
    this->level.linedefs.push_back(std::move(converted));
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
    std::unique_ptr<DOOM::AbstractThing>	converted = DOOM::AbstractThing::factory(*this, thing);

    // Check for error
    if (converted.get() == nullptr)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    this->level.things.push_back(std::move(converted));
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
  for (DOOM::Doom::Level::Sector & sector : sectors)
    sector.update(elapsed);

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

DOOM::Doom::Level::Sector::Sector(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector) :
  floor_name(sector.floor_texture),
  ceiling_name(sector.ceiling_texture),
  floor_flat(nullptr),
  ceiling_flat(nullptr),
  tag(sector.tag),
  special(sector.special),
  _light(sector.light), _baseLight(sector.light),
  _floor(sector.floor_height), _baseFloor(sector.floor_height),
  _ceiling(sector.ceiling_height), _baseCeiling(sector.ceiling_height),
  _damage(0.f), _baseDamage(0.f),
  _neighbors(),
  _doom(doom),
  _leveling(nullptr),
  _lighting(nullptr)
{
  // Check for errors
  if (_floor > _ceiling ||
    (floor_name != DOOM::str_to_key("F_SKY1") && doom.resources.flats.find(floor_name) == doom.resources.flats.end()) ||
    (ceiling_name != DOOM::str_to_key("F_SKY1") && doom.resources.flats.find(ceiling_name) == doom.resources.flats.end()) ||
    _light < 0 || _light > 255) {
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }

  // Retrieve flat textures
  if (floor_name != DOOM::str_to_key("F_SKY1"))
    floor_flat = doom.resources.flats.find(floor_name)->second.get();
  if (ceiling_name != DOOM::str_to_key("F_SKY1"))
    ceiling_flat = doom.resources.flats.find(ceiling_name)->second.get();

  // Index of this sector
  int16_t	index = (int16_t)doom.level.sectors.size();

  // Compute neighbor sectors
  for (const DOOM::Wad::RawLevel::Linedef & linedef : doom.wad.levels.find(doom.level.episode)->second.linedefs) {
    if (doom.wad.levels.find(doom.level.episode)->second.sidedefs[linedef.front].sector == index && linedef.back != -1)
      _neighbors.push_back(doom.wad.levels.find(doom.level.episode)->second.sidedefs[linedef.back].sector);
    if (linedef.back != -1 && doom.wad.levels.find(doom.level.episode)->second.sidedefs[linedef.back].sector == index)
      _neighbors.push_back(doom.wad.levels.find(doom.level.episode)->second.sidedefs[linedef.front].sector);
  }

  // Sort sector list
  std::sort(_neighbors.begin(), _neighbors.end());

  // Unique sector list
  _neighbors.resize(std::distance(_neighbors.begin(), std::unique(_neighbors.begin(), _neighbors.end())));

  // Push action for specific specials
  switch (sector.special)
  {
  case DOOM::Doom::Level::Sector::Special::Normal:
    break;
  case DOOM::Doom::Level::Sector::Special::LightBlinkRandom:
    _lighting.reset(new DOOM::RandomLightingAction<24, 4>());
    break;
  case DOOM::Doom::Level::Sector::Special::LightBlink05:
    _lighting.reset(new DOOM::BlinkLightingAction<15, 4, false>());
    break;
  case DOOM::Doom::Level::Sector::Special::LightBlink10:
    _lighting.reset(new DOOM::BlinkLightingAction<35, 4, false>());
    break;
  case DOOM::Doom::Level::Sector::Special::Damage20Blink05:
    break;	// TODO
  case DOOM::Doom::Level::Sector::Special::Damage10:
    _damage = 10.f;
    break;
  case DOOM::Doom::Level::Sector::Special::Damage5:
    _damage = 5.f;
    break;
  case DOOM::Doom::Level::Sector::Special::LightOscillates:
    _lighting.reset(new DOOM::OscillateLightingAction());
    break;
  case DOOM::Doom::Level::Sector::Special::Secret:
    break;	// TODO
  case DOOM::Doom::Level::Sector::Special::DoorClose:
    _leveling.reset(new DOOM::DoorLevelingAction({ DOOM::DoorLevelingAction::State::StateWait, DOOM::DoorLevelingAction::State::StateClose }, DOOM::DoorLevelingAction::Speed::SpeedSlow, (int)(sf::seconds(30.f).asMicroseconds() / DOOM::Doom::Tic.asMicroseconds())));
    break;
  case DOOM::Doom::Level::Sector::Special::End:
    break;	// TODO
  case DOOM::Doom::Level::Sector::Special::LightBlink05Sync:
    _lighting.reset(new DOOM::BlinkLightingAction<15, 4, true>());
    break;
  case DOOM::Doom::Level::Sector::Special::LightBlink10Sync:
    _lighting.reset(new DOOM::BlinkLightingAction<35, 4, true>());
    break;
  case DOOM::Doom::Level::Sector::Special::DoorOpen:
    _leveling.reset(new DOOM::DoorLevelingAction({ DOOM::DoorLevelingAction::State::StateWait, DOOM::DoorLevelingAction::State::StateOpen }, DOOM::DoorLevelingAction::Speed::SpeedSlow, (int)(sf::seconds(300.f).asMicroseconds() / DOOM::Doom::Tic.asMicroseconds())));
    break;
  case DOOM::Doom::Level::Sector::Special::Damage20:
    _damage = 20.f;
    break;
  case DOOM::Doom::Level::Sector::Special::LightFlickers:
    _lighting.reset(new DOOM::FlickerLightingAction());
    break;

  default:
    break;
  }
}

DOOM::Doom::Level::Sector::Sector(DOOM::Doom::Level::Sector && sector) :
  floor_name(sector.floor_name),
  ceiling_name(sector.ceiling_name),
  floor_flat(sector.floor_flat),
  ceiling_flat(sector.ceiling_flat),
  tag(sector.tag),
  special(sector.special),
  _light(sector._light), _baseLight(sector._baseLight),
  _floor(sector._floor), _baseFloor(sector._baseFloor),
  _ceiling(sector._ceiling), _baseCeiling(sector._baseCeiling),
  _damage(sector._damage), _baseDamage(sector._baseDamage),
  _neighbors(std::move(sector._neighbors)),
  _doom(sector._doom),
  _leveling(std::move(sector._leveling)),
  _lighting(std::move(sector._lighting))
{}

DOOM::Doom::Level::Sector::~Sector()
{}

void	DOOM::Doom::Level::Sector::update(sf::Time elapsed)
{
  // Update sector actions
  if (_leveling.get() != nullptr)
    _leveling->update(*this, elapsed);
  if (_lighting.get() != nullptr)
    _lighting->update(*this, elapsed);
}

void	DOOM::Doom::Level::Sector::leveling(int16_t type)
{
  // Push action if leveling slot available
  if (_leveling.get() == nullptr)
    _leveling = std::move(DOOM::Doom::Level::Sector::AbstractAction::factory(*this, type));
}

void	DOOM::Doom::Level::Sector::lighting(int16_t type)
{
  // Push action if lighting slot available
  if (_lighting.get() == nullptr)
    _lighting = std::move(DOOM::Doom::Level::Sector::AbstractAction::factory(*this, type));
}

float	DOOM::Doom::Level::Sector::getNeighborLowestFloor() const
{
  float	result = std::numeric_limits<float>::quiet_NaN();

  // Find lowest neighboor floor
  for (int16_t index : _neighbors)
    result = std::isnan(result) == true ? _doom.level.sectors[index].baseFloor() : std::min(result, _doom.level.sectors[index].baseFloor());

  return result;
}

float	DOOM::Doom::Level::Sector::getNeighborHighestFloor() const
{
  float	result = std::numeric_limits<float>::quiet_NaN();

  // Find highest neighboor floor
  for (int16_t index : _neighbors)
    result = std::isnan(result) == true ? _doom.level.sectors[index].baseFloor() : std::max(result, _doom.level.sectors[index].baseFloor());

  return result;
}

float	DOOM::Doom::Level::Sector::getNeighborNextLowestFloor(float height) const
{
  float		result = std::numeric_limits<float>::quiet_NaN();

  // Find next lowest neighboor floor
  for (int16_t index : _neighbors) {
    float	floor = _doom.level.sectors[index].baseFloor();

    if (floor < height && (std::isnan(result) == true || floor > result))
      result = std::max(result, floor);
  }

  return result;
}

float	DOOM::Doom::Level::Sector::getNeighborNextHighestFloor(float height) const
{
  float		result = std::numeric_limits<float>::quiet_NaN();

  // Find next highest neighboor floor
  for (int16_t index : _neighbors) {
    float	floor = _doom.level.sectors[index].baseFloor();

    if (floor > height && (std::isnan(result) == true || floor < result))
      result = std::min(result, floor);
  }

  return result;
}

float	DOOM::Doom::Level::Sector::getNeighborLowestCeiling() const
{
  float	result = std::numeric_limits<float>::quiet_NaN();

  // Find lowest neighboor ceiling
  for (int16_t index : _neighbors)
    result = std::isnan(result) == true ? _doom.level.sectors[index].baseCeiling() : std::min(result, _doom.level.sectors[index].baseCeiling());

  return result;
}

float	DOOM::Doom::Level::Sector::getNeighborHighestCeiling() const
{
  float	result = std::numeric_limits<float>::quiet_NaN();

  // Find highest neighboor ceiling
  for (int16_t index : _neighbors)
    result = std::isnan(result) == true ? _doom.level.sectors[index].baseCeiling() : std::max(result, _doom.level.sectors[index].baseCeiling());

  return result;
}

float	DOOM::Doom::Level::Sector::getNeighborNextLowestCeiling(float height) const
{
  float		result = std::numeric_limits<float>::quiet_NaN();

  // Find next lowest neighboor ceiling
  for (int16_t index : _neighbors) {
    float	ceiling = _doom.level.sectors[index].baseCeiling();

    if (ceiling < height && (std::isnan(result) == true || ceiling > result))
      result = std::max(result, ceiling);
  }

  return result;
}

float	DOOM::Doom::Level::Sector::getNeighborNextHighestCeiling(float height) const
{
  float		result = std::numeric_limits<float>::quiet_NaN();

  // Find next highest neighboor floor
  for (int16_t index : _neighbors) {
    float	ceiling = _doom.level.sectors[index].baseCeiling();

    if (ceiling > height && (std::isnan(result) == true || ceiling < result))
      result = std::min(result, ceiling);
  }

  return result;
}

int16_t	DOOM::Doom::Level::Sector::getNeighborLowestLight() const
{
  int16_t	result = light();

  // Find lowest neighboor light level
  for (int16_t index : _neighbors)
    result = std::min(result, _doom.level.sectors[index].baseLight());

  return result;
}

int16_t	DOOM::Doom::Level::Sector::getNeighborHighestLight() const
{
  int16_t	result = light();

  // Find highest neighboor light level
  for (int16_t index : _neighbors)
    result = std::min(result, _doom.level.sectors[index].baseLight());

  return result;
}

DOOM::Doom::Level::Sector::AbstractAction::AbstractAction()
{}

DOOM::Doom::Level::Sector::AbstractAction::~AbstractAction()
{}

std::unique_ptr<DOOM::Doom::Level::Sector::AbstractAction>	DOOM::Doom::Level::Sector::AbstractAction::factory(DOOM::Doom::Level::Sector & sector, int16_t type)
{
  // Generate action from type
  switch (type) {

    // Regular and extended door types
  case 1: case 4: case 29: case 63: case 90:	// Open, wait then close (slow)
    return std::unique_ptr<DOOM::DoorLevelingAction>(new DOOM::DoorLevelingAction({ DOOM::DoorLevelingAction::State::StateOpen, DOOM::DoorLevelingAction::State::StateWait, DOOM::DoorLevelingAction::State::StateClose }, DOOM::DoorLevelingAction::Speed::SpeedSlow, (int)(sf::seconds(4.f).asMicroseconds() / DOOM::Doom::Tic.asMicroseconds())));
  case 105: case 108: case 111: case 114: case 117:	// Open, wait then close (fast)
    return std::unique_ptr<DOOM::DoorLevelingAction>(new DOOM::DoorLevelingAction({ DOOM::DoorLevelingAction::State::StateOpen, DOOM::DoorLevelingAction::State::StateWait, DOOM::DoorLevelingAction::State::StateClose }, DOOM::DoorLevelingAction::Speed::SpeedFast, (int)(sf::seconds(4.f).asMicroseconds() / DOOM::Doom::Tic.asMicroseconds())));
  case 2: case 31: case 46: case 61: case 86: case 103:	// Open and stay open (slow)
    return std::unique_ptr<DOOM::DoorLevelingAction>(new DOOM::DoorLevelingAction({ DOOM::DoorLevelingAction::State::StateOpen }, DOOM::DoorLevelingAction::Speed::SpeedSlow));
  case 106: case 109: case 112: case 115: case 118:	// Open and stay open (fast)
    return std::unique_ptr<DOOM::DoorLevelingAction>(new DOOM::DoorLevelingAction({ DOOM::DoorLevelingAction::State::StateOpen }, DOOM::DoorLevelingAction::Speed::SpeedFast));
  case 16: case 76: case 175: case 196:	// Close, wait then open (slow)
    return std::unique_ptr<DOOM::DoorLevelingAction>(new DOOM::DoorLevelingAction({ DOOM::DoorLevelingAction::State::StateClose, DOOM::DoorLevelingAction::State::StateWait, DOOM::DoorLevelingAction::State::StateOpen }, DOOM::DoorLevelingAction::Speed::SpeedNormal, (int)(sf::seconds(30.f).asMicroseconds() / DOOM::Doom::Tic.asMicroseconds())));
  case 3: case 42: case 50: case 75:	// Close and stay close (slow)
    return std::unique_ptr<DOOM::DoorLevelingAction>(new DOOM::DoorLevelingAction({ DOOM::DoorLevelingAction::State::StateClose }, DOOM::DoorLevelingAction::Speed::SpeedSlow));
  case 107: case 110: case 113: case 116:	// Close and stay close (fast)
    return std::unique_ptr<DOOM::DoorLevelingAction>(new DOOM::DoorLevelingAction({ DOOM::DoorLevelingAction::State::StateClose }, DOOM::DoorLevelingAction::Speed::SpeedFast));

    // Regular and extended locked door types
  case 32: case 33: case 34:	// Open and stay open (slow)
    return std::unique_ptr<DOOM::DoorLevelingAction>(new DOOM::DoorLevelingAction({ DOOM::DoorLevelingAction::State::StateOpen }, DOOM::DoorLevelingAction::Speed::SpeedSlow));
  case 99: case 133: case 134: case 135: case 136: case 137:	// Open and stay open (fast)
    return std::unique_ptr<DOOM::DoorLevelingAction>(new DOOM::DoorLevelingAction({ DOOM::DoorLevelingAction::State::StateOpen }, DOOM::DoorLevelingAction::Speed::SpeedFast));
  case 26: case 27: case 28:	// Open, wait then close (slow)
    return std::unique_ptr<DOOM::DoorLevelingAction>(new DOOM::DoorLevelingAction({ DOOM::DoorLevelingAction::State::StateOpen, DOOM::DoorLevelingAction::State::StateWait, DOOM::DoorLevelingAction::State::StateClose }, DOOM::DoorLevelingAction::Speed::SpeedSlow, (int)(sf::seconds(4.f).asMicroseconds() / DOOM::Doom::Tic.asMicroseconds())));

  default:
    return nullptr;
  }

  return nullptr;
}

void	DOOM::Doom::Level::Sector::AbstractAction::remove(DOOM::Doom::Level::Sector & sector)
{
  // Remove action from sector
  if (sector._leveling.get() == this)
    sector._leveling.reset();
  else if (sector._lighting.get() == this)
    sector._lighting.reset();
}

void	DOOM::Doom::Level::Sector::AbstractAction::stop()
{}
