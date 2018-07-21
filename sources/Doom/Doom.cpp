#include <iostream>

#include "Doom/Doom.hpp"
#include "Doom/Action/BlinkLightingAction.hpp"
#include "Doom/Action/CeilingLevelingAction.hpp"
#include "Doom/Action/DoorLevelingAction.hpp"
#include "Doom/Action/FlickerLightingAction.hpp"
#include "Doom/Action/FloorLevelingAction.hpp"
#include "Doom/Action/OscillateLightingAction.hpp"
#include "Doom/Action/RandomLightingAction.hpp"

sf::Time const	DOOM::Doom::Tic = sf::seconds(1.f / 35.f);

DOOM::Doom::Resources::Texture const	DOOM::Doom::Resources::Texture::Null = DOOM::Doom::Resources::Texture();

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
  resources.update(*this, elapsed);
  level.update(*this, elapsed);
}

std::list<std::pair<uint8_t, uint8_t>>	DOOM::Doom::getLevel() const
{
  std::list<std::pair<uint8_t, uint8_t>>	list;

  // Build list of available levels in WAD file
  for (std::pair<std::pair<uint8_t, uint8_t>, DOOM::Wad::RawLevel> const & level : wad.levels)
    list.emplace_back(level.first);

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
  resources.palettes = std::array<DOOM::Doom::Resources::Palette, 14>();
  resources.colormaps = std::array<DOOM::Doom::Resources::Colormap, 34>();
  resources.flats.clear();
  resources.textures.clear();
  resources.sprites.clear();
  resources.menus.clear();
  resources.sounds.clear();
}

void	DOOM::Doom::clearLevel()
{
  // Reset level base info
  level.episode = { 0, 0 };
  level.sky = std::ref(DOOM::Doom::Resources::Texture::Null);

  // Reset level components
  level.things.clear();
  level.linedefs.clear();
  level.sidedefs.clear();
  level.vertexes.clear();
  level.segments.clear();
  level.subsectors.clear();
  level.nodes.clear();
  level.sectors.clear();
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
  resources.update(*this, sf::Time::Zero);
}

void	DOOM::Doom::buildResourcesPalettes()
{
  // Check palettes data
  if (wad.resources.palettes.size() != 14)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Load palettes from WAD resources
  for (unsigned int index = 0; index < 14; index++)
    resources.palettes[index] = DOOM::Doom::Resources::Palette(*this, wad.resources.palettes[index]);
}

void	DOOM::Doom::buildResourcesColormaps()
{
  // Check color maps data
  if (wad.resources.colormaps.size() != 34)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Load color maps from WAD resources
  for (unsigned int index = 0; index < 34; index++)
    resources.colormaps[index] = DOOM::Doom::Resources::Colormap(*this, wad.resources.colormaps[index]);
}

void	DOOM::Doom::buildResourcesTextures()
{
  // Load textures from WAD resources
  for (std::pair<uint64_t, DOOM::Wad::RawResources::Texture> const & texture : wad.resources.textures)
    resources.textures.emplace(std::piecewise_construct, std::forward_as_tuple(texture.first), std::forward_as_tuple(*this, texture.second));
}

void	DOOM::Doom::buildResourcesSprites()
{
  // Load sprites textures from WAD resources
  for (std::pair<uint64_t, DOOM::Wad::RawResources::Patch> const & patch : wad.resources.sprites)
    resources.sprites.emplace(std::piecewise_construct, std::forward_as_tuple(patch.first), std::forward_as_tuple(*this, patch.second));
}

void	DOOM::Doom::buildResourcesMenus()
{
  // Load menus textures from WAD resources
  for (std::pair<uint64_t, DOOM::Wad::RawResources::Patch> const & menu : wad.resources.menus)
    resources.menus.emplace(std::piecewise_construct, std::forward_as_tuple(menu.first), std::forward_as_tuple(*this, menu.second));
}

void	DOOM::Doom::buildResourcesFlats()
{
  // Load flats from WAD resources
  for (std::pair<uint64_t, DOOM::Wad::RawResources::Flat> const & flat : wad.resources.flats)
  {
    // Convert flat from WAD
    std::unique_ptr<DOOM::AbstractFlat>	converted = DOOM::AbstractFlat::factory(*this, flat.first, flat.second);

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
    resources.sounds.emplace(std::piecewise_construct, std::forward_as_tuple(sound.first), std::forward_as_tuple(*this, sound.second));
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
  this->level.sky = std::cref(resources.textures[0x0000000000594B53 | (((int64_t)level.first + '0') << 24)]);

  // Build every component of resources
  try
  {
    buildLevelVertexes();
    buildLevelSectors();
    buildLevelLinedefs();
    buildLevelSidedefs();
    buildLevelSubsectors();
    buildLevelThings();
    buildLevelSegments();
    buildLevelNodes();
    buildLevelBlockmap();
  }
  catch (std::exception e)
  {
    clearLevel();
    throw std::runtime_error(e.what());
  }

  // Update components (might be useful for initializations)
  this->level.update(*this, sf::Time::Zero);
}

void	DOOM::Doom::buildLevelVertexes()
{
  // Load level's vertexes from WAD
  for (DOOM::Wad::RawLevel::Vertex const & vertex : wad.levels[level.episode].vertexes)
    level.vertexes.emplace_back(*this, vertex);
}

void	DOOM::Doom::buildLevelSectors()
{
  // Load level's sectors from WAD
  for (DOOM::Wad::RawLevel::Sector const & sector : wad.levels[level.episode].sectors)
    level.sectors.emplace_back(*this, sector);
}

void	DOOM::Doom::buildLevelSubsectors()
{
  // Load level's sectors from WAD
  for (DOOM::Wad::RawLevel::Subsector const & subsector : wad.levels[level.episode].subsectors)
    level.subsectors.emplace_back(*this, subsector);
}

void	DOOM::Doom::buildLevelLinedefs()
{
  // Load level's linedefs from WAD
  for (DOOM::Wad::RawLevel::Linedef const & linedef : wad.levels[level.episode].linedefs)
  {
    // Convert linedef from WAD
    std::unique_ptr<DOOM::AbstractLinedef>	converted = DOOM::AbstractLinedef::factory(*this, linedef);

    // Check for error
    if (converted.get() == nullptr)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    else
      level.linedefs.emplace_back(std::move(converted));
  }
}

void	DOOM::Doom::buildLevelSidedefs()
{
  // Load level's sidedefs from WAD
  for (DOOM::Wad::RawLevel::Sidedef const & sidedef : wad.levels[level.episode].sidedefs)
    level.sidedefs.emplace_back(*this, sidedef);
}

void	DOOM::Doom::buildLevelSegments()
{
  // Load level's segments from WAD
  for (DOOM::Wad::RawLevel::Segment const & segment : wad.levels[level.episode].segments)
    level.segments.emplace_back(*this, segment);
}

void	DOOM::Doom::buildLevelNodes()
{
  // Load level's nodes from WAD
  for (DOOM::Wad::RawLevel::Node const & node : wad.levels[level.episode].nodes)
    level.nodes.emplace_back(*this, node);
}

void	DOOM::Doom::buildLevelThings()
{
  // Load level's things from WAD
  for (DOOM::Wad::RawLevel::Thing const & thing : wad.levels[level.episode].things)
  {
    // Convert thing from WAD
    std::unique_ptr<DOOM::AbstractThing>	converted = DOOM::AbstractThing::factory(*this, thing);

    // Check for error
    if (converted.get() == nullptr)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    else
      level.things.emplace_back(std::move(converted));
  }
}

void	DOOM::Doom::buildLevelBlockmap()
{
  // Convert WAD bockmap
  level.blockmap = DOOM::Doom::Level::Blockmap(*this, wad.levels[level.episode].blockmap);
}

void	DOOM::Doom::Resources::update(DOOM::Doom & doom, sf::Time elapsed)
{
  // Update resources flats
  for (const std::pair<const uint64_t, std::unique_ptr<DOOM::AbstractFlat>> & flat : flats)
    flat.second->update(doom, elapsed);
}

void	DOOM::Doom::Level::update(DOOM::Doom & doom, sf::Time elapsed)
{
  // Update level things
  for (const std::unique_ptr<DOOM::AbstractThing> & thing : things)
    thing->update(doom, elapsed);

  // Update level sectors
  for (DOOM::Doom::Level::Sector & sector : sectors)
    sector.update(doom, elapsed);

  // Update level linedef
  for (const std::unique_ptr<DOOM::AbstractLinedef> & linedef : linedefs)
    linedef->update(doom, elapsed);

  // Update level sidedef
  for (DOOM::Doom::Level::Sidedef & sidedef : sidedefs)
    sidedef.update(doom, elapsed);
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

DOOM::Doom::Resources::Palette::Palette(DOOM::Doom & doom, const DOOM::Wad::RawResources::Palette & palette) :
  std::array<sf::Color, 256>()
{
  // Convert WAD color to sf::Color
  for (unsigned int index = 0; index < 256; index++)
    at(index) = sf::Color(palette.colors[index].r, palette.colors[index].g, palette.colors[index].b);
}

DOOM::Doom::Resources::Colormap::Colormap(DOOM::Doom & doom, const DOOM::Wad::RawResources::Colormap & colormap) :
  std::array<uint8_t, 256>()
{
  // Convert color map indexes
  for (unsigned int index = 0; index < 256; index++)
    at(index) = colormap.index[index];
}

DOOM::Doom::Resources::Texture::Texture(DOOM::Doom & doom, const DOOM::Wad::RawResources::Texture & texture) :
  width(texture.width),
  height(texture.height),
  left(0),
  top(0),
  columns()
{
  // Initialize full texture map (-1 for transparency)
  std::vector<std::vector<int>>	texture_map(width, std::vector<int>(height, -1));

  // Build full texture map from texture patches
  for (DOOM::Wad::RawResources::Texture::Patch const & texture_patch : texture.patches)
  {
    // Check patch datas
    if (texture_patch.colormap != 0 ||
      texture_patch.stepdir != 1 ||
      texture_patch.pname >= doom.wad.resources.pnames.size() ||
      doom.wad.resources.patches.find(doom.wad.resources.pnames[texture_patch.pname]) == doom.wad.resources.patches.end())
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    // Get patch from WAD
    DOOM::Wad::RawResources::Patch const &	patch = doom.wad.resources.patches[doom.wad.resources.pnames[texture_patch.pname]];

    // Print patch on full texture map
    for (int x = std::max(0, -patch.left); x < std::min((int)patch.width, width - texture_patch.x); x++)
      for (DOOM::Wad::RawResources::Patch::Column::Span span : patch.columns[x].spans)
	for (int y = std::max(0, -(texture_patch.y + span.offset)); y < std::min((int)span.pixels.size(), height - (texture_patch.y + span.offset)); y++)
	  if (x + texture_patch.x >= 0 && x + texture_patch.x < width && y + texture_patch.y + span.offset >= 0 && y + texture_patch.y + span.offset < height)
	    texture_map[x + texture_patch.x][y + texture_patch.y + span.offset] = span.pixels[y];
  }

  // Allocate number of column of texture according to its width
  columns.resize(width);

  // Iterate through every pixel of the texture
  for (int x = 0; x < width; x++)
    for (int y = 0; y < height;)
    {
      // Ignore pixel if transparent
      if (texture_map[x][y] == -1)
	y++;

      // Add column span if not transparent
      else
      {
	columns[x].spans.push_back(DOOM::Doom::Resources::Texture::Column::Span());
	columns[x].spans.back().offset = y;

	// Push whole span of pixels in column span
	for (; y < height && texture_map[x][y] != -1; y++)
	  columns[x].spans.back().pixels.push_back(texture_map[x][y]);
      }
    }
}

DOOM::Doom::Resources::Texture::Texture(DOOM::Doom & doom, const DOOM::Wad::RawResources::Patch & patch) :
  width(patch.width),
  height(patch.height),
  left(patch.left),
  top(patch.top),
  columns()
{
  // Copy texture data structures
  for (DOOM::Wad::RawResources::Patch::Column const & column : patch.columns)
  {
    columns.push_back(DOOM::Doom::Resources::Texture::Column());
    for (DOOM::Wad::RawResources::Patch::Column::Span const & span : column.spans)
    {
      columns.back().spans.push_back(DOOM::Doom::Resources::Texture::Column::Span());
      columns.back().spans.back().offset = span.offset;
      for (const uint8_t pixel : span.pixels)
	columns.back().spans.back().pixels.push_back(pixel);
    }
  }
}

DOOM::Doom::Resources::Sound::Sound(DOOM::Doom & doom, const DOOM::Wad::RawResources::Sound & raw) :
  buffer(),
  sound()
{
  std::vector<int16_t>	converted;

  // Convert uint8 format to int16
  for (const uint8_t sample : raw.buffer)
    converted.push_back((int32_t)sample * 256 - 32768);

  // Load sound buffer (mono)
  if (buffer.loadFromSamples(converted.data(), raw.samples, 1, raw.rate) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Attribute soundbuffer to sound
  sound.setBuffer(buffer);
}

DOOM::Doom::Level::Level() :
  episode(0, 0),
  sky(DOOM::Doom::Resources::Texture::Null),
  things(),
  linedefs(),
  sidedefs(),
  vertexes(),
  segments(),
  subsectors(),
  nodes(),
  sectors(),
  blockmap()
{}

DOOM::Doom::Level::Vertex::Vertex(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Vertex & vertex) :
  Math::Vector<2>((float)vertex.x, (float)vertex.y)
{}

DOOM::Doom::Level::Sidedef::Sidedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sidedef & sidedef) :
  x(sidedef.x), y(sidedef.y), sector(sidedef.sector), _elapsed(),
  _upper(animation(doom, sidedef.upper)),
  _lower(animation(doom, sidedef.lower)),
  _middle(animation(doom, sidedef.middle))
{
  // Check for errors
  if (sidedef.sector < 0 || sidedef.sector >= doom.wad.levels[doom.level.episode].sectors.size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>>	DOOM::Doom::Level::Sidedef::animation(const DOOM::Doom & doom, uint64_t name) const
{
  // Check for null texture
  if (DOOM::key_to_str(name) == "-")
    return { DOOM::Doom::Resources::Texture::Null };

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

  std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>>	result;

  // Check if frames are registered in resources
  for (uint64_t frame : animation) {
    if (doom.resources.textures.find(frame) == doom.resources.textures.end())
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    result.push_back(std::cref(doom.resources.textures.find(frame)->second));
  }

  // Return animation
  return result;
}

void	DOOM::Doom::Level::Sidedef::update(DOOM::Doom & doom, sf::Time elapsed)
{
  // Add elapsed time to total
  _elapsed += elapsed;
}

const DOOM::Doom::Resources::Texture &	DOOM::Doom::Level::Sidedef::upper() const
{
  // Return upper frame
  return _upper[_elapsed.asMicroseconds() / (DOOM::Doom::Tic.asMicroseconds() * DOOM::Doom::Level::Sidedef::FrameDuration) % _upper.size()];
}

const DOOM::Doom::Resources::Texture &	DOOM::Doom::Level::Sidedef::lower() const
{
  // Return lower frame
  return _lower[_elapsed.asMicroseconds() / (DOOM::Doom::Tic.asMicroseconds() * DOOM::Doom::Level::Sidedef::FrameDuration) % _lower.size()];
}

const DOOM::Doom::Resources::Texture &	DOOM::Doom::Level::Sidedef::middle() const
{
  // Return middle frame
  return _middle[_elapsed.asMicroseconds() / (DOOM::Doom::Tic.asMicroseconds() * DOOM::Doom::Level::Sidedef::FrameDuration) % _middle.size()];
}

DOOM::Doom::Level::Segment::Segment(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Segment & segment) :
  start(segment.start),
  end(segment.end),
  angle(segment.angle / 32768.f * Math::Pi),
  linedef(segment.linedef),
  direction(segment.direction),
  offset(segment.offset)
{
  // Check for errors
  if (segment.start < 0 || segment.start >= doom.wad.levels[doom.level.episode].segments.size() ||
    segment.end < 0 || segment.end >= doom.wad.levels[doom.level.episode].segments.size() ||
    segment.linedef < 0 || segment.linedef >= doom.wad.levels[doom.level.episode].linedefs.size() ||
    (segment.direction != 0 && segment.direction != 1))
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

DOOM::Doom::Level::Subsector::Subsector(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Subsector & subsector) :
  count(subsector.count),
  index(subsector.index),
  sector(0)
{
  // Check for errors
  if (subsector.index < 0 || subsector.count < 0 ||
    subsector.index + subsector.count > doom.wad.levels[doom.level.episode].segments.size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Find sector of subsector
  sector = doom.wad.levels[doom.level.episode].sidedefs[
    doom.wad.levels[doom.level.episode].segments[subsector.index].direction == 0 ?
      doom.wad.levels[doom.level.episode].linedefs[doom.wad.levels[doom.level.episode].segments[subsector.index].linedef].front :
      doom.wad.levels[doom.level.episode].linedefs[doom.wad.levels[doom.level.episode].segments[subsector.index].linedef].back
  ].sector;
}

DOOM::Doom::Level::Node::Node(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Node & node) :
  origin((float)node.origin_x, (float)node.origin_y),
  direction((float)node.direction_x, (float)node.direction_y),
  rightbound(
    {
      node.right_bb.top,
      node.right_bb.bottom,
      node.right_bb.left,
      node.right_bb.right
    }),
  leftbound(
    {
      node.left_bb.top,
      node.left_bb.bottom,
      node.left_bb.left,
      node.left_bb.right
    }),
  rightchild(node.right_ss),
  leftchild(node.left_ss)

{
  // Check for errors
  if ((node.direction_x == 0 && node.direction_y == 0) ||
    node.right_bb.top < node.right_bb.bottom || node.right_bb.right < node.right_bb.left ||
    node.left_bb.top < node.left_bb.bottom || node.left_bb.right < node.left_bb.left ||
    ((node.right_ss & 0b1000000000000000) ? ((node.right_ss & 0b0111111111111111) >= doom.wad.levels[doom.level.episode].subsectors.size()) : (node.right_ss >= doom.wad.levels[doom.level.episode].nodes.size())) ||
    ((node.left_ss & 0b1000000000000000) ? ((node.left_ss & 0b0111111111111111) >= doom.wad.levels[doom.level.episode].subsectors.size()) : (node.left_ss >= doom.wad.levels[doom.level.episode].nodes.size())))
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

DOOM::Doom::Level::Blockmap::Blockmap(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Blockmap & blockmap) :
  x(blockmap.x),
  y(blockmap.y),
  column(blockmap.column),
  row(blockmap.row)
{
  // Load blockmap from WAD
  for (const uint16_t offset : blockmap.offset)
  {
    uint16_t	index = (uint16_t)(offset - (4 + blockmap.offset.size()));

    // Check first blocklist delimiter
    if (index >= blockmap.blocklist.size() || blockmap.blocklist[index] != 0x0000)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    // Skip blocklist delimiter
    index += 1;

    // Push new block in blockmap
    blocks.push_back(DOOM::Doom::Level::Blockmap::Block());

    // Iterate over blocklist
    for (; index < blockmap.blocklist.size() && blockmap.blocklist[index] != (int16_t)0xFFFF; index++)
    {
      // Check for errors
      if (blockmap.blocklist[index] < 0 || blockmap.blocklist[index] >= doom.wad.levels[doom.level.episode].linedefs.size())
	throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

      blocks.back().linedefs.push_back(blockmap.blocklist[index]);
    }

    // Check last blocklist delimiter
    if (index >= blockmap.blocklist.size() || blockmap.blocklist[index] != (int16_t)0xFFFF)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
}

DOOM::Doom::Level::Sector::Sector(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector) :
  floor_name(sector.floor_texture),
  ceiling_name(sector.ceiling_texture),
  floor_flat(DOOM::AbstractFlat::Null),
  ceiling_flat(DOOM::AbstractFlat::Null),
  light_current(sector.light), light_base(sector.light),
  floor_current(sector.floor_height), floor_base(sector.floor_height),
  ceiling_current(sector.ceiling_height), ceiling_base(sector.ceiling_height),
  damage(0.f),
  tag(sector.tag),
  special(sector.special),
  _neighbors(),
  _actions()
{
  // Check for errors
  if ((floor_name != DOOM::str_to_key("F_SKY1") && doom.resources.flats.find(floor_name) == doom.resources.flats.end()) ||
    (ceiling_name != DOOM::str_to_key("F_SKY1") && doom.resources.flats.find(ceiling_name) == doom.resources.flats.end()) ||
    light_current < 0 || light_current > 255)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Retrieve flat textures
  if (floor_name != DOOM::str_to_key("F_SKY1"))
    floor_flat = std::cref(*doom.resources.flats.find(floor_name)->second.get());
  if (ceiling_name != DOOM::str_to_key("F_SKY1"))
    ceiling_flat = std::cref(*doom.resources.flats.find(ceiling_name)->second.get());

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
    action<DOOM::EnumAction::Type::TypeLighting>(std::make_unique<DOOM::RandomLightingAction<24, 4>>(doom));
    break;
  case DOOM::Doom::Level::Sector::Special::LightBlink05:
    action<DOOM::EnumAction::Type::TypeLighting>(std::make_unique<DOOM::BlinkLightingAction<15, 5, false>>(doom));
    break;
  case DOOM::Doom::Level::Sector::Special::LightBlink10:
    action<DOOM::EnumAction::Type::TypeLighting>(std::make_unique<DOOM::BlinkLightingAction<35, 5, false>>(doom));
    break;
  case DOOM::Doom::Level::Sector::Special::Damage20Blink05:
    break;	// TODO
  case DOOM::Doom::Level::Sector::Special::Damage10:
    damage = 10.f;
    break;
  case DOOM::Doom::Level::Sector::Special::Damage5:
    damage = 5.f;
    break;
  case DOOM::Doom::Level::Sector::Special::LightOscillates:
    action<DOOM::EnumAction::Type::TypeLighting>(std::make_unique<DOOM::OscillateLightingAction<>>(doom));
    break;
  case DOOM::Doom::Level::Sector::Special::Secret:
    break;	// TODO
  case DOOM::Doom::Level::Sector::Special::DoorClose:
    action<DOOM::EnumAction::Type::TypeLeveling>(std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Speed::SpeedSlow, 1050>>(doom, std::list<DOOM::EnumAction::State>({ DOOM::EnumAction::State::StateWait, DOOM::EnumAction::State::StateClose })));
    break;
  case DOOM::Doom::Level::Sector::Special::End:
    break;	// TODO
  case DOOM::Doom::Level::Sector::Special::LightBlink05Sync:
    action<DOOM::EnumAction::Type::TypeLighting>(std::make_unique<DOOM::BlinkLightingAction<15, 5, true>>(doom));
    break;
  case DOOM::Doom::Level::Sector::Special::LightBlink10Sync:
    action<DOOM::EnumAction::Type::TypeLighting>(std::make_unique<DOOM::BlinkLightingAction<35, 5, true>>(doom));
    break;
  case DOOM::Doom::Level::Sector::Special::DoorOpen:
    action<DOOM::EnumAction::Type::TypeLeveling>(std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Speed::SpeedSlow, 10500>>(doom, std::list<DOOM::EnumAction::State>({ DOOM::EnumAction::State::StateWait, DOOM::EnumAction::State::StateOpen })));
    break;
  case DOOM::Doom::Level::Sector::Special::Damage20:
    damage = 20.f;
    break;
  case DOOM::Doom::Level::Sector::Special::LightFlickers:
    action<DOOM::EnumAction::Type::TypeLighting>(std::make_unique<DOOM::FlickerLightingAction<>>(doom));
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
  light_current(sector.light_current), light_base(sector.light_base),
  floor_current(sector.floor_current), floor_base(sector.floor_base),
  ceiling_current(sector.ceiling_current), ceiling_base(sector.ceiling_base),
  damage(sector.damage),
  tag(sector.tag),
  special(sector.special),
  _neighbors(std::move(sector._neighbors)),
  _actions(std::move(sector._actions))
{}

std::unique_ptr<DOOM::AbstractAction>	DOOM::Doom::Level::Sector::_factory(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, int16_t type)
{
  // Just a relay (cycling inclusion problem)
  return DOOM::AbstractAction::factory(doom, sector, type);
}

void	DOOM::Doom::Level::Sector::update(DOOM::Doom & doom, sf::Time elapsed)
{
  // Update sector actions
  for (unsigned int type = 0; type < DOOM::EnumAction::Type::TypeNumber; type++)
    if (_actions.at(type).get() != nullptr)
      _actions.at(type)->update(doom, *this, elapsed);
}

float	DOOM::Doom::Level::Sector::getNeighborLowestFloor(const DOOM::Doom & doom) const
{
  float	result = std::numeric_limits<float>::quiet_NaN();

  // Find lowest neighboor floor
  for (int16_t index : _neighbors)
    result = std::isnan(result) == true ? doom.level.sectors[index].floor_base : std::min(result, doom.level.sectors[index].floor_base);

  return result;
}

float	DOOM::Doom::Level::Sector::getNeighborHighestFloor(const DOOM::Doom & doom) const
{
  float	result = std::numeric_limits<float>::quiet_NaN();

  // Find highest neighboor floor
  for (int16_t index : _neighbors)
    result = std::isnan(result) == true ? doom.level.sectors[index].floor_base : std::max(result, doom.level.sectors[index].floor_base);

  return result;
}

float	DOOM::Doom::Level::Sector::getNeighborNextLowestFloor(const DOOM::Doom & doom, float height) const
{
  float		result = std::numeric_limits<float>::quiet_NaN();

  // Find next lowest neighboor floor
  for (int16_t index : _neighbors) {
    float	floor = doom.level.sectors[index].floor_base;

    if (floor < height && (std::isnan(result) == true || floor > result))
      result = std::max(result, floor);
  }

  return result;
}

float	DOOM::Doom::Level::Sector::getNeighborNextHighestFloor(const DOOM::Doom & doom, float height) const
{
  float		result = std::numeric_limits<float>::quiet_NaN();

  // Find next highest neighboor floor
  for (int16_t index : _neighbors) {
    float	floor = doom.level.sectors[index].floor_base;

    if (floor > height && (std::isnan(result) == true || floor < result))
      result = std::min(result, floor);
  }

  return result;
}

float	DOOM::Doom::Level::Sector::getNeighborLowestCeiling(const DOOM::Doom & doom) const
{
  float	result = std::numeric_limits<float>::quiet_NaN();

  // Find lowest neighboor ceiling
  for (int16_t index : _neighbors)
    result = std::isnan(result) == true ? doom.level.sectors[index].ceiling_base : std::min(result, doom.level.sectors[index].ceiling_base);

  return result;
}

float	DOOM::Doom::Level::Sector::getNeighborHighestCeiling(const DOOM::Doom & doom) const
{
  float	result = std::numeric_limits<float>::quiet_NaN();

  // Find highest neighboor ceiling
  for (int16_t index : _neighbors)
    result = std::isnan(result) == true ? doom.level.sectors[index].ceiling_base : std::max(result, doom.level.sectors[index].ceiling_base);

  return result;
}

float	DOOM::Doom::Level::Sector::getNeighborNextLowestCeiling(const DOOM::Doom & doom, float height) const
{
  float		result = std::numeric_limits<float>::quiet_NaN();

  // Find next lowest neighboor ceiling
  for (int16_t index : _neighbors) {
    float	ceiling = doom.level.sectors[index].ceiling_base;

    if (ceiling < height && (std::isnan(result) == true || ceiling > result))
      result = std::max(result, ceiling);
  }

  return result;
}

float	DOOM::Doom::Level::Sector::getNeighborNextHighestCeiling(const DOOM::Doom & doom, float height) const
{
  float		result = std::numeric_limits<float>::quiet_NaN();

  // Find next highest neighboor floor
  for (int16_t index : _neighbors) {
    float	ceiling = doom.level.sectors[index].ceiling_base;

    if (ceiling > height && (std::isnan(result) == true || ceiling < result))
      result = std::min(result, ceiling);
  }

  return result;
}

int16_t	DOOM::Doom::Level::Sector::getNeighborLowestLight(const DOOM::Doom & doom) const
{
  int16_t	result = light_base;

  // Find lowest neighboor light level
  for (int16_t index : _neighbors)
    result = std::min(result, doom.level.sectors[index].light_base);

  return result;
}

int16_t	DOOM::Doom::Level::Sector::getNeighborHighestLight(const DOOM::Doom & doom) const
{
  int16_t	result = light_base;

  // Find highest neighboor light level
  for (int16_t index : _neighbors)
    result = std::min(result, doom.level.sectors[index].light_base);

  return result;
}