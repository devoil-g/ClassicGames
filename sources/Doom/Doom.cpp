#include "Doom/Doom.hpp"
#include "Doom/Action/BlinkLightingAction.hpp"
#include "Doom/Action/DoorLevelingAction.hpp"
#include "Doom/Action/FlickerLightingAction.hpp"
#include "Doom/Action/OscillateLightingAction.hpp"
#include "Doom/Action/RandomLightingAction.hpp"
#include "Doom/Thing/PlayerThing.hpp"

const sf::Time		DOOM::Doom::Tic = sf::seconds(1.f / 35.f);
const unsigned int	DOOM::Doom::RenderWidth = 320;
const unsigned int	DOOM::Doom::RenderHeight = 200;
const float		DOOM::Doom::RenderStretching = 6.f / 5.f;

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
  for (const std::pair<std::pair<uint8_t, uint8_t>, DOOM::Wad::RawLevel> & level : wad.levels)
    list.emplace_back(level.first);

  return list;
}

void	DOOM::Doom::setLevel(std::pair<uint8_t, uint8_t> const & level)
{
  // Build level
  buildLevel(level);
}

void	DOOM::Doom::addPlayer(int controller)
{
  // Cancel if invalid controller id
  if (controller < 0)
    return;

  // Cancel if controller already registered
  for (const std::reference_wrapper<DOOM::PlayerThing> & player : level.players)
    if (player.get().controller == controller)
      return;
  
  // Push new player
  level.things.push_back(std::make_unique<DOOM::PlayerThing>(*this, (int)level.players.size() + 1, controller));
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

  // Force deletion of players
  level.players.clear();
  level.things.clear();

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

  // Reset level things (preserve players)
  level.things.erase(std::remove_if(level.things.begin(), level.things.end(),
    [](const std::unique_ptr<DOOM::AbstractThing> & thing)
    {
      return thing->type != -1;
    }
  ), level.things.end());

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
  for (const std::pair<uint64_t, DOOM::Wad::RawResources::Texture> & texture : wad.resources.textures)
    resources.textures.emplace(std::piecewise_construct, std::forward_as_tuple(texture.first), std::forward_as_tuple(*this, texture.second));
}

void	DOOM::Doom::buildResourcesSprites()
{
  // Load sprites textures from WAD resources
  for (const std::pair<uint64_t, DOOM::Wad::RawResources::Patch> & patch : wad.resources.sprites) {
    std::string	name = DOOM::key_to_str(patch.first);

    // Build sprite and get its reference
    std::reference_wrapper<const DOOM::Doom::Resources::Texture>	sprite = std::ref(resources.sprites.emplace(std::piecewise_construct, std::forward_as_tuple(patch.first), std::forward_as_tuple(*this, patch.second)).first->second);

    if (name.length() >= 6)
    {
      // Resize animation sequence
      if (resources.animations[DOOM::str_to_key(name.substr(0, 4))].size() < name.at(4) - 'A' + 1)
	resources.animations[DOOM::str_to_key(name.substr(0, 4))].resize(name.at(4) - 'A' + 1, { {
	  { std::ref(DOOM::Doom::Resources::Texture::Null), false },
	  { std::ref(DOOM::Doom::Resources::Texture::Null), false },
	  { std::ref(DOOM::Doom::Resources::Texture::Null), false },
	  { std::ref(DOOM::Doom::Resources::Texture::Null), false },
	  { std::ref(DOOM::Doom::Resources::Texture::Null), false },
	  { std::ref(DOOM::Doom::Resources::Texture::Null), false },
	  { std::ref(DOOM::Doom::Resources::Texture::Null), false },
	  { std::ref(DOOM::Doom::Resources::Texture::Null), false }
	  } });

      // Push sprite in animation sequence
      if (name.at(5) == '0')
	for (std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool> & frame : resources.animations[DOOM::str_to_key(name.substr(0, 4))][name.at(4) - 'A'])
	  frame = { sprite, false };
      else
	resources.animations[DOOM::str_to_key(name.substr(0, 4))][name.at(4) - 'A'][name.at(5) - '1'] = { sprite, false };
    }

    if (name.length() >= 8)
    {
      // Resize animation sequence
      if (resources.animations[DOOM::str_to_key(name.substr(0, 4))].size() < name.at(6) - 'A' + 1)
	resources.animations[DOOM::str_to_key(name.substr(0, 4))].resize(name.at(6) - 'A' + 1, { {
	  { std::ref(DOOM::Doom::Resources::Texture::Null), false },
	  { std::ref(DOOM::Doom::Resources::Texture::Null), false },
	  { std::ref(DOOM::Doom::Resources::Texture::Null), false },
	  { std::ref(DOOM::Doom::Resources::Texture::Null), false },
	  { std::ref(DOOM::Doom::Resources::Texture::Null), false },
	  { std::ref(DOOM::Doom::Resources::Texture::Null), false },
	  { std::ref(DOOM::Doom::Resources::Texture::Null), false },
	  { std::ref(DOOM::Doom::Resources::Texture::Null), false }
	  } });

      // Push sprite in animation sequence
      if (name.at(7) == '0')
	for (std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool> & frame : resources.animations[DOOM::str_to_key(name.substr(0, 4))][name.at(6) - 'A'])
	  frame = { sprite, true };
      else
	resources.animations[DOOM::str_to_key(name.substr(0, 4))][name.at(6) - 'A'][name.at(7) - '1'] = { sprite, true };
    }
  }
}

void	DOOM::Doom::buildResourcesMenus()
{
  // Load menus textures from WAD resources
  for (const std::pair<uint64_t, DOOM::Wad::RawResources::Patch> & menu : wad.resources.menus)
    resources.menus.emplace(std::piecewise_construct, std::forward_as_tuple(menu.first), std::forward_as_tuple(*this, menu.second));
}

void	DOOM::Doom::buildResourcesFlats()
{
  // Load flats from WAD resources
  for (const std::pair<uint64_t, DOOM::Wad::RawResources::Flat> & flat : wad.resources.flats)
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
  this->level.sky = std::cref(resources.textures.find(0x0000000000594B53 | (((int64_t)level.first + '0') << 24))->second);

  // Build every component of resources
  try
  {
    buildLevelVertexes();
    buildLevelSectors();
    buildLevelLinedefs();
    buildLevelSidedefs();
    buildLevelSubsectors();
    buildLevelSegments();
    buildLevelNodes();
    buildLevelBlockmap();
    buildLevelThings();
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

  // Set player in blockmap
  for (std::reference_wrapper<DOOM::PlayerThing> & player : level.players)
    level.blockmap.addThing(player.get(), player.get().position.convert<2>());
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
  // Update level linedef
  for (const std::unique_ptr<DOOM::AbstractLinedef> & linedef : linedefs)
    linedef->update(doom, elapsed);

  // Update level sectors
  for (DOOM::Doom::Level::Sector & sector : sectors)
    sector.update(doom, elapsed);

  // Update level sidedef
  for (DOOM::Doom::Level::Sidedef & sidedef : sidedefs)
    sidedef.update(doom, elapsed);

  // Update level things
  for (std::list<std::unique_ptr<DOOM::AbstractThing>>::iterator iterator = things.begin(); iterator != things.end(); )
    // Remove thing if update return true
    if (iterator->get()->update(doom, elapsed) == true) {
      blockmap.removeThing(*iterator->get(), iterator->get()->position.convert<2>());
      iterator = things.erase(iterator);
    }
    else {
      iterator++;
    }
}

std::set<int16_t>	DOOM::Doom::Level::getSector(const DOOM::AbstractThing & thing) const
{
  std::set<int16_t>	result;

  // Get sector at thing central position
  result.insert(getSector(thing.position.convert<2>()).first);

  // Return nothing if thing is outside of level
  if (*result.cbegin() == -1)
    return {};

  // Get near linedef using blockmap
  std::set<int16_t>	blocks;
  std::set<int16_t>	linedefs;

  // Get blocks thing stand in
  blocks.insert(blockmap.index(Math::Vector<2>(thing.position.x() - (float)thing.radius, thing.position.y() - (float)thing.radius)));
  blocks.insert(blockmap.index(Math::Vector<2>(thing.position.x() - (float)thing.radius, thing.position.y() + (float)thing.radius)));
  blocks.insert(blockmap.index(Math::Vector<2>(thing.position.x() + (float)thing.radius, thing.position.y() - (float)thing.radius)));
  blocks.insert(blockmap.index(Math::Vector<2>(thing.position.x() + (float)thing.radius, thing.position.y() + (float)thing.radius)));

  // Get index of linedefs to test againt position
  for (int16_t block_index : blocks)
    if (block_index != -1) {
      const DOOM::Doom::Level::Blockmap::Block &	block = blockmap.blocks[block_index];

      linedefs.insert(block.linedefs.begin(), block.linedefs.end());
    }

  // Check for intersection with each linedef
  for (int16_t linedef_index : linedefs) {
    const DOOM::AbstractLinedef &	linedef = *this->linedefs[linedef_index].get();
    const DOOM::Doom::Level::Vertex &	linedef_start = vertexes[linedef.start];
    const DOOM::Doom::Level::Vertex &	linedef_end = vertexes[linedef.end];

    // Get closest point to thing along linedef
    float	s = std::clamp(-((linedef_start.x() - thing.position.x()) * (linedef_end.x() - linedef_start.x()) + (linedef_start.y() - thing.position.y()) * (linedef_end.y() - linedef_start.y())) / (std::pow(linedef_end.x() - linedef_start.x(), 2) + std::pow(linedef_end.y() - linedef_start.y(), 2)), 0.f, 1.f);

    // Add linedef sectors to result if intersecting with thing bounds
    if ((linedef_start + (linedef_end - linedef_start) * s - thing.position.convert<2>()).length() < thing.radius - 1.f) {
      result.insert(sidedefs[linedef.front].sector);
      if (linedef.back != -1)
	result.insert(sidedefs[linedef.back].sector);
    }      
  }

  return result;
}

std::pair<int16_t, int16_t>	DOOM::Doom::Level::getSector(Math::Vector<2> const & position, int16_t index) const
{
  // Start to search sector from top node
  if (index == -1)
    return getSector(position, (int16_t)nodes.size() - 1);

  // Return subsector's sector if node ID has subsector mask
  if (index & 0b1000000000000000)
    return { subsectors[index & 0b0111111111111111].sector, index & 0b0111111111111111 };

  DOOM::Doom::Level::Node const &	node(nodes[index]);

  // Use derterminant to find on which side the position is
  if (Math::Vector<2>::determinant(node.direction, position - node.origin) > 0.f)
    return getSector(position, node.leftchild);
  else
    return getSector(position, node.rightchild);
}

std::list<int16_t>	DOOM::Doom::Level::getLinedefs(const Math::Vector<2> & position, const Math::Vector<2> & direction, float limit) const
{
  std::list<int16_t>	result;

  // Start to search subsector from top node
  getLinedefsNode(result, position, direction, limit, (int16_t)nodes.size() - 1);

  return result;
}

bool	DOOM::Doom::Level::getLinedefsNode(std::list<int16_t> & result, const Math::Vector<2> & position, const Math::Vector<2> & direction, float limit, int16_t index) const
{
  // Draw subsector if node ID has subsector mask
  if (index & 0b1000000000000000)
    return getLinedefsSubsector(result, position, direction, limit, index & 0b0111111111111111);

  DOOM::Doom::Level::Node const &	node(nodes[index]);

  // Use derterminant to find on which side the position is
  if (Math::Vector<2>::determinant(node.direction, position - node.origin) > 0.f)
    return getLinedefsNode(result, position, direction, limit, node.leftchild) == true ||
      (Math::Vector<2>::determinant(node.origin - position, node.direction) / Math::Vector<2>::determinant(direction, node.direction) >= 0.f && getLinedefsNode(result, position, direction, limit, node.rightchild) == true);
  else
    return getLinedefsNode(result, position, direction, limit, node.rightchild) == true ||
      (Math::Vector<2>::determinant(node.origin - position, node.direction) / Math::Vector<2>::determinant(direction, node.direction) >= 0.f && getLinedefsNode(result, position, direction, limit, node.leftchild) == true);
}


bool	DOOM::Doom::Level::getLinedefsSubsector(std::list<int16_t> & result, const Math::Vector<2> & position, const Math::Vector<2> & direction, float limit, int16_t index) const
{
  DOOM::Doom::Level::Subsector const &	subsector(subsectors[index]);
  float					distance = -1.f;

  // Render subsector segs
  for (int16_t i = 0; i < subsector.count; i++)
    distance = std::max(distance, getLinedefsSeg(result, position, direction, limit, subsector.index + i));

  // Return true if limit has been reached
  return distance > limit;
}

float	DOOM::Doom::Level::getLinedefsSeg(std::list<int16_t> & result, const Math::Vector<2> & position, const Math::Vector<2> & direction, float limit, int16_t index) const
{
  // Get segment from level data
  const DOOM::Doom::Level::Segment &	seg(segments[index]);
  const DOOM::Doom::Level::Vertex &	seg_start(vertexes[seg.start]);
  const DOOM::Doom::Level::Vertex &	seg_end(vertexes[seg.end]);

  // Compute intersection of ray with segment
  std::pair<float, float>	intersection(Math::intersection(position, direction, seg_start, seg_end - seg_start));

  // Check if segment is intersected in acceptable bounds
  if (std::isnan(intersection.first) == true || std::isnan(intersection.second) == true || intersection.first < 0.f || intersection.second < 0.f || intersection.second > 1.f)
    return -1.f;

  // Limit reached
  if (intersection.first > limit)
    return intersection.first;
  
  // Cancel if wrong side
  if ((seg.direction == 0 && Math::Vector<2>::determinant(direction, seg_end - seg_start) > 0.f) ||
    (seg.direction == 1 && (linedefs[seg.linedef]->back != -1 || Math::Vector<2>::determinant(direction, seg_end - seg_start) < 0.f)))
    return -1.f;
  
  // Push linedef index as result
  result.push_back(seg.linedef);

  // Return segment distance
  return intersection.first;
}

std::list<std::reference_wrapper<DOOM::AbstractThing>>	DOOM::Doom::Level::getThings(const DOOM::Doom::Level::Sector & sector, int16_t properties) const
{
  std::set<DOOM::AbstractThing *>	things;

  // Iterate blocks of sectors
  for (int block_index : blockmap.sectors.find(&sector)->second) {
    const DOOM::Doom::Level::Blockmap::Block &	block = blockmap.blocks[block_index];

    // Check if things have correct properties before testing gaint linedefs
    for (const std::reference_wrapper<DOOM::AbstractThing> & thing : block.things) {
      if ((thing.get().properties & properties) == properties && things.find(&(thing.get())) == things.end()) {
	for (int16_t linedef_index : blockmap.blocks[block_index].linedefs) {
	  const DOOM::AbstractLinedef &	linedef = *linedefs[linedef_index].get();

	  // Only test thing against linedef of sector
	  if (&sectors[sidedefs[linedef.front].sector] == &sector || (linedef.back != -1 && &sectors[sidedefs[linedef.back].sector] == &sector)) {
	    const DOOM::Doom::Level::Vertex &	linedef_start = vertexes[linedef.start];
	    const DOOM::Doom::Level::Vertex &	linedef_end = vertexes[linedef.end];

	    // Get closest point to thing along linedef
	    float	s = std::clamp(-((linedef_start.x() - thing.get().position.x()) * (linedef_end.x() - linedef_start.x()) + (linedef_start.y() - thing.get().position.y()) * (linedef_end.y() - linedef_start.y())) / (std::pow(linedef_end.x() - linedef_start.x(), 2) + std::pow(linedef_end.y() - linedef_start.y(), 2)), 0.f, 1.f);

	    // Add linedef sectors to result if intersecting with thing bounds
	    if ((linedef_start + (linedef_end - linedef_start) * s - thing.get().position.convert<2>()).length() < thing.get().radius - 1.f)
	      things.insert(&(thing.get()));
	  }
	}
      }
    }
  }

  std::list<std::reference_wrapper<DOOM::AbstractThing>>	result;

  // Convert things set to reference list
  for (DOOM::AbstractThing * thing : things)
    result.push_back(*thing);

  return result;
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
  x((float)sidedef.x), y((float)sidedef.y), sector(sidedef.sector), _elapsed(),
  _upper_name(sidedef.upper),
  _lower_name(sidedef.lower),
  _middle_name(sidedef.middle),
  _upper_textures(animation(doom, sidedef.upper)),
  _lower_textures(animation(doom, sidedef.lower)),
  _middle_textures(animation(doom, sidedef.middle))
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
  const static std::vector<std::vector<uint64_t>>	animations =
  {
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

    result.push_back(doom.resources.textures.find(frame)->second);
  }

  // Return animation
  return result;
}

void	DOOM::Doom::Level::Sidedef::update(DOOM::Doom & doom, sf::Time elapsed)
{
  // Add elapsed time to total
  _elapsed += elapsed;

  // Re-toggle switch
  if (_toggle > sf::Time::Zero) {
    _toggle -= elapsed;

    // Switch texture if toggle time elapsed
    if (_toggle <= sf::Time::Zero)
      switched(doom, sf::Time::Zero);
  }
}

bool	DOOM::Doom::Level::Sidedef::switched(DOOM::Doom & doom, sf::Time toggle)
{
  // Does nothing if a toggle is already running
  if (_toggle > sf::Time::Zero)
    return false;

  // Set toggle timer
  _toggle = toggle;

  // Call animation method to find texture set of switch
  return switched(doom, _upper_textures, _upper_name) |
    switched(doom, _lower_textures, _lower_name) |
    switched(doom, _middle_textures, _middle_name);
}

bool	DOOM::Doom::Level::Sidedef::switched(DOOM::Doom & doom, std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>> & textures, uint64_t & name)
{
  // NOTE: the switched is performed using name convention method,
  // switches should be hard coded

  std::string	name_str = DOOM::key_to_str(name);

  // Check if texture name is switchable
  if (name_str.substr(0, 3) == "SW1")
    name_str.at(2) = '2';
  else if (name_str.substr(0, 3) == "SW2")
    name_str.at(2) = '1';
  else
    return false;

  // Find new texture set
  name = DOOM::str_to_key(name_str);
  textures = animation(doom, name);

  return true;
}

const DOOM::Doom::Resources::Texture &	DOOM::Doom::Level::Sidedef::upper() const
{
  // Return upper frame
  return _upper_textures[_elapsed.asMicroseconds() / (DOOM::Doom::Tic.asMicroseconds() * DOOM::Doom::Level::Sidedef::FrameDuration) % _upper_textures.size()];
}

const DOOM::Doom::Resources::Texture &	DOOM::Doom::Level::Sidedef::lower() const
{
  // Return lower frame
  return _lower_textures[_elapsed.asMicroseconds() / (DOOM::Doom::Tic.asMicroseconds() * DOOM::Doom::Level::Sidedef::FrameDuration) % _lower_textures.size()];
}

const DOOM::Doom::Resources::Texture &	DOOM::Doom::Level::Sidedef::middle() const
{
  // Return middle frame
  return _middle_textures[_elapsed.asMicroseconds() / (DOOM::Doom::Tic.asMicroseconds() * DOOM::Doom::Level::Sidedef::FrameDuration) % _middle_textures.size()];
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
  row(blockmap.row),
  blocks()
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

  // Build sectors to block indexes map
  for (int16_t sector_index = 0; sector_index < doom.level.sectors.size(); sector_index++)
    for (int16_t block_index = 0; block_index < blocks.size(); block_index++)
      for (int16_t linedef_index : blocks[block_index].linedefs)
	if (doom.level.sidedefs[doom.level.linedefs[linedef_index].get()->front].sector == sector_index || (doom.level.linedefs[linedef_index].get()->back != -1 && doom.level.sidedefs[doom.level.linedefs[linedef_index].get()->back].sector == sector_index))
	  sectors[&doom.level.sectors[sector_index]].insert(block_index);
}

int	DOOM::Doom::Level::Blockmap::index(const Math::Vector<2> & position) const
{
  // Check if position out of bound
  if (position.x() < x || position.x() >= x + 128 * column ||
    position.y() < y || position.y() >= y + 128 * row)
    return -1;

  // Compute position index in blockmap
  return ((int)position.y() - y) / 128 * column + ((int)position.x() - x) / 128;
}

void	DOOM::Doom::Level::Blockmap::addThing(DOOM::AbstractThing & thing, const Math::Vector<2> & position)
{
  std::set<int>	thing_blocks;

  // Get blocks of thing
  thing_blocks.insert(index(Math::Vector<2>(position.x() - thing.radius, position.y() - thing.radius)));
  thing_blocks.insert(index(Math::Vector<2>(position.x() - thing.radius, position.y() + thing.radius)));
  thing_blocks.insert(index(Math::Vector<2>(position.x() + thing.radius, position.y() - thing.radius)));
  thing_blocks.insert(index(Math::Vector<2>(position.x() + thing.radius, position.y() + thing.radius)));

  // Insert thing in blocks
  for (int index : thing_blocks)
    if (index != -1)
      blocks[index].things.insert(std::ref(thing));
}

void	DOOM::Doom::Level::Blockmap::moveThing(DOOM::AbstractThing & thing, const Math::Vector<2> & old_position, const Math::Vector<2> & new_position)
{
  // Remove and insert thing of blockmap
  removeThing(thing, old_position);
  addThing(thing, new_position);
}

void	DOOM::Doom::Level::Blockmap::removeThing(DOOM::AbstractThing & thing, const Math::Vector<2> & position)
{
  std::set<int>	thing_blocks;

  // Get blocks of thing
  thing_blocks.insert(index(Math::Vector<2>(position.x() - thing.radius, position.y() - thing.radius)));
  thing_blocks.insert(index(Math::Vector<2>(position.x() - thing.radius, position.y() + thing.radius)));
  thing_blocks.insert(index(Math::Vector<2>(position.x() + thing.radius, position.y() - thing.radius)));
  thing_blocks.insert(index(Math::Vector<2>(position.x() + thing.radius, position.y() + thing.radius)));

  // Remove thing from blocks
  for (int index : thing_blocks)
    if (index != -1)
      blocks[index].things.erase(std::ref(thing));
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
    action<DOOM::Doom::Level::Sector::Action::Lighting>(std::make_unique<DOOM::RandomLightingAction<24, 4>>(doom, *this));
    break;
  case DOOM::Doom::Level::Sector::Special::LightBlink05:
    action<DOOM::Doom::Level::Sector::Action::Lighting>(std::make_unique<DOOM::BlinkLightingAction<15, 5, false>>(doom, *this));
    break;
  case DOOM::Doom::Level::Sector::Special::LightBlink10:
    action<DOOM::Doom::Level::Sector::Action::Lighting>(std::make_unique<DOOM::BlinkLightingAction<35, 5, false>>(doom, *this));
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
    action<DOOM::Doom::Level::Sector::Action::Lighting>(std::make_unique<DOOM::OscillateLightingAction<>>(doom, *this));
    break;
  case DOOM::Doom::Level::Sector::Special::Secret:
    break;	// TODO
  case DOOM::Doom::Level::Sector::Special::DoorClose:
    action<DOOM::Doom::Level::Sector::Action::Leveling>(std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Door::DoorWaitClose, DOOM::EnumAction::Speed::SpeedSlow, 1050>>(doom, *this));
    break;
  case DOOM::Doom::Level::Sector::Special::End:
    break;	// TODO
  case DOOM::Doom::Level::Sector::Special::LightBlink05Sync:
    action<DOOM::Doom::Level::Sector::Action::Lighting>(std::make_unique<DOOM::BlinkLightingAction<15, 5, true>>(doom, *this));
    break;
  case DOOM::Doom::Level::Sector::Special::LightBlink10Sync:
    action<DOOM::Doom::Level::Sector::Action::Lighting>(std::make_unique<DOOM::BlinkLightingAction<35, 5, true>>(doom, *this));
    break;
  case DOOM::Doom::Level::Sector::Special::DoorOpen:
    action<DOOM::Doom::Level::Sector::Action::Leveling>(std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Door::DoorWaitOpen, DOOM::EnumAction::Speed::SpeedSlow, 10500>>(doom, *this));
    break;
  case DOOM::Doom::Level::Sector::Special::Damage20:
    damage = 20.f;
    break;
  case DOOM::Doom::Level::Sector::Special::LightFlickers:
    action<DOOM::Doom::Level::Sector::Action::Lighting>(std::make_unique<DOOM::FlickerLightingAction<>>(doom, *this));
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

std::unique_ptr<DOOM::AbstractAction>	DOOM::Doom::Level::Sector::_factory(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, int16_t type, int16_t model)
{
  // Just a relay (cycling inclusion problem)
  return DOOM::AbstractAction::factory(doom, sector, type, model);
}

void	DOOM::Doom::Level::Sector::update(DOOM::Doom & doom, sf::Time elapsed)
{
  // Update sector actions
  for (unsigned int type = 0; type < DOOM::Doom::Level::Sector::Action::Number; type++)
    if (_actions.at(type).get() != nullptr)
      _actions.at(type)->update(doom, *this, elapsed);
}

std::pair<int16_t, float>	DOOM::Doom::Level::Sector::getNeighborLowestFloor(const DOOM::Doom & doom) const
{
  std::pair<int16_t, float>	result = { -1, std::numeric_limits<float>::quiet_NaN() };

  // Find lowest neighboor floor
  for (int16_t index : _neighbors)
    if (result.first == -1 || doom.level.sectors[index].floor_base < doom.level.sectors[result.first].floor_base)
      result = { index, doom.level.sectors[index].floor_base };

  return result;
}

std::pair<int16_t, float>	DOOM::Doom::Level::Sector::getNeighborHighestFloor(const DOOM::Doom & doom) const
{
  std::pair<int16_t, float>	result = { -1, std::numeric_limits<float>::quiet_NaN() };

  // Find highest neighboor floor
  for (int16_t index : _neighbors)
    if (result.first == -1 || doom.level.sectors[index].floor_base > doom.level.sectors[result.first].floor_base)
      result = { index, doom.level.sectors[index].floor_base };

  return result;
}

std::pair<int16_t, float>	DOOM::Doom::Level::Sector::getNeighborNextLowestFloor(const DOOM::Doom & doom, float height) const
{
  std::pair<int16_t, float>	result = { -1, std::numeric_limits<float>::quiet_NaN() };

  // Find next lowest neighboor floor
  for (int16_t index : _neighbors) {
    float	floor = doom.level.sectors[index].floor_base;

    if (floor < height && (result.first == -1 || floor > result.second))
      result = { index, floor };
  }

  return result;
}

std::pair<int16_t, float>	DOOM::Doom::Level::Sector::getNeighborNextHighestFloor(const DOOM::Doom & doom, float height) const
{
  std::pair<int16_t, float>	result = { -1, std::numeric_limits<float>::quiet_NaN() };

  // Find next highest neighboor floor
  for (int16_t index : _neighbors) {
    float	floor = doom.level.sectors[index].floor_base;

    if (floor > height && (result.first == -1 || floor < result.second))
      result = { index, floor };
  }

  return result;
}

std::pair<int16_t, float>	DOOM::Doom::Level::Sector::getNeighborLowestCeiling(const DOOM::Doom & doom) const
{
  std::pair<int16_t, float>	result = { -1, std::numeric_limits<float>::quiet_NaN() };

  // Find lowest neighboor ceiling
  for (int16_t index : _neighbors)
    if (result.first == -1 || doom.level.sectors[index].ceiling_base < doom.level.sectors[result.first].ceiling_base)
      result = { index, doom.level.sectors[index].ceiling_base };

  return result;
}

std::pair<int16_t, float>	DOOM::Doom::Level::Sector::getNeighborHighestCeiling(const DOOM::Doom & doom) const
{
  std::pair<int16_t, float>	result = { -1, std::numeric_limits<float>::quiet_NaN() };

  // Find lowest neighboor ceiling
  for (int16_t index : _neighbors)
    if (result.first == -1 || doom.level.sectors[index].ceiling_base > doom.level.sectors[result.first].ceiling_base)
      result = { index, doom.level.sectors[index].ceiling_base };

  return result;
}

std::pair<int16_t, float>	DOOM::Doom::Level::Sector::getNeighborNextLowestCeiling(const DOOM::Doom & doom, float height) const
{
  std::pair<int16_t, float>	result = { -1, std::numeric_limits<float>::quiet_NaN() };

  // Find next lowest neighboor ceiling
  for (int16_t index : _neighbors) {
    float	ceiling = doom.level.sectors[index].ceiling_base;

    if (ceiling < height && (result.first == -1 || ceiling > result.second))
      result = { index, ceiling };
  }

  return result;
}

std::pair<int16_t, float>	DOOM::Doom::Level::Sector::getNeighborNextHighestCeiling(const DOOM::Doom & doom, float height) const
{
  std::pair<int16_t, float>	result = { -1, std::numeric_limits<float>::quiet_NaN() };

  // Find next highest neighboor floor
  for (int16_t index : _neighbors) {
    float	ceiling = doom.level.sectors[index].ceiling_base;

    if (ceiling > height && (result.first == -1 || ceiling < result.second))
      result = { index, ceiling };
  }

  return result;
}

int16_t	DOOM::Doom::Level::Sector::getShortestLowerTexture(const DOOM::Doom & doom) const
{
  int16_t	result = 0;
  int16_t	index = (int16_t)(((uint64_t)this - (uint64_t)doom.level.sectors.data()) / sizeof(DOOM::Doom::Level::Sector));

  // Find shortest lower texture of sector
  for (const std::unique_ptr<DOOM::AbstractLinedef> & linedef : doom.level.linedefs)
    if (linedef->back != -1 && linedef->front != -1) {
      if (doom.level.sidedefs[linedef->back].sector == index)
	result = ((result == 0) ? doom.level.sidedefs[linedef->front].lower().height : std::min(doom.level.sidedefs[linedef->front].lower().height, result));
      else if (doom.level.sidedefs[linedef->front].sector == index)
	result = ((result == 0) ? doom.level.sidedefs[linedef->back].lower().height : std::min(doom.level.sidedefs[linedef->back].lower().height, result));
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

namespace std
{
  // Define comparison of Thing ref to build a std::set
  bool	operator<(const std::reference_wrapper<DOOM::AbstractThing> & left, const std::reference_wrapper<DOOM::AbstractThing> & right)
  {
    return &left.get() < &right.get();
  }
}