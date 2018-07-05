#include <iostream>

#include "Doom/Doom.hpp"
#include "Doom/Sector/AbstractSector.hpp"
#include "Doom/Sector/NormalSector.hpp"
#include "Doom/Sector/FixedLightSector.hpp"
#include "Doom/Sector/FlickerLightSector.hpp"
#include "Doom/Sector/GlowLightSector.hpp"
#include "Doom/Sector/RandomLightSector.hpp"
#include "Doom/Sector/Action/DoorAction.hpp"

const std::vector<int16_t>	DOOM::AbstractSector::AbstractAction::LevelingTypes = {
  63
};

const std::vector<int16_t>	DOOM::AbstractSector::AbstractAction::LightingTypes = {

};

DOOM::AbstractSector::AbstractSector(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector) :
  floor_name(sector.floor_texture),
  ceiling_name(sector.ceiling_texture),
  floor_flat(nullptr),
  ceiling_flat(nullptr),
  tag(sector.tag),
  _light(sector.light), _baseLight(sector.light),
  _floor(sector.floor_height), _baseFloor(sector.floor_height),
  _ceiling(sector.ceiling_height), _baseCeiling(sector.ceiling_height),
  _damage(0), _baseDamage(0),
  _neighbors(),
  _doom(doom)
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
}

DOOM::AbstractSector::~AbstractSector()
{}

void	DOOM::AbstractSector::update(sf::Time elapsed)
{
  // Update sector actions
  if (_leveling.get() != nullptr)
    _leveling->update(elapsed);
  if (_lighting.get() != nullptr)
    _lighting->update(elapsed);
}

DOOM::AbstractSector *	DOOM::AbstractSector::factory(DOOM::Doom const & doom, DOOM::Wad::RawLevel::Sector const & sector)
{
  switch (sector.special)
  {
  case Special::Normal:			// Normal
    return new DOOM::NormalSector(doom, sector);
  case Special::LightBlinkRandom:	// Blink random
    return new DOOM::RandomLightSector(doom, sector);
  case Special::LightBlink05:		// Blink 0.5 second
    return new DOOM::FixedLightSector<18, 4>(doom, sector);
  case Special::LightBlink10:		// Blink 1.0 second
    return new DOOM::FixedLightSector<36, 6>(doom, sector);
  case Special::Damage20Blink05:	// 20 % damage per second plus light blink 0.5 second
    return new DOOM::FixedLightSector<18, 4>(doom, sector);
  case Special::Damage10:		// 10 % damage per second
    return new DOOM::NormalSector(doom, sector);
  case Special::Damage5:		// 5 % damage per second
    return new DOOM::NormalSector(doom, sector);
  case Special::LightOscillates:	// Oscillates
    return new DOOM::GlowLightSector(doom, sector);
  case Special::Secret:			// Player entering this sector gets credit for finding a secret
    return new DOOM::NormalSector(doom, sector);
  case Special::DoorClose:		// 30 seconds after level start, ceiling closes like a door
    return new DOOM::NormalSector(doom, sector);
  case Special::End:			// Cancel God mode if active, 20 % damage per second, when player dies, level ends
    return new DOOM::NormalSector(doom, sector);
  case Special::LightBlink05Sync:	// Blink 0.5 second, synchronized
    return new DOOM::FixedLightSector<18, 4>(doom, sector);
  case Special::LightBlink10Sync:	// Blink 1.0 second, synchronized
    return new DOOM::FixedLightSector<36, 6>(doom, sector);
  case Special::DoorOpen:		// 300 seconds after level start, ceiling opens like a door
    return new DOOM::NormalSector(doom, sector);
  case Special::Damage20:		// 20 % damage per second
    return new DOOM::NormalSector(doom, sector);
  case Special::LightFlickers:		// Flickers randomly
    return new DOOM::FlickerLightSector(doom, sector);
  default:
    return nullptr;
  }
}

void	DOOM::AbstractSector::action(int16_t type)
{
  // Check if action is a leveling effect
  if (std::find(DOOM::AbstractSector::AbstractAction::LevelingTypes.begin(), DOOM::AbstractSector::AbstractAction::LevelingTypes.end(), type) != DOOM::AbstractSector::AbstractAction::LevelingTypes.end()) {
    if (_leveling.get() == nullptr)
      _leveling.reset(DOOM::AbstractSector::AbstractAction::factory(*this, type));
  }

  // Check if action is a lighting effect
  else if (std::find(DOOM::AbstractSector::AbstractAction::LightingTypes.begin(), DOOM::AbstractSector::AbstractAction::LightingTypes.end(), type) != DOOM::AbstractSector::AbstractAction::LightingTypes.end()) {
    if (_lighting.get() == nullptr)
      _lighting.reset(DOOM::AbstractSector::AbstractAction::factory(*this, type));
  }

  // Warning if unknown action
  else {
    // std::cerr << "[Doom::AbstractSector]: Warning, unknown action type '" << type << "'." << std::endl;
  }
}

float	DOOM::AbstractSector::getNeighborLowestFloor() const
{
  float	result = std::numeric_limits<float>::quiet_NaN();

  // Find lowest neighboor floor
  for (int16_t index : _neighbors)
    result = std::isnan(result) == true ? _doom.level.sectors[index]->floor() : std::min(result, _doom.level.sectors[index]->floor());

  return result;
}

float	DOOM::AbstractSector::getNeighborHighestFloor() const
{
  float	result = std::numeric_limits<float>::quiet_NaN();

  // Find highest neighboor floor
  for (int16_t index : _neighbors)
    result = std::isnan(result) == true ? _doom.level.sectors[index]->floor() : std::max(result, _doom.level.sectors[index]->floor());

  return result;
}

float	DOOM::AbstractSector::getNeighborNextLowestFloor(float height) const
{
  float		result = std::numeric_limits<float>::quiet_NaN();

  // Find next lowest neighboor floor
  for (int16_t index : _neighbors) {
    float	floor = _doom.level.sectors[index]->floor();

    if (floor < height && (std::isnan(result) == true || floor > result))
      result = std::max(result, floor);
  }

  return result;
}

float	DOOM::AbstractSector::getNeighborNextHighestFloor(float height) const
{
  float		result = std::numeric_limits<float>::quiet_NaN();

  // Find next highest neighboor floor
  for (int16_t index : _neighbors) {
    float	floor = _doom.level.sectors[index]->floor();

    if (floor > height && (std::isnan(result) == true || floor < result))
      result = std::min(result, floor);
  }

  return result;
}

float	DOOM::AbstractSector::getNeighborLowestCeiling() const
{
  float	result = std::numeric_limits<float>::quiet_NaN();

  // Find lowest neighboor ceiling
  for (int16_t index : _neighbors)
    result = std::isnan(result) == true ? _doom.level.sectors[index]->ceiling() : std::min(result, _doom.level.sectors[index]->ceiling());

  return result;
}

float	DOOM::AbstractSector::getNeighborHighestCeiling() const
{
  float	result = std::numeric_limits<float>::quiet_NaN();

  // Find highest neighboor ceiling
  for (int16_t index : _neighbors)
    result = std::isnan(result) == true ? _doom.level.sectors[index]->ceiling() : std::max(result, _doom.level.sectors[index]->ceiling());

  return result;
}

float	DOOM::AbstractSector::getNeighborNextLowestCeiling(float height) const
{
  float		result = std::numeric_limits<float>::quiet_NaN();

  // Find next lowest neighboor ceiling
  for (int16_t index : _neighbors) {
    float	ceiling = _doom.level.sectors[index]->ceiling();

    if (ceiling < height && (std::isnan(result) == true || ceiling > result))
      result = std::max(result, ceiling);
  }

  return result;
}

float	DOOM::AbstractSector::getNeighborNextHighestCeiling(float height) const
{
  float		result = std::numeric_limits<float>::quiet_NaN();

  // Find next highest neighboor floor
  for (int16_t index : _neighbors) {
    float	ceiling = _doom.level.sectors[index]->ceiling();

    if (ceiling > height && (std::isnan(result) == true || ceiling < result))
      result = std::min(result, ceiling);
  }

  return result;
}

int16_t	DOOM::AbstractSector::getNeighborLowestLight() const
{
  int16_t	result = light();

  // Find lowest neighboor light level
  for (int16_t index : _neighbors)
    result = std::min(result, _doom.level.sectors[index]->_light);

  return result;
}

int16_t	DOOM::AbstractSector::getNeighborHighestLight() const
{
  int16_t	result = light();

  // Find highest neighboor light level
  for (int16_t index : _neighbors)
    result = std::min(result, _doom.level.sectors[index]->_light);

  return result;
}

DOOM::AbstractSector::AbstractAction::AbstractAction(DOOM::AbstractSector & sector)
  : sector(sector)
{}

DOOM::AbstractSector::AbstractAction::~AbstractAction()
{}

DOOM::AbstractSector::AbstractAction *	DOOM::AbstractSector::AbstractAction::factory(DOOM::AbstractSector & sector, int16_t type)
{
  // Generate action from type
  switch (type) {
  case 63:
    return new DOOM::DoorAction(sector, { DOOM::DoorAction::State::StateOpen, DOOM::DoorAction::State::StateWait, DOOM::DoorAction::State::StateClose }, DOOM::DoorAction::Speed::SpeedNormal, (int)(sf::seconds(4.f).asMicroseconds() / DOOM::Doom::Tic.asMicroseconds()));

  default:
    return nullptr;
  }

  return nullptr;
}

void	DOOM::AbstractSector::AbstractAction::remove()
{
  // Remove action from sector
  if (sector._leveling.get() == this)
    sector._leveling.reset();
  else if (sector._lighting.get() == this)
    sector._lighting.reset();
}