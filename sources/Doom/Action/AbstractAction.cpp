#include "Doom/Action/AbstractAction.hpp"
#include "Doom/Action/BlinkLightingAction.hpp"
#include "Doom/Action/CeilingLevelingAction.hpp"
#include "Doom/Action/CrusherLevelingAction.hpp"
#include "Doom/Action/DoorLevelingAction.hpp"
#include "Doom/Action/FlickerLightingAction.hpp"
#include "Doom/Action/FloorLevelingAction.hpp"
#include "Doom/Action/LiftLevelingAction.hpp"
#include "Doom/Action/OscillateLightingAction.hpp"
#include "Doom/Action/PlatformLevelingAction.hpp"
#include "Doom/Action/RandomLightingAction.hpp"

DOOM::AbstractAction::AbstractAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector)
{}

std::unique_ptr<DOOM::AbstractAction> DOOM::AbstractAction::factory(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, int16_t type, int16_t model)
{
  // Generate action from type
  switch (type) {
    // Regular and extended door types
  case 1: case 4: case 29: case 63: case 90:            // Open, wait then close (normal)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Door::DoorOpenWaitClose, DOOM::EnumAction::Speed::SpeedNormal, 140>>(doom, sector);
  case 105: case 108: case 111: case 114: case 117:     // Open, wait then close (turbo)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Door::DoorOpenWaitClose, DOOM::EnumAction::Speed::SpeedTurbo, 140>>(doom, sector);
  case 2: case 31: case 46: case 61: case 86: case 103: // Open and stay open (normal)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Door::DoorWaitOpen, DOOM::EnumAction::Speed::SpeedNormal, 0>>(doom, sector);
  case 106: case 109: case 112: case 115: case 118:     // Open and stay open (turbo)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Door::DoorWaitOpen, DOOM::EnumAction::Speed::SpeedTurbo, 0>>(doom, sector);
  case 16: case 76:                                     // Close, wait then open (normal)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Door::DoorCloseWaitOpen, DOOM::EnumAction::Speed::SpeedNormal, 1050>>(doom, sector);
  case 3: case 42: case 50: case 75:                    // Close and stay close (normal)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Door::DoorWaitClose, DOOM::EnumAction::Speed::SpeedNormal, 0>>(doom, sector);
  case 107: case 110: case 113: case 116:               // Close and stay close (turbo)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Door::DoorWaitClose, DOOM::EnumAction::Speed::SpeedTurbo, 0>>(doom, sector);

    // Regular and extended locked door types
  case 32: case 33: case 34:                                  // Open and stay open (normal)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Door::DoorWaitOpen, DOOM::EnumAction::Speed::SpeedNormal, 0>>(doom, sector);
  case 99: case 133: case 134: case 135: case 136: case 137:  // Open and stay open (turbo)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Door::DoorWaitOpen, DOOM::EnumAction::Speed::SpeedTurbo, 0>>(doom, sector);
  case 26: case 27: case 28:                                  // Open, wait then close (med)
    return std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Door::DoorOpenWaitClose, DOOM::EnumAction::Speed::SpeedSlow, 140>>(doom, sector);

    // Regular floor types
  case 30: case 96: {                             // Abs Shortest Lower Texture (slow)
    int16_t height = sector.getShortestLowerTexture(doom);
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedSlow>>(doom, sector, height == 0 ? +32000.f : sector.floor_base + height);
  }
  case 58: case 92:                               // Absolute 24 (up, slow)
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedSlow>>(doom, sector, sector.floor_base + 24.f);
  case 59: case 93:                               // Absolute 24 (up, slow, TxTy)
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Change::Type::TextureType, DOOM::EnumAction::Change::Time::After>>(doom, sector, sector.floor_base + 24.f, model);
  case 140:                                       // Absolute 512 (up, normal)
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedNormal>>(doom, sector, sector.floor_base + 512.f);
  case 19: case 45: case 83: case 102: {          // Highest neighboor floor (down, slow)
    const float height = sector.getNeighborHighestFloor(doom).second;
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionDown, DOOM::EnumAction::Speed::SpeedSlow>>(doom, sector, std::isnan(height) == true ? -32000.f : height);
  }
  case 36: case 70: case 71: case 98: {           // Highest neighboor floor + 8 (down, fast)
    const float	height = sector.getNeighborHighestFloor(doom).second;
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionDown, DOOM::EnumAction::Speed::SpeedFast>>(doom, sector, (std::isnan(height) == true ? -32000.f : height) + 8.f);
  }
  case 5: case 24: case 64: case 91: case 101: {  // Lowest sector & neighboor ceiling (up, slow)
    const float	height = sector.getNeighborLowestCeiling(doom).second;
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedSlow>>(doom, sector, (std::isnan(height) == true || height > sector.ceiling_base) ? sector.ceiling_base : height);
  }
  case 55: case 56: case 65: case 94: {           // Lowest sector & neighboor ceiling - 8 (up, slow, crush)
    const float	height = sector.getNeighborLowestCeiling(doom).second;
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Change::Type::None, DOOM::EnumAction::Change::Time::Before, true>>(doom, sector, ((std::isnan(height) == true || height > sector.ceiling_base) ? sector.ceiling_base : height) - 8.f);
  }
  case 23: case 38: case 60: case 82: {           // Lowest sector & neighboor floor (down, slow)
    const float	height = sector.getNeighborLowestFloor(doom).second;
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionDown, DOOM::EnumAction::Speed::SpeedSlow>>(doom, sector, (std::isnan(height) == true || height > sector.floor_base) ? sector.floor_base : height);
  }
  case 37: case 84: {                             // Lowest sector & neighboor floor (down, slow, TxTy)
    const std::pair<int16_t, float>	lowest = sector.getNeighborLowestFloor(doom);
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionDown, DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Change::Type::TextureType, DOOM::EnumAction::Change::Time::After>>(doom, sector, (lowest.first == -1 || lowest.second > sector.floor_base) ? sector.floor_base : lowest.second, lowest.first);
  }
  case 18: case 69: case 119: case 128: {         // Next neighboor floor (up, slow)
    const float	height = sector.getNeighborNextHighestFloor(doom, sector.floor_base).second;
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedSlow>>(doom, sector, std::isnan(height) == true ? sector.floor_base : height);
  }
  case 129: case 130: case 131: case 132: {       // Next neighboor floor (up, turbo)
    const float	height = sector.getNeighborNextHighestFloor(doom, sector.floor_base).second;
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedTurbo>>(doom, sector, std::isnan(height) == true ? sector.floor_base : height);
  }

    // Regular ceiling types
  case 40: {        // Highest neighboor ceiling (up, slow)
    const float	height = sector.getNeighborHighestCeiling(doom).second;
    return std::make_unique<DOOM::CeilingLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedSlow>>(doom, sector, std::isnan(height) == true ? -32000.f : height);
  }
  case 41: case 43: // Floor (down, slow)
    return std::make_unique<DOOM::CeilingLevelingAction<DOOM::EnumAction::Direction::DirectionDown, DOOM::EnumAction::Speed::SpeedSlow>>(doom, sector, sector.floor_base);
  case 44: case 72: // Floor + 8 (down, slow)
    return std::make_unique<DOOM::CeilingLevelingAction<DOOM::EnumAction::Direction::DirectionDown, DOOM::EnumAction::Speed::SpeedSlow>>(doom, sector, sector.floor_base + 8.f);

    // Regular lighting
  case 17:  // Blinking
    return std::make_unique<DOOM::BlinkLightingAction<35, 5, false>>(doom, sector);

    // Platform
  case 15: case 66:                               // Raise 24 units (Tx, slow)
    return std::make_unique<DOOM::PlatformLevelingAction<DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Change::Type::Texture, DOOM::EnumAction::Change::Time::After>>(doom, sector, sector.floor_base + 24.f, model);
  case 14: case 67:	                          // Raise 32 units (Tx0, slow)
    return std::make_unique<DOOM::PlatformLevelingAction<DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Change::Type::TextureZeroed, DOOM::EnumAction::Change::Time::After>>(doom, sector, sector.floor_base + 32.f, model);
  case 20: case 22: case 47: case 68: case 95: {  // Raise next floor (Tx0, slow)
    const float	height = sector.getNeighborNextHighestFloor(doom, sector.floor_base).second;
    return std::make_unique<DOOM::PlatformLevelingAction<DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Change::Type::TextureZeroed, DOOM::EnumAction::Change::Time::Before>>(doom, sector, std::isnan(height) == true ? sector.floor_base : height, model);
  }

    // Lift
  case 10: case 21: case 62: case 88: {     // Lowest neighboor floor (fast)
    const float	height = sector.getNeighborLowestFloor(doom).second;
    return std::make_unique<DOOM::LiftLevelingAction<DOOM::EnumAction::Speed::SpeedFast>>(doom, sector, std::isnan(height) == true ? sector.floor_base : std::min(height, sector.floor_base), sector.floor_base);
  }
  case 120: case 121: case 122: case 123: { // Lowest neighboor floor (turbo)
    const float	height = sector.getNeighborLowestFloor(doom).second;
    return std::make_unique<DOOM::LiftLevelingAction<DOOM::EnumAction::Speed::SpeedTurbo>>(doom, sector, std::isnan(height) == true ? sector.floor_base : std::min(height, sector.floor_base), sector.floor_base);
  }
  case 53: case 87: {                       // Lowest and highest floor (perpetual, slow)
    const float	height_low = sector.getNeighborLowestFloor(doom).second;
    const float	height_high = sector.getNeighborHighestFloor(doom).second;
    return std::make_unique<DOOM::LiftLevelingAction<DOOM::EnumAction::Speed::SpeedSlow, true>>(doom, sector, std::isnan(height_low) == true ? sector.floor_base : std::min(height_low, sector.floor_base), std::isnan(height_high) == true ? sector.floor_base : std::max(height_high, sector.floor_base));
  }

    // Regular crusher ceiling
  case 6: case 77:
    return std::make_unique<DOOM::CrusherLevelingAction<DOOM::EnumAction::Speed::SpeedFast>>(doom, sector);
  case 25: case 49: case 73:
    return std::make_unique<DOOM::CrusherLevelingAction<DOOM::EnumAction::Speed::SpeedSlow>>(doom, sector);
  case 141:
    return std::make_unique<DOOM::CrusherLevelingAction<DOOM::EnumAction::Speed::SpeedSlow, true>>(doom, sector);

  default:
    return nullptr;
  }

  return nullptr;
}

bool	                              DOOM::AbstractAction::stop(DOOM::Doom& doom, DOOM::AbstractThing& thing)
{
  return false;
}

bool	                              DOOM::AbstractAction::start(DOOM::Doom& doom, DOOM::AbstractThing& thing)
{
  return false;
}