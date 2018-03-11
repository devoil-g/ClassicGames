#include <iostream>

#include "Doom/Doom.hpp"
#include "Doom/Flat/AbstractFlat.hpp"
#include "Doom/Thing/AbstractThing.hpp"

sf::Time const	Game::Doom::Tic = sf::seconds(1.f / 35.f);

Game::Doom::Resources::Texture const	Game::Doom::Resources::NullTexture = { 0,0,0,0,{} };

Game::Doom::Doom() :
  resources(),
  level()
{}

Game::Doom::~Doom()
{
  // Delete every resources loaded
  clear();
}

bool	Game::Doom::load(std::string const & path)
{
  // Clear resources
  clear();

  // Attempt to load WAD file
  if (_wad.load(path) == false)
    return false;

  // Build resources of WAD file
  if (buildResources() == false)
    return false;

  return true;
}

void	Game::Doom::update(sf::Time elapsed)
{
  // Update components
  resources.update(elapsed);
  level.update(elapsed);
}

std::list<std::pair<uint8_t, uint8_t>>	Game::Doom::getLevel() const
{
  std::list<std::pair<uint8_t, uint8_t>>	list;

  // Build list of available levels in WAD file
  for (std::pair<std::pair<uint8_t, uint8_t>, Game::Wad::RawLevel> const & level : _wad.levels)
    list.push_back(level.first);

  return list;
}

bool	Game::Doom::setLevel(std::pair<uint8_t, uint8_t> const & level)
{
  // Build level
  return buildLevel(level);
}

void	Game::Doom::clear()
{
  // Clear resources and current level
  clearResources();
  clearLevel();
}

void	Game::Doom::clearResources()
{
  // Clear level to avoid reference to deleted datas
  clearLevel();

  // Delete allocated resources
  for (std::pair<uint64_t, Game::AbstractFlat *> const & flat : resources.flats)
    delete flat.second;

  // Clear resources data containers
  resources.palettes.clear();
  resources.colormaps.clear();
  resources.textures.clear();
  resources.sprites.clear();
  resources.menus.clear();
  resources.flats.clear();
  resources.sounds.clear();
}

void	Game::Doom::clearLevel()
{
  // Set default level identifier
  level.episode = { 0, 0 };
  level.sky = Game::Doom::Resources::NullTexture;

  // Delete allocated things
  for (Game::AbstractThing const * thing : level.things)
    delete thing;

  // Clear level data containers
  level.things.clear();
  level.linedefs.clear();
  level.sidedefs.clear();
  level.vertexes.clear();
  level.segments.clear();
  level.subsectors.clear();
  level.nodes.clear();
  level.sectors.clear();
}

bool	Game::Doom::buildResources()
{
  // Remove old resources and level
  clear();

  // Build every component of resources
  if (buildResourcesPalettes() == false ||
    buildResourcesColormaps() == false ||
    buildResourcesTextures() == false ||
    buildResourcesSprites() == false ||
    buildResourcesMenus() == false ||
    buildResourcesFlats() == false ||
    buildResourcesSounds() == false)
  {
    clear();
    return false;
  }

  // Update components (might be useful for initializations)
  resources.update(sf::seconds(0.f));

  return true;
}

bool	Game::Doom::buildResourcesPalettes()
{
  // Load palettes from WAD resources
  for (Game::Wad::RawResources::Palette const & palette : _wad.resources.palettes)
  {
    // Push new palette in resources
    resources.palettes.push_back(Game::Doom::Resources::Palette());

    // Convert WAD color to sf::Color
    for (Game::Wad::RawResources::Palette::Color const & color : palette.colors)
      resources.palettes.back().push_back(sf::Color(color.r, color.g, color.b));
  }

  // Check palettes data
  if (resources.palettes.size() != 14)
    return false;

  return true;
}

bool	Game::Doom::buildResourcesColormaps()
{
  // Load color maps from WAD resources
  for (Game::Wad::RawResources::Colormap const & colormap : _wad.resources.colormaps)
  {
    // Push new color map in resources
    resources.colormaps.push_back(Game::Doom::Resources::Colormap());

    // Convert color map indexes
    for (uint8_t const & index : colormap.index)
      resources.colormaps.back().push_back(index);
  }

  // Check color maps data
  if (resources.colormaps.size() != 34)
    return false;

  return true;
}

bool	Game::Doom::buildResourcesTextures()
{
  // Load textures from WAD resources
  for (std::pair<uint64_t, Game::Wad::RawResources::Texture> const & texture : _wad.resources.textures)
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
    for (Game::Wad::RawResources::Texture::Patch const & texture_patch : texture.second.patches)
    {
      // Check patch datas
      if (texture_patch.colormap != 0 ||
	texture_patch.stepdir != 1 ||
	texture_patch.pname >= _wad.resources.pnames.size() ||
	_wad.resources.patches.find(_wad.resources.pnames[texture_patch.pname]) == _wad.resources.patches.end())
	return false;

      Game::Wad::RawResources::Patch const &	patch = _wad.resources.patches[_wad.resources.pnames[texture_patch.pname]];

      // Print patch on full texture map
      for (int x = std::max(0, -patch.left); x < std::min((int)patch.width, resources.textures[texture.first].width - texture_patch.x); x++)
	for (Game::Wad::RawResources::Patch::Column::Span span : patch.columns[x].spans)
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
	  resources.textures[texture.first].columns[x].spans.push_back(Game::Doom::Resources::Texture::Column::Span());
	  resources.textures[texture.first].columns[x].spans.back().offset = y;

	  // Push whole span of pixels in column span
	  for (; y < resources.textures[texture.first].height && texture_map[x][y] != -1; y++)
	    resources.textures[texture.first].columns[x].spans.back().pixels.push_back(texture_map[x][y]);
	}
      }
  }

  return true;
}

bool	Game::Doom::buildResourcesSprites()
{
  // Load sprites textures from WAD resources
  for (std::pair<uint64_t, Game::Wad::RawResources::Patch> const & patch : _wad.resources.sprites)
  {
    // Copy texture size and position
    resources.sprites[patch.first].width = patch.second.width;
    resources.sprites[patch.first].height = patch.second.height;
    resources.sprites[patch.first].left = patch.second.left;
    resources.sprites[patch.first].top = patch.second.top;

    // Copy texture data structures
    for (Game::Wad::RawResources::Patch::Column const & column : patch.second.columns)
    {
      resources.sprites[patch.first].columns.push_back(Game::Doom::Resources::Texture::Column());
      for (Game::Wad::RawResources::Patch::Column::Span const & span : column.spans)
      {
	resources.sprites[patch.first].columns.back().spans.push_back(Game::Doom::Resources::Texture::Column::Span());
	resources.sprites[patch.first].columns.back().spans.back().offset = span.offset;
	for (uint8_t const & pixel : span.pixels)
	  resources.sprites[patch.first].columns.back().spans.back().pixels.push_back(pixel);
      }
    }
  }

  return true;
}

bool	Game::Doom::buildResourcesMenus()
{
  // Load menus textures from WAD resources
  for (std::pair<uint64_t, Game::Wad::RawResources::Patch> const & patch : _wad.resources.menus)
  {
    // Copy texture size and position
    resources.menus[patch.first].width = patch.second.width;
    resources.menus[patch.first].height = patch.second.height;
    resources.menus[patch.first].left = patch.second.left;
    resources.menus[patch.first].top = patch.second.top;

    // Copy texture data structures
    for (Game::Wad::RawResources::Patch::Column const & column : patch.second.columns)
    {
      resources.menus[patch.first].columns.push_back(Game::Doom::Resources::Texture::Column());
      for (Game::Wad::RawResources::Patch::Column::Span const & span : column.spans)
      {
	resources.menus[patch.first].columns.back().spans.push_back(Game::Doom::Resources::Texture::Column::Span());
	resources.menus[patch.first].columns.back().spans.back().offset = span.offset;
	for (uint8_t const & pixel : span.pixels)
	  resources.menus[patch.first].columns.back().spans.back().pixels.push_back(pixel);
      }
    }
  }

  return true;
}

bool	Game::Doom::buildResourcesFlats()
{
  // Load flats from WAD resources
  for (std::pair<uint64_t, Game::Wad::RawResources::Flat> const & flat : _wad.resources.flats)
  {
    // Convert flat from WAD
    Game::AbstractFlat *	converted = Game::AbstractFlat::factory(_wad, flat.first, flat.second);

    // Check for error
    if (converted == nullptr)
      std::cout << "[Doom::build] Warning, failed to convert flat '" << Game::key_to_str(flat.first) << "'." << std::endl;
    else
      resources.flats[flat.first] = converted;
  }

  return true;
}

bool	Game::Doom::buildResourcesSounds()
{
  // Load sounds from WAD resources
  for (std::pair<uint64_t, Game::Wad::RawResources::Sound> const & sound : _wad.resources.sounds)
  {
    std::vector<int16_t>	buffer;

    // Convert uint8 format to int16
    for (uint8_t sample : sound.second.buffer)
      buffer.push_back((int32_t)sample * 256 - 32768);

    // Load sound buffer (mono)
    if (resources.sounds[sound.first].buffer.loadFromSamples(buffer.data(), sound.second.samples, 1, sound.second.rate) == false)
      return false;

    // Attribute soundbuffer to sound
    resources.sounds[sound.first].sound.setBuffer(resources.sounds[sound.first].buffer);
  }

  return true;
}

bool	Game::Doom::buildLevel(std::pair<uint8_t, uint8_t> const & level)
{
  // Remove old level
  clearLevel();

  // Check if level exist in WAD file
  if (_wad.levels.find(level) == _wad.levels.end())
    return false;

  // Check for sky texture
  if (resources.textures.find(0x0000000000594B53 | (((int64_t)level.first + '0') << 24)) == resources.textures.end())
    return false;

  this->level.episode = level;
  this->level.sky = resources.textures[0x0000000000594B53 | (((int64_t)level.first + '0') << 24)];

  // Build every component of resources
  if (buildLevelVertexes(level) == false ||
    buildLevelSectors(level) == false ||
    buildLevelLinedefs(level) == false ||
    buildLevelSidedefs(level) == false ||
    buildLevelSubsectors(level) == false ||
    buildLevelThings(level) == false ||
    buildLevelSegments(level) == false ||
    buildLevelNodes(level) == false)
  {
    clearLevel();
    return false;
  }

  // Update components (might be useful for initializations)
  this->level.update(sf::seconds(0.f));

  return true;
}

bool	Game::Doom::buildLevelVertexes(std::pair<uint8_t, uint8_t> const & level)
{
  // Load level's vertexes from WAD
  for (Game::Wad::RawLevel::Vertex const & vertex : _wad.levels[level].vertexes)
    this->level.vertexes.push_back(Game::Doom::Level::Vertex((float)vertex.x, (float)vertex.y));

  return true;
}

bool	Game::Doom::buildLevelSectors(std::pair<uint8_t, uint8_t> const & level)
{
  // Load level's sectors from WAD
  for (Game::Wad::RawLevel::Sector const & sector : _wad.levels[level].sectors)
  {
    // Check for errors
    if (sector.floor_height > sector.ceiling_height ||
      (sector.floor_texture != Game::str_to_key("F_SKY1") && resources.flats.find(sector.floor_texture) == resources.flats.end()) ||
      (sector.ceiling_texture != Game::str_to_key("F_SKY1") && resources.flats.find(sector.ceiling_texture) == resources.flats.end()) ||
      sector.light < 0 || sector.light > 255)
    {
      std::cerr << "[Doom::build] Invalid sector." << std::endl;
      return false;
    }

    // Convert data structure
    this->level.sectors.push_back(Game::Doom::Level::Sector{
      (float)sector.floor_height,
      (float)sector.ceiling_height,
      sector.floor_texture,
      sector.ceiling_texture,
      (sector.floor_texture == Game::str_to_key("F_SKY1")) ? nullptr : resources.flats[sector.floor_texture],
      (sector.ceiling_texture == Game::str_to_key("F_SKY1")) ? nullptr : resources.flats[sector.ceiling_texture],
      sector.light,
      sector.special,
      sector.tag
    });
  }
  return true;
}

bool	Game::Doom::buildLevelSubsectors(std::pair<uint8_t, uint8_t> const & level)
{
  // Load level's sectors from WAD
  for (Game::Wad::RawLevel::Subsector const & subsector : _wad.levels[level].subsectors)
  {
    // Check for errors
    if (subsector.index < 0 || subsector.count < 0 ||
      subsector.index + subsector.count > _wad.levels[level].segments.size())
    {
      std::cerr << "[Doom::build] Invalid subsector." << std::endl;
      return false;
    }

    // Convert data structure
    this->level.subsectors.push_back(Game::Doom::Level::Subsector{
      subsector.count,
      subsector.index,

      _wad.levels[level].sidedefs[
	_wad.levels[level].segments[subsector.index].direction == 0 ?
	  _wad.levels[level].linedefs[_wad.levels[level].segments[subsector.index].linedef].front :
	  _wad.levels[level].linedefs[_wad.levels[level].segments[subsector.index].linedef].back
      ].sector
    });
  }

  return true;
}

bool	Game::Doom::buildLevelLinedefs(std::pair<uint8_t, uint8_t> const & level)
{
  // Load level's linedefs from WAD
  for (Game::Wad::RawLevel::Linedef const & linedef : _wad.levels[level].linedefs)
  {
    // Check for errors
    if (linedef.start < 0 || linedef.start >= _wad.levels[level].vertexes.size() ||
      linedef.end < 0 || linedef.end >= _wad.levels[level].vertexes.size() ||
      linedef.front < 0 || linedef.front >= _wad.levels[level].sidedefs.size() ||
      linedef.back != -1 && (linedef.back < 0 || linedef.back >= _wad.levels[level].sidedefs.size()))
    {
      std::cerr << "[Doom::build] Invalid linedef." << std::endl;
      return false;
    }

    // Convert data structure
    this->level.linedefs.push_back(Game::Doom::Level::Linedef{
      linedef.start,
      linedef.end,
      linedef.flag,
      linedef.type,
      linedef.tag,
      linedef.front,
      linedef.back
    });
  }

  return true;
}

bool	Game::Doom::buildLevelSidedefs(std::pair<uint8_t, uint8_t> const & level)
{
  // Load level's sidedefs from WAD
  try
  {
    for (Game::Wad::RawLevel::Sidedef const & sidedef : _wad.levels[level].sidedefs)
      this->level.sidedefs.push_back(Game::Doom::Level::Sidedef(*this, sidedef));
  }
  catch (std::exception e)
  {
    std::cerr << "[Doom::build] Invalid sidedef (" << std::string(e.what()) << ")." << std::endl;
    return false;
  }

  return true;
}

bool	Game::Doom::buildLevelSegments(std::pair<uint8_t, uint8_t> const & level)
{
  // Load level's segments from WAD
  for (Game::Wad::RawLevel::Segment const & segment : _wad.levels[level].segments)
  {
    // Check for errors
    if (segment.start < 0 || segment.start >= _wad.levels[level].segments.size() ||
      segment.end < 0 || segment.end >= _wad.levels[level].segments.size() ||
      segment.linedef < 0 || segment.linedef >= _wad.levels[level].linedefs.size() ||
      (segment.direction != 0 && segment.direction != 1))
    {
      std::cerr << "[Doom::build] Invalid segment." << std::endl;
      return false;
    }

    // Convert data structure
    this->level.segments.push_back(Game::Doom::Level::Segment{
      segment.start,
      segment.end,
      segment.angle / 32768.f * Math::Pi,
      segment.linedef,
      segment.direction,
      segment.offset
    });
  }

  return true;
}

bool	Game::Doom::buildLevelNodes(std::pair<uint8_t, uint8_t> const & level)
{
  // Load level's nodes from WAD
  for (Game::Wad::RawLevel::Node const & node : _wad.levels[level].nodes)
  {
    // Check for errors
    if ((node.direction_x == 0 && node.direction_y == 0) ||
      node.right_bb.top < node.right_bb.bottom || node.right_bb.right < node.right_bb.left ||
      node.left_bb.top < node.left_bb.bottom || node.left_bb.right < node.left_bb.left ||
      ((node.right_ss & 0b1000000000000000) ? ((node.right_ss & 0b0111111111111111) >= _wad.levels[level].subsectors.size()) : (node.right_ss >= _wad.levels[level].nodes.size())) ||
      ((node.left_ss & 0b1000000000000000) ? ((node.left_ss & 0b0111111111111111) >= _wad.levels[level].subsectors.size()) : (node.left_ss >= _wad.levels[level].nodes.size())))
    {
      std::cerr << "[Doom::build] Invalid node." << std::endl;
      return false;
    }

    // Convert data structure
    this->level.nodes.push_back(Game::Doom::Level::Node{
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

  return true;
}

bool	Game::Doom::buildLevelThings(std::pair<uint8_t, uint8_t> const & level)
{
  // Load level's things from WAD
  for (Game::Wad::RawLevel::Thing const & thing : _wad.levels[level].things)
  {
    // Convert thing from WAD
    Game::AbstractThing *	converted = Game::AbstractThing::factory(*this, thing);

    // Check for error
    if (converted == nullptr)
      std::cerr << "[Doom::build]: Warning, unknown thing type '" << thing.type << "'." << std::endl;
    else
      this->level.things.push_back(converted);
  }

  return true;
}

std::pair<int16_t, int16_t>	Game::Doom::Level::sector(Math::Vector<2> const & position, int16_t index) const
{
  // Start to search sector from top node
  if (index == -1)
    return sector(position, (int16_t)nodes.size() - 1);

  // Draw subsector if node ID has subsector mask
  if (index & 0b1000000000000000)
    return { subsectors[index & 0b0111111111111111].sector, index & 0b0111111111111111 };

  Game::Doom::Level::Node const &	node(nodes[index]);
  
  // Use derterminant to find on which side the position is
  if (Math::Vector<2>::determinant(node.direction, position - node.origin) > 0.f)
    return sector(position, node.leftchild);
  else
    return sector(position, node.rightchild);
}

void	Game::Doom::Resources::update(sf::Time elapsed)
{
  // Update resources flats
  for (std::pair<uint64_t, Game::AbstractFlat *> const & flat : flats)
    flat.second->update(elapsed);
}

void	Game::Doom::Level::update(sf::Time elapsed)
{
  // Update level things
  for (Game::AbstractThing * thing : things)
    thing->update(elapsed);

  // Update level sidedef
  for (Game::Doom::Level::Sidedef & sidedef : sidedefs)
    sidedef.update(elapsed);
}

Game::Doom::Level::Sidedef::Sidedef(const Game::Doom & doom, const Game::Wad::RawLevel::Sidedef & sidedef) :
  x(sidedef.x), y(sidedef.y), sector(sidedef.sector), _elapsed(),
  _upper(animation(doom, sidedef.upper)),
  _lower(animation(doom, sidedef.lower)),
  _middle(animation(doom, sidedef.middle))
{
  // Check for errors
  if (sidedef.sector < 0 || sidedef.sector >= doom.level.sectors.size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

Game::Doom::Level::Sidedef::~Sidedef()
{}

std::vector<const Game::Doom::Resources::Texture *>	Game::Doom::Level::Sidedef::animation(const Game::Doom & doom, uint64_t name) const
{
  // Check for null texture
  if (Game::key_to_str(name) == "-")
    return { &Game::Doom::Resources::NullTexture };

  // List of registered animations
  const static std::vector<std::vector<uint64_t>>	animations = {
    { Game::str_to_key("BLODGR1"), Game::str_to_key("BLODGR2"), Game::str_to_key("BLODGR3"), Game::str_to_key("BLODGR4") },
    { Game::str_to_key("BLODRIP1"), Game::str_to_key("BLODRIP2"), Game::str_to_key("BLODRIP3"), Game::str_to_key("BLODRIP4") },
    { Game::str_to_key("FIREBLU1"), Game::str_to_key("FIREBLU2") },
    { Game::str_to_key("FIRLAV3"), Game::str_to_key("FIRLAVA") },
    { Game::str_to_key("FIREMAG1"), Game::str_to_key("FIREMAG2"), Game::str_to_key("FIREMAG3") },
    { Game::str_to_key("FIREWALA"), Game::str_to_key("FIREWALB"), Game::str_to_key("FIREWALL") },
    { Game::str_to_key("GSTFONT1"), Game::str_to_key("GSTFONT2"), Game::str_to_key("GSTFONT3") },
    { Game::str_to_key("ROCKRED1"), Game::str_to_key("ROCKRED2"), Game::str_to_key("ROCKRED3") },
    { Game::str_to_key("SLADRIP1"), Game::str_to_key("SLADRIP2"), Game::str_to_key("SLADRIP3") },
    { Game::str_to_key("BFALL1"), Game::str_to_key("BFALL2"), Game::str_to_key("BFALL3"), Game::str_to_key("BFALL4") },
    { Game::str_to_key("SFALL1"), Game::str_to_key("SFALL2"), Game::str_to_key("SFALL3"), Game::str_to_key("SFALL4") },
    { Game::str_to_key("WFALL1"), Game::str_to_key("WFALL2"), Game::str_to_key("WFALL3"), Game::str_to_key("WFALL4") },
    { Game::str_to_key("DBRAIN1"), Game::str_to_key("DBRAIN2"), Game::str_to_key("DBRAIN3"), Game::str_to_key("DBRAIN4") }
  };

  std::vector<uint64_t>	animation = { name };

  // Find if frame is part of an animation
  for (const std::vector<uint64_t> & frames : animations)
    for (uint64_t frame : frames)
      if (name == frame)
	animation = frames;

  std::vector<const Game::Doom::Resources::Texture *>	result;

  // Check if frames are registered in resources
  for (uint64_t frame : animation) {
    if (doom.resources.textures.find(frame) == doom.resources.textures.end()) {
      std::cout << "'" << Game::key_to_str(frame) << "'" << std::endl;
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }
    result.push_back(&doom.resources.textures.find(frame)->second);
  }
  
  // Return animation
  return result;
}

void	Game::Doom::Level::Sidedef::update(sf::Time elapsed)
{
  // Add elapsed time to total
  _elapsed += elapsed;
}

const Game::Doom::Resources::Texture &	Game::Doom::Level::Sidedef::upper() const
{
  // Return upper frame
  return *_upper[_elapsed.asMicroseconds() / (Game::Doom::Tic.asMicroseconds() * Game::Doom::Level::Sidedef::FrameDuration) % _upper.size()];
}

const Game::Doom::Resources::Texture &	Game::Doom::Level::Sidedef::lower() const
{
  // Return lower frame
  return *_lower[_elapsed.asMicroseconds() / (Game::Doom::Tic.asMicroseconds() * Game::Doom::Level::Sidedef::FrameDuration) % _lower.size()];
}

const Game::Doom::Resources::Texture &	Game::Doom::Level::Sidedef::middle() const
{
  // Return middle frame
  return *_middle[_elapsed.asMicroseconds() / (Game::Doom::Tic.asMicroseconds() * Game::Doom::Level::Sidedef::FrameDuration) % _middle.size()];
}
