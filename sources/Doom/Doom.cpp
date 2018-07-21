#include <iostream>

#include "Doom/Doom.hpp"
#include "Doom/Flat/AnimatedFlat.hpp"
#include "Doom/Flat/StaticFlat.hpp"
#include "Doom/Linedef/ActionTriggerableLinedef.hpp"
#include "Doom/Linedef/LightTriggerableLinedef.hpp"
#include "Doom/Linedef/NullLinedef.hpp"
#include "Doom/Linedef/StairTriggerableLinedef.hpp"
#include "Doom/Sector/BlinkLightingAction.hpp"
#include "Doom/Sector/CeilingLevelingAction.hpp"
#include "Doom/Sector/DoorLevelingAction.hpp"
#include "Doom/Sector/FlickerLightingAction.hpp"
#include "Doom/Sector/FloorLevelingAction.hpp"
#include "Doom/Sector/OscillateLightingAction.hpp"
#include "Doom/Sector/RandomLightingAction.hpp"
#include "Doom/Thing/AnimatedThing.hpp"
#include "Doom/Thing/NullThing.hpp"

sf::Time const	DOOM::Doom::Tic = sf::seconds(1.f / 35.f);

DOOM::Doom::Resources::Texture const	DOOM::Doom::Resources::NullTexture = DOOM::Doom::Resources::Texture();

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
  level.sky = std::ref(DOOM::Doom::Resources::NullTexture);

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
    std::unique_ptr<DOOM::Doom::Resources::AbstractFlat>	converted = DOOM::Doom::Resources::AbstractFlat::factory(*this, flat.first, flat.second);

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
    std::unique_ptr<DOOM::Doom::Level::AbstractLinedef>	converted = DOOM::Doom::Level::AbstractLinedef::factory(*this, linedef);

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
    std::unique_ptr<DOOM::Doom::Level::AbstractThing>	converted = DOOM::Doom::Level::AbstractThing::factory(*this, thing);

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

void	DOOM::Doom::Resources::update(DOOM::Doom & doom, sf::Time elapsed)
{
  // Update resources flats
  for (const std::pair<const uint64_t, std::unique_ptr<DOOM::Doom::Resources::AbstractFlat>> & flat : flats)
    flat.second->update(doom, elapsed);
}

void	DOOM::Doom::Level::update(DOOM::Doom & doom, sf::Time elapsed)
{
  // Update level things
  for (const std::unique_ptr<DOOM::Doom::Level::AbstractThing> & thing : things)
    thing->update(doom, elapsed);

  // Update level sectors
  for (DOOM::Doom::Level::Sector & sector : sectors)
    sector.update(doom, elapsed);

  // Update level linedef
  for (const std::unique_ptr<DOOM::Doom::Level::AbstractLinedef> & linedef : linedefs)
    linedef->update(doom, elapsed);

  // Update level sidedef
  for (DOOM::Doom::Level::Sidedef & sidedef : sidedefs)
    sidedef.update(doom, elapsed);
}

DOOM::Doom::Level::Sector::AbstractAction::AbstractAction(DOOM::Doom & doom)
{}

std::unique_ptr<DOOM::Doom::Level::Sector::AbstractAction>	DOOM::Doom::Level::Sector::AbstractAction::factory(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, int16_t type)
{
  // Generate action from type
  switch (type) {

    // Regular and extended door types
  case 1: case 4: case 29: case 63: case 90:	// Open, wait then close (slow)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Speed::SpeedSlow, 140>>(doom, std::list<DOOM::EnumAction::State>({ DOOM::EnumAction::State::StateOpen, DOOM::EnumAction::State::StateWait, DOOM::EnumAction::State::StateClose }));
  case 105: case 108: case 111: case 114: case 117:	// Open, wait then close (fast)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Speed::SpeedFast, 140>>(doom, std::list<DOOM::EnumAction::State>({ DOOM::EnumAction::State::StateOpen, DOOM::EnumAction::State::StateWait, DOOM::EnumAction::State::StateClose }));
  case 2: case 31: case 46: case 61: case 86: case 103:	// Open and stay open (slow)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Speed::SpeedSlow>>(doom, std::list<DOOM::EnumAction::State>({ DOOM::EnumAction::State::StateOpen }));
  case 106: case 109: case 112: case 115: case 118:	// Open and stay open (fast)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Speed::SpeedFast>>(doom, std::list<DOOM::EnumAction::State>({ DOOM::EnumAction::State::StateOpen }));
  case 16: case 76: case 175: case 196:	// Close, wait then open (slow)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Speed::SpeedNormal, 1050>>(doom, std::list<DOOM::EnumAction::State>({ DOOM::EnumAction::State::StateClose, DOOM::EnumAction::State::StateWait, DOOM::EnumAction::State::StateOpen }));
  case 3: case 42: case 50: case 75:	// Close and stay close (slow)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Speed::SpeedSlow>>(doom, std::list<DOOM::EnumAction::State>({ DOOM::EnumAction::State::StateClose }));
  case 107: case 110: case 113: case 116:	// Close and stay close (fast)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Speed::SpeedFast>>(doom, std::list<DOOM::EnumAction::State>({ DOOM::EnumAction::State::StateClose }));

    // Regular and extended locked door types
  case 32: case 33: case 34:	// Open and stay open (slow)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Speed::SpeedSlow>>(doom, std::list<DOOM::EnumAction::State>({ DOOM::EnumAction::State::StateOpen }));
  case 99: case 133: case 134: case 135: case 136: case 137:	// Open and stay open (fast)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Speed::SpeedFast>>(doom, std::list<DOOM::EnumAction::State>({ DOOM::EnumAction::State::StateOpen }));
  case 26: case 27: case 28:	// Open, wait then close (slow)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Speed::SpeedSlow, 140>>(doom, std::list<DOOM::EnumAction::State>({ DOOM::EnumAction::State::StateOpen, DOOM::EnumAction::State::StateWait, DOOM::EnumAction::State::StateClose }));

    // Regular floor types
  case 30: case 96:
    break;	// TODO: Abs Shortest Lower Texture
  case 58: case 92:	// Absolute 24 (up, slow)
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Crush::CrushFalse>>(doom, sector.floor_base + 24.f);
  case 59: case 93:
    break;	// TODO: TxTy
  case 140:	// Absolute 512 (up, slow)
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Crush::CrushFalse>>(doom, sector.floor_base + 512.f);
  case 19: case 45: case 83: case 102: {	// Highest neighboor floor (down, slow)
    float	height = sector.getNeighborHighestFloor(doom);
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionDown, DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Crush::CrushFalse>>(doom, std::isnan(height) == true ? -32000.f : height);
  }
  case 36: case 70: case 71: case 98: {	// Highest neighboor floor + 8 (down, fast)
    float	height = sector.getNeighborHighestFloor(doom);
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionDown, DOOM::EnumAction::Speed::SpeedFast, DOOM::EnumAction::Crush::CrushFalse>>(doom, (std::isnan(height) == true ? -32000.f : height) + 8.f);
  }
  case 5: case 24: case 64: case 91: case 101: {	// Lowest sector & neighboor ceiling (up, slow)
    float	height = sector.getNeighborHighestFloor(doom);
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Crush::CrushFalse>>(doom, (std::isnan(height) == true || height > sector.floor_base) ? sector.floor_base : height);
  }
  case 55: case 56: case 65: case 94: {	// Lowest sector & neighboor ceiling - 8 (up, slow, crush)
    float	height = sector.getNeighborHighestFloor(doom);
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Crush::CrushTrue>>(doom, ((std::isnan(height) == true || height > sector.floor_base) ? sector.floor_base : height) - 8.f);
  }
  case 23: case 38: case 60: case 82: {	// Lowest sector & neighboor floor (down, slow)
    float	height = sector.getNeighborLowestFloor(doom);
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionDown, DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Crush::CrushFalse>>(doom, (std::isnan(height) == true || height > sector.floor_base) ? sector.floor_base : height);
  }
  case 37: case 84:
    break;	// TODO: TxTy
  case 18: case 69: case 119: case 128: {	// Next neighboor floor (up, slow)
    float	height = sector.getNeighborNextHighestFloor(doom, sector.floor_base);
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Crush::CrushFalse>>(doom, std::isnan(height) == true ? sector.floor_base : height);
  }
  case 129: case 130: case 131: case 132: {	// Next neighboor floor (up, fast)
    float	height = sector.getNeighborNextHighestFloor(doom, sector.floor_base);
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedFast, DOOM::EnumAction::Crush::CrushFalse>>(doom, std::isnan(height) == true ? sector.floor_base : height);
  }

    // Regular ceiling types
  case 40: {	// Highest neighboor ceiling (up, slow)
    float	height = sector.getNeighborHighestCeiling(doom);
    return std::make_unique<DOOM::CeilingLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Crush::CrushFalse>>(doom, std::isnan(height) == true ? -32000.f : height);
  }
  case 41: case 43:	// Floor (down, fast)
    return std::make_unique<DOOM::CeilingLevelingAction<DOOM::EnumAction::Direction::DirectionDown, DOOM::EnumAction::Speed::SpeedFast, DOOM::EnumAction::Crush::CrushFalse>>(doom, sector.floor_base);
  case 44: case 72:	// Floor + 8 (down, slow)
    return std::make_unique<DOOM::CeilingLevelingAction<DOOM::EnumAction::Direction::DirectionDown, DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Crush::CrushFalse>>(doom, sector.floor_base + 8.f);

    // Regular lighting
  case 17:	// Blinking
    return std::make_unique<DOOM::BlinkLightingAction<35, 5, false>>(doom);

    /* TODO:

    CEILINGS

    49   Ceil   S1  mover slow  -  -   down to floor + 8
    
    LIFTS

    10   Lift   W1  lift  fast  3  -   lift
    21   Lift   S1  lift  fast  3  -   lift
    88   Lift   WRm lift  fast  3  -   lift
    62   Lift   SR  lift  fast  3  -   lift
    121 * Lift   W1  lift  turbo 3  -   lift
    122 * Lift   S1  lift  turbo 3  -   lift
    120 * Lift   WR  lift  turbo 3  -   lift
    123 * Lift   SR  lift  turbo 3  -   lift

    FLOORS

    22   Floor  W1& mover slow  -  TX  up to nhEF
    95   Floor  WR& mover slow  -  TX  up to nhEF
    20   Floor  S1& mover slow  -  TX  up to nhEF
    68   Floor  SR& mover slow  -  TX  up to nhEF
    47   Floor  G1& mover slow  -  TX  up to nhEF
    15   Floor  S1& mover slow  -  TX  up 24
    66   Floor  SR& mover slow  -  TX  up 24
    59   Floor  W1& mover slow  -  TXP up 24
    93   Floor  WR& mover slow  -  TXP up 24
    14   Floor  S1& mover slow  -  TX  up 32
    67   Floor  SR& mover slow  -  TX  up 32
    30   Floor  W1  mover slow  -  -   up ShortestLowerTexture
    96   Floor  WR  mover slow  -  -   up ShortestLowerTexture
    37   Floor  W1  mover slow  -  NXP down to LEF
    84   Floor  WR  mover slow  -  NXP down to LEF
    9   Floor  S1  mover slow  -  NXP donut (see note 12 above)

    MOVING FLOORS

    53   MvFlr  W1& lift  slow  3  -   start moving floor
    54   MvFlr  W1& -     -     -  -   stop moving floor
    87   MvFlr  WR& lift  slow  3  -   start moving floor
    89   MvFlr  WR& -     -     -  -   stop moving floor

    CRUSHING CEILINGS

    6   Crush  W1& crush med   0  -   start crushing, fast hurt
    25   Crush  W1& crush med   0  -   start crushing, slow hurt
    73   Crush  WR& crush slow  0  -   start crushing, slow hurt
    77   Crush  WR& crush med   0  -   start crushing, fast hurt
    57   Crush  W1& -     -     -  -   stop crush
    74   Crush  WR& -     -     -  -   stop crush
    141 * Crush  W1& none? slow  0  -   start crushing, slow hurt "Silent"

    EXIT LEVEL

    11   Exit  nS-  clunk -     -  -   End level, go to next level
    51   Exit  nS-  clunk -     -  -   End level, go to secret level
    52   Exit  nW-  clunk -     -  -   End level, go to next level
    124 * Exit  nW-  clunk -     -  -   End level, go to secret level

    TELEPORT

    39   Telpt  W1m tport -     -  -   Teleport
    97   Telpt  WRm tport -     -  -   Teleport
    125 * Telpt  W1m tport -     -  -   Teleport monsters only
    126 * Telpt  WRm tport -     -  -   Teleport monsters only
    */

  default:
    return nullptr;
  }

  return nullptr;
}

void	DOOM::Doom::Level::Sector::AbstractAction::stop()
{}

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
    return { DOOM::Doom::Resources::NullTexture };

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

    result.push_back(doom.resources.textures.find(frame)->second);
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
  floor_flat(DOOM::StaticFlat::NullFlat),
  ceiling_flat(DOOM::StaticFlat::NullFlat),
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

DOOM::Doom::Resources::AbstractFlat::AbstractFlat(DOOM::Doom & doom)
{}

void	DOOM::Doom::Resources::AbstractFlat::update(DOOM::Doom & doom, sf::Time)
{}

std::unique_ptr<DOOM::Doom::Resources::AbstractFlat>	DOOM::Doom::Resources::AbstractFlat::factory(DOOM::Doom & doom, uint64_t name, DOOM::Wad::RawResources::Flat const & flat)
{
  // List of animated flats and their frames
  static std::vector<std::vector<uint64_t>> const	animations =
  {
    { DOOM::str_to_key("NUKAGE1"), DOOM::str_to_key("NUKAGE2"), DOOM::str_to_key("NUKAGE3") },					// Green slime, nukage
  { DOOM::str_to_key("FWATER1"), DOOM::str_to_key("FWATER2"), DOOM::str_to_key("FWATER3"), DOOM::str_to_key("FWATER4") },	// Blue water
  { DOOM::str_to_key("SWATER1"), DOOM::str_to_key("SWATER2"), DOOM::str_to_key("SWATER3"), DOOM::str_to_key("SWATER4") },	// Blue water
  { DOOM::str_to_key("LAVA1"), DOOM::str_to_key("LAVA2"), DOOM::str_to_key("LAVA3"), DOOM::str_to_key("LAVA4") },		// Lava
  { DOOM::str_to_key("BLOOD1"), DOOM::str_to_key("BLOOD2"), DOOM::str_to_key("BLOOD3") },					// Blood
  { DOOM::str_to_key("RROCK05"), DOOM::str_to_key("RROCK06"), DOOM::str_to_key("RROCK07"), DOOM::str_to_key("RROCK08") },	// Large molten rock
  { DOOM::str_to_key("SLIME01"), DOOM::str_to_key("SLIME02"), DOOM::str_to_key("SLIME03"), DOOM::str_to_key("SLIME04") },	// Brown water
  { DOOM::str_to_key("SLIME05"), DOOM::str_to_key("SLIME06"), DOOM::str_to_key("SLIME07"), DOOM::str_to_key("SLIME08") },	// Brown slime
  { DOOM::str_to_key("SLIME09"), DOOM::str_to_key("SLIME10"), DOOM::str_to_key("SLIME11"), DOOM::str_to_key("SLIME12") }	// Small molten rock
  };

  // Check if flat is part of an animated sequence
  for (std::vector<uint64_t> const & sequence : animations)
    for (uint64_t const frame : sequence)
      if (name == frame)
	return std::make_unique<DOOM::AnimatedFlat<>>(doom, sequence);

  // Default static flat
  return std::make_unique<DOOM::StaticFlat>(doom, flat);
}

DOOM::Doom::Level::AbstractThing::AbstractThing(DOOM::Doom & doom, DOOM::Wad::RawLevel::Thing const & thing, int16_t radius, int16_t properties) :
  position((float)thing.x, (float)thing.y),
  height(0),
  angle(thing.angle / 360.f * 2.f * Math::Pi),
  type(thing.type),
  flag(thing.flag),
  radius(radius),
  properties(properties)
{}

bool	DOOM::Doom::Level::AbstractThing::update(DOOM::Doom & doom, sf::Time)
{
  // Do nothing
  return false;
}

std::unique_ptr<DOOM::Doom::Level::AbstractThing>	DOOM::Doom::Level::AbstractThing::factory(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing)
{
  switch (thing.type)
  {
    // Artifact items
  case 2023:	// Berserk
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PSTR", "A", thing, 20, Artifact | Pickup);
  case 2026:	// Computer map
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PMAP", "ABCDCB", thing, 20, Artifact | Pickup);
  case 2014:	// Health potion
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "BON1", "ABCDCB", thing, 20, Artifact | Pickup);
  case 2024:	// Invisibility
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PINS", "ABCD", thing, 20, Artifact | Pickup);
  case 2022:	// Invulnerability
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PINV", "ABCD", thing, 20, Artifact | Pickup);
  case 2045:	// Light amplification visor
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PVIS", "AB", thing, 20, Artifact | Pickup);
  case 83:	// Megasphere
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "MEGA", "ABCD", thing, 20, Artifact | Pickup);
  case 2013:	// Soul sphere
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SOUL", "ABCDCB", thing, 20, Artifact | Pickup);
  case 2015:	// Spiritual armor
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "BON2", "ABCDCB", thing, 20, Artifact | Pickup);

    // Powerups
  case 8:	// Backpack
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "BPAK", "A", thing, 20, Pickup);
  case 2019:	// Blue armor
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "ARM2", "AB", thing, 20, Pickup);
  case 2018:	// Green armor
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "ARM1", "AB", thing, 20, Pickup);
  case 2012:	// Medikit
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "MEDI", "A", thing, 20, Pickup);
  case 2025:	// Radiation suit
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SUIT", "A", thing, 20, Pickup);
  case 2011:	// Stimpack
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "STIM", "A", thing, 20, Pickup);

    // Weapons
  case 2006:	// BFG 9000
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "BFUG", "A", thing, 20, Pickup);
  case 2002:	// Chaingun
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "MGUN", "A", thing, 20, Pickup);
  case 2005:	// Chainsaw
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "CSAW", "A", thing, 20, Pickup);
  case 2004:	// Plasma rifle
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PLAS", "A", thing, 20, Pickup);
  case 2003:	// Rocket launcher
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "LAUN", "A", thing, 20, Pickup);
  case 2001:	// Shotgun
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SHOT", "A", thing, 20, Pickup);
  case 82:	// Super shotgun
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SGN2", "A", thing, 20, Pickup);

    // Ammunition
  case 2007:	// Ammo clip
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "CLIP", "A", thing, 20, Pickup);
  case 2048:	// Box of ammo
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "AMMO", "A", thing, 20, Pickup);
  case 2046:	// Box of rockets
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "BROK", "A", thing, 20, Pickup);
  case 2049:	// Box of shells
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SBOX", "A", thing, 20, Pickup);
  case 2047:	// Cell charge
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "CELL", "A", thing, 20, Pickup);
  case 17:	// Cell charge pack
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "CELP", "A", thing, 20, Pickup);
  case 2010:	// Rocket
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "ROCK", "A", thing, 20, Pickup);
  case 2008:	// Shotgun shells
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SHEL", "A", thing, 20, Pickup);

    // Monsters (TODO)

    // Obstacles
  case 70:	// Burning barrel
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "FCAN", "A", thing, 10, Obstacle);
  case 43:	// Burnt tree
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "TRE1", "A", thing, 16, Obstacle);
  case 35:	// Candelabra
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "CBRA", "A", thing, 16, Obstacle);
  case 41:	// Evil eye
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "CEYE", "ABCB", thing, 16, Obstacle);
  case 28:	// Five skulls "shish kebab"
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POL2", "A", thing, 16, Obstacle);
  case 42:	// Floating skull
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "KSKU", "ABC", thing, 16, Obstacle);
  case 2028:	// Floor lamp
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "COLU", "A", thing, 16, Obstacle);
  case 53:	// Hanging leg
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR5", "A", thing, 16, Obstacle | Hanging);
  case 52:	// Hanging pair of leg
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR4", "A", thing, 16, Obstacle | Hanging);
  case 78:	// Hanging torso, brain removed
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "HDB6", "A", thing, 16, Obstacle | Hanging);
  case 75:	// Hanging torso, looking down
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "HDB3", "A", thing, 16, Obstacle | Hanging);
  case 77:	// Hanging torso, looking up
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "HDB5", "A", thing, 16, Obstacle | Hanging);
  case 76:	// Hanging torso, open skull
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "HDB4", "A", thing, 16, Obstacle | Hanging);
  case 50:	// Hanging victim, arms out
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR2", "A", thing, 16, Obstacle | Hanging);
  case 74:	// Hanging victim, guts and brain removed
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "HDB2", "A", thing, 16, Obstacle | Hanging);
  case 73:	// Hanging victim, guts removed
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "HDB1", "A", thing, 16, Obstacle | Hanging);
  case 51:	// Hanging victim, one-legged
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR3", "A", thing, 16, Obstacle | Hanging);
  case 49:	// Hanging victim, twitching
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR1", "BACA", thing, 16, Obstacle | Hanging);
  case 25:	// Impaled human
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POL1", "A", thing, 16, Obstacle);
  case 54:	// Large brown tree
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "TRE2", "A", thing, 32, Obstacle);
  case 29:	// Pile of skulls and candles
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POL3", "AB", thing, 16, Obstacle);
  case 55:	// Short blue firestick
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SMBT", "ABCD", thing, 16, Obstacle);
  case 56:	// Short green firestick
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SMGT", "ABCD", thing, 16, Obstacle);
  case 31:	// Short green pillar
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "COL2", "A", thing, 16, Obstacle);
  case 36:	// Short green pillar with beating heart
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "COL5", "AB", thing, 16, Obstacle);
  case 57:	// Short red firestick
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SMRT", "ABCD", thing, 16, Obstacle);
  case 33:	// Short red pillar
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "COL4", "A", thing, 16, Obstacle);
  case 37:	// Short red pillar with skull
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "COL6", "A", thing, 16, Obstacle);
  case 86:	// Short techno floor lamp
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "TLP2", "ABCD", thing, 16, Obstacle);
  case 27:	// Skull on a pole
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POL4", "A", thing, 16, Obstacle);
  case 47:	// Stalagmite
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SMIT", "A", thing, 16, Obstacle);
  case 44:	// Tall blue firestick
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "TBLU", "ABCD", thing, 16, Obstacle);
  case 45:	// Tall green firestick
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "TGRN", "ABCD", thing, 16, Obstacle);
  case 30:	// Tall green pillar
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "COL1", "A", thing, 16, Obstacle);
  case 46:	// Tall red firestick
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "TRED", "ABCD", thing, 16, Obstacle);
  case 32:	// Tall red pillar
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "COL3", "A", thing, 16, Obstacle);
  case 85:	// Tall techno floor lamp
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "TLMP", "ABCD", thing, 16, Obstacle);
  case 48:	// Tall techno pillar
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "ELEC", "A", thing, 16, Obstacle);
  case 26:	// Twitching impaled human
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POL6", "AB", thing, 16, Obstacle);

    // Decorations
  case 10:	// Bloody mess
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PLAY", "W", thing, 16, None);
  case 12:	// Bloody mess
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PLAY", "W", thing, 16, None);
  case 34:	// Candle
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "CAND", "A", thing, 16, None);
  case 22:	// Dead cacodemon
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "HEAD", "L", thing, 31, None);
  case 21:	// Dead demon
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SARG", "N", thing, 30, None);
  case 18:	// Dead former human
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POSS", "L", thing, 20, None);
  case 19:	// Dead former sergeant
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SPOS", "L", thing, 20, None);
  case 20:	// Dead imp
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "TROO", "M", thing, 20, None);
  case 23:	// Dead lost soul (invisible)
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SKUL", "K", thing, 16, None);
  case 15:	// Dead player
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PLAY", "N", thing, 16, None);
  case 62:	// Hanging leg
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR5", "A", thing, 16, Hanging);
  case 60:	// Hanging pair of legs
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR4", "A", thing, 16, Hanging);
  case 59:	// Hanging victim, arms out
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR2", "A", thing, 16, Hanging);
  case 61:	// Hanging victimn one-legged
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR3", "A", thing, 16, Hanging);
  case 63:	// Hanging victim, twitching
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR1", "BACA", thing, 16, Hanging);
  case 79:	// Pool of blood
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POB1", "A", thing, 16, None);
  case 80:	// Pool of blood
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POB2", "A", thing, 16, None);
  case 24:	// Pool of blood and flesh
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POL5", "A", thing, 16, None);
  case 81:	// Pool of brains
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "BRS1", "A", thing, 16, None);

    // Other (TODO)

  default:
    return std::make_unique<DOOM::NullThing>(doom, thing);
  }
}

DOOM::Doom::Level::AbstractLinedef::AbstractLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
  start(linedef.start), end(linedef.end),
  flag(linedef.flag),
  type(linedef.type),
  tag(linedef.tag),
  front(linedef.front), back(linedef.back)
{}

DOOM::Doom::Level::AbstractLinedef::AbstractLinedef(DOOM::Doom & doom, const DOOM::Doom::Level::AbstractLinedef & linedef) :
  start(linedef.start), end(linedef.end),
  flag(linedef.flag),
  type(linedef.type),
  tag(linedef.tag),
  front(linedef.front), back(linedef.back)
{}

std::unique_ptr<DOOM::Doom::Level::AbstractLinedef>	DOOM::Doom::Level::AbstractLinedef::factory(DOOM::Doom & doom, DOOM::Wad::RawLevel::Linedef const & linedef)
{
  switch (linedef.type)
  {
  case 0:	// Normal linedef
    return std::make_unique<DOOM::NullLinedef>(doom, linedef);

    // Regular door types
  case 46:	// Gunfire repeatable
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerGunfire, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 31: case 118:	// Pushed once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 117:	// Pushed repeatable
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 1:	// Pushed repeatable (monster)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue, DOOM::EnumLinedef::Monster::MonsterTrue>>(doom, linedef);
  case 103: case 50: case 113: case 112: case 111: case 29:	// Switched once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 114: case 116: case 42: case 115: case 63: case 61:	// Switched repeatable
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 2: case 3: case 110: case 108: case 109: case 16:	// Walkover once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 4:	// Walkover once (monster)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse, DOOM::EnumLinedef::Monster::MonsterTrue>>(doom, linedef);
  case 106: case 107: case 75: case 105: case 86: case 90: case 76:	// Walkover repeatable
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);

    // Regular locked door types
  case 32:	// Pushed once (blue)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyBlue>>(doom, linedef);
  case 33:	// Pushed once (red)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyRed>>(doom, linedef);
  case 34:	// Pushed once (yellow)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyYellow>>(doom, linedef);
  case 26:	// Pushed repeatable (blue)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyBlue>>(doom, linedef);
  case 27:	// Pushed repeatable (yellow)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyYellow>>(doom, linedef);
  case 28:	// Pushed repeatable (red)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyRed>>(doom, linedef);
  case 133:	// Switched once (blue)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyBlue>>(doom, linedef);
  case 135:	// Switched once (red)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyRed>>(doom, linedef);
  case 137:	// Switched once (yellow)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyYellow>>(doom, linedef);
  case 99:	// Switched repeatable (blue)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyBlue>>(doom, linedef);
  case 134:	// Switched repeatable (red)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyRed>>(doom, linedef);
  case 136:	// Switched repeatable (yellow)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyYellow>>(doom, linedef);

    // Regular floor types
  case 82: case 83: case 84: case 91: case 92: case 93: case 96: case 98: case 128: case 129:	// Walkover repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 5: case 19: case 30: case 36: case 37: case 38: case 56: case 58: case 59: case 119: case 130:	// Walkover once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 45: case 64: case 65: case 69: case 70: case 60: case 132:	// Switched repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 18: case 23: case 55: case 71: case 101: case 102: case 131: case 140:	// Switched once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 24:	// Gunfire once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerGunfire, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);

    // Regular ceiling types
  case 72:	// Walkover repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 40: case 44:	// Walkover once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 43:	// Switched repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 41:	// Switched once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);

    // Regular platform lift
  case 87: case 88: case 95: case 120:	// Walkover repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 10: case 22: case 53: case 121:	// Walkover one
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 62: case 66: case 67: case 68: case 123:	// Switched repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 14: case 15: case 20: case 21: case 122:	// Switched once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 47:	// Gunfire once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerGunfire, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);

    // Regular crusher
  case 73: case 77:	// Walkover repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 6: case 25: case 141:	// Walkover once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 49:	// Switched once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);

    // Regular lighting
  case 12:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Light::LightMaximum, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 13:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Light::Light255, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 17:
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Action::ActionLeveling, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 35:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Light::Light35, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 79:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Light::Light35, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 80:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Light::LightMaximum, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 81:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Light::Light255, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 104:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Light::LightMinimum, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 138:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Light::Light255, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 139:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Light::Light35, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);

    // Regular stair builders
  case 7:	// Switched once, slow, 8 step
    return std::make_unique<DOOM::StairTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumAction::Speed::SpeedSlow, 8>>(doom, linedef);
  case 8:	// Walkover once, slow, 8 step
    return std::make_unique<DOOM::StairTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumAction::Speed::SpeedSlow, 8>>(doom, linedef);
  case 100:	// Walkover once, fast, 16 step
    return std::make_unique<DOOM::StairTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumAction::Speed::SpeedFast, 16>>(doom, linedef);
  case 127:	// Switched once, fast, 16 step
    return std::make_unique<DOOM::StairTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumAction::Speed::SpeedFast, 16>>(doom, linedef);

    // TODO: type 54 & 89 stop perpetual platform lift (remove sector leveling action)
    // TODO: type 57 & 74 stop crusher (remove sector leveling action)
    // TODO: stair builder
    // TODO: exits
    // TODO: teleporters
    // TODO: donuts
    // TODO: scrolling wall (48 left, 85 right)

  default:	// TODO: return nullptr for error
    return std::make_unique<DOOM::NullLinedef>(doom, linedef);
  }
}

void	DOOM::Doom::Level::AbstractLinedef::pushed(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing)
{}

void	DOOM::Doom::Level::AbstractLinedef::switched(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing)
{}

void	DOOM::Doom::Level::AbstractLinedef::walkover(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing)
{}

void	DOOM::Doom::Level::AbstractLinedef::gunfire(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing)
{}

DOOM::Doom::Level::Level() :
  episode(0, 0),
  sky(DOOM::Doom::Resources::NullTexture),
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