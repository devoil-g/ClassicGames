#include "Doom/Action/AbstractAction.hpp"
#include "Doom/Action/BlinkLightingAction.hpp"
#include "Doom/Action/CeilingLevelingAction.hpp"
#include "Doom/Action/DoorLevelingAction.hpp"
#include "Doom/Action/FlickerLightingAction.hpp"
#include "Doom/Action/FloorLevelingAction.hpp"
#include "Doom/Action/OscillateLightingAction.hpp"
#include "Doom/Action/RandomLightingAction.hpp"

DOOM::AbstractAction::AbstractAction(DOOM::Doom & doom)
{}

std::unique_ptr<DOOM::AbstractAction>	DOOM::AbstractAction::factory(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, int16_t type)
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
  case 30: case 96: {	// Abs Shortest Lower Texture
    int16_t	height = sector.getShortestLowerTexture(doom);
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Crush::CrushFalse>>(doom, height == 0 ? +32000.f : sector.floor_base + height);
  }
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
    float	height = sector.getNeighborLowestCeiling(doom);
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Crush::CrushFalse>>(doom, (std::isnan(height) == true || height > sector.ceiling_base) ? sector.ceiling_base : height);
  }
  case 55: case 56: case 65: case 94: {	// Lowest sector & neighboor ceiling - 8 (up, slow, crush)
    float	height = sector.getNeighborLowestCeiling(doom);
    return std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, DOOM::EnumAction::Speed::SpeedSlow, DOOM::EnumAction::Crush::CrushTrue>>(doom, ((std::isnan(height) == true || height > sector.ceiling_base) ? sector.ceiling_base : height) - 8.f);
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

  sizeof(long double);

  return nullptr;
}

void	DOOM::AbstractAction::stop()
{}
