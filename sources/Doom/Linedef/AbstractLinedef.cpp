#include "Doom/Doom.hpp"
#include "Doom/Linedef/AbstractLinedef.hpp"
#include "Doom/Linedef/ActionTriggerableLinedef.hpp"
#include "Doom/Linedef/LightTriggerableLinedef.hpp"
#include "Doom/Linedef/StairTriggerableLinedef.hpp"
#include "Doom/Linedef/NullLinedef.hpp"

DOOM::AbstractLinedef::AbstractLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
  start(linedef.start), end(linedef.end),
  flag(linedef.flag),
  type(linedef.type),
  tag(linedef.tag),
  front(linedef.front), back(linedef.back)
{}

DOOM::AbstractLinedef::AbstractLinedef(DOOM::Doom & doom, const DOOM::AbstractLinedef & linedef) :
  start(linedef.start), end(linedef.end),
  flag(linedef.flag),
  type(linedef.type),
  tag(linedef.tag),
  front(linedef.front), back(linedef.back)
{}

std::unique_ptr<DOOM::AbstractLinedef>	DOOM::AbstractLinedef::factory(DOOM::Doom & doom, DOOM::Wad::RawLevel::Linedef const & linedef)
{
  switch (linedef.type)
  {
  case 0:	// Normal linedef
    return std::make_unique<DOOM::NullLinedef>(doom, linedef);

    // Regular door types
  case 46:	// Gunfire repeatable
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerGunfire, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 31: case 118:	// Pushed once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 117:	// Pushed repeatable
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 1:	// Pushed repeatable (monster)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Repeat::RepeatTrue, DOOM::EnumLinedef::Monster::MonsterTrue>>(doom, linedef);
  case 103: case 50: case 113: case 112: case 111: case 29:	// Switched once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 114: case 116: case 42: case 115: case 63: case 61:	// Switched repeatable
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 2: case 3: case 110: case 108: case 109: case 16:	// Walkover once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 4:	// Walkover once (monster)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatFalse, DOOM::EnumLinedef::Monster::MonsterTrue>>(doom, linedef);
  case 106: case 107: case 75: case 105: case 86: case 90: case 76:	// Walkover repeatable
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);

    // Regular locked door types
  case 32:	// Pushed once (blue)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Repeat::RepeatFalse, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyBlue>>(doom, linedef);
  case 33:	// Pushed once (red)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Repeat::RepeatFalse, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyRed>>(doom, linedef);
  case 34:	// Pushed once (yellow)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Repeat::RepeatFalse, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyYellow>>(doom, linedef);
  case 26:	// Pushed repeatable (blue)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Repeat::RepeatTrue, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyBlue>>(doom, linedef);
  case 27:	// Pushed repeatable (yellow)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Repeat::RepeatTrue, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyYellow>>(doom, linedef);
  case 28:	// Pushed repeatable (red)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerPushed, DOOM::EnumLinedef::Repeat::RepeatTrue, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyRed>>(doom, linedef);
  case 133:	// Switched once (blue)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatFalse, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyBlue>>(doom, linedef);
  case 135:	// Switched once (red)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatFalse, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyRed>>(doom, linedef);
  case 137:	// Switched once (yellow)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatFalse, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyYellow>>(doom, linedef);
  case 99:	// Switched repeatable (blue)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatTrue, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyBlue>>(doom, linedef);
  case 134:	// Switched repeatable (red)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatTrue, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyRed>>(doom, linedef);
  case 136:	// Switched repeatable (yellow)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatTrue, DOOM::EnumLinedef::Monster::MonsterFalse, DOOM::EnumLinedef::Key::KeyYellow>>(doom, linedef);

    // Regular floor types
  case 82: case 83: case 84: case 91: case 92: case 93: case 96: case 98: case 128: case 129:	// Walkover repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 5: case 19: case 30: case 36: case 37: case 38: case 56: case 58: case 59: case 119: case 130:	// Walkover once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 45: case 64: case 65: case 69: case 70: case 60: case 132:	// Switched repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 18: case 23: case 55: case 71: case 101: case 102: case 131: case 140:	// Switched once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 24:	// Gunfire once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerGunfire, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);

    // Regular ceiling types
  case 72:	// Walkover repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 40: case 44:	// Walkover once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 43:	// Switched repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 41:	// Switched once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);

    // Regular platform lift
  case 87: case 88: case 95: case 120:	// Walkover repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 10: case 22: case 53: case 121:	// Walkover one
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 62: case 66: case 67: case 68: case 123:	// Switched repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 14: case 15: case 20: case 21: case 122:	// Switched once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 47:	// Gunfire once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerGunfire, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);

    // Regular crusher
  case 73: case 77:	// Walkover repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 6: case 25: case 141:	// Walkover once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 49:	// Switched once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLeveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);

    // Regular lighting
  case 12:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::LightMaximum, DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 13:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::Light255, DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 17:
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::EnumAction::Type::TypeLighting, DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 35:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::Light35, DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 79:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::Light35, DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 80:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::LightMaximum, DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 81:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::Light255, DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 104:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::LightMinimum, DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumLinedef::Repeat::RepeatFalse>>(doom, linedef);
  case 138:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::Light255, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);
  case 139:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::Light35, DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumLinedef::Repeat::RepeatTrue>>(doom, linedef);

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

void	DOOM::AbstractLinedef::pushed(DOOM::Doom & doom, DOOM::AbstractThing & thing)
{}

void	DOOM::AbstractLinedef::switched(DOOM::Doom & doom, DOOM::AbstractThing & thing)
{}

void	DOOM::AbstractLinedef::walkover(DOOM::Doom & doom, DOOM::AbstractThing & thing)
{}

void	DOOM::AbstractLinedef::gunfire(DOOM::Doom & doom, DOOM::AbstractThing & thing)
{}
