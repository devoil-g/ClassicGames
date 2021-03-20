#include "Doom/Doom.hpp"
#include "Doom/Linedef/AbstractLinedef.hpp"
#include "Doom/Linedef/ActionTriggerableLinedef.hpp"
#include "Doom/Linedef/DonutTriggerableLinedef.hpp"
#include "Doom/Linedef/ExitLinedef.hpp"
#include "Doom/Linedef/LightTriggerableLinedef.hpp"
#include "Doom/Linedef/StairTriggerableLinedef.hpp"
#include "Doom/Linedef/StopTriggerableLinedef.hpp"
#include "Doom/Linedef/TeleporterTriggerableLinedef.hpp"
#include "Doom/Linedef/NullLinedef.hpp"
#include "Doom/Linedef/ScrollerLinedef.hpp"

DOOM::AbstractLinedef::AbstractLinedef(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Linedef& linedef) :
  start(linedef.start), end(linedef.end),
  flag(linedef.flag),
  type(linedef.type),
  tag(linedef.tag),
  front(linedef.front), back(linedef.back)
{}

DOOM::AbstractLinedef::AbstractLinedef(DOOM::Doom& doom, const DOOM::AbstractLinedef& linedef) :
  start(linedef.start), end(linedef.end),
  flag(linedef.flag),
  type(linedef.type),
  tag(linedef.tag),
  front(linedef.front), back(linedef.back)
{}

std::unique_ptr<DOOM::AbstractLinedef>  DOOM::AbstractLinedef::factory(DOOM::Doom& doom, DOOM::Wad::RawLevel::Linedef const& linedef)
{
  switch (linedef.type)
  {
  case 0: // Normal linedef
    return std::make_unique<DOOM::NullLinedef>(doom, linedef);

    // Regular door types
  case 46:                                                          // Gunfire repeatable
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerGunfire, true>>(doom, linedef);
  case 31: case 118:                                                // Pushed once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerPushed, false>>(doom, linedef);
  case 117:                                                         // Pushed repeatable
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerPushed, true>>(doom, linedef);
  case 1:                                                           // Pushed repeatable (monster)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerPushed, true, DOOM::EnumLinedef::Target::TargetPlayerMonster>>(doom, linedef);
  case 103: case 50: case 113: case 112: case 111: case 29:         // Switched once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, false>>(doom, linedef);
  case 114: case 116: case 42: case 115: case 63: case 61:          // Switched repeatable
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, true>>(doom, linedef);
  case 2: case 3: case 110: case 108: case 109: case 16:            // Walkover once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, false>>(doom, linedef);
  case 4:                                                           // Walkover once (monster)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, false, DOOM::EnumLinedef::Target::TargetPlayerMonster>>(doom, linedef);
  case 106: case 107: case 75: case 105: case 86: case 90: case 76: // Walkover repeatable
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, true>>(doom, linedef);

    // Regular locked door types
  case 32:  // Pushed once (blue)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerPushed, false, DOOM::EnumLinedef::Target::TargetPlayer, DOOM::Enum::KeyColor::KeyColorBlue>>(doom, linedef);
  case 33:  // Pushed once (red)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerPushed, false, DOOM::EnumLinedef::Target::TargetPlayer, DOOM::Enum::KeyColor::KeyColorRed>>(doom, linedef);
  case 34:  // Pushed once (yellow)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerPushed, false, DOOM::EnumLinedef::Target::TargetPlayer, DOOM::Enum::KeyColor::KeyColorYellow>>(doom, linedef);
  case 26:  // Pushed repeatable (blue)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerPushed, true, DOOM::EnumLinedef::Target::TargetPlayer, DOOM::Enum::KeyColor::KeyColorBlue>>(doom, linedef);
  case 27:  // Pushed repeatable (yellow)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerPushed, true, DOOM::EnumLinedef::Target::TargetPlayer, DOOM::Enum::KeyColor::KeyColorYellow>>(doom, linedef);
  case 28:  // Pushed repeatable (red)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerPushed, true, DOOM::EnumLinedef::Target::TargetPlayer, DOOM::Enum::KeyColor::KeyColorRed>>(doom, linedef);
  case 133: // Switched once (blue)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, false, DOOM::EnumLinedef::Target::TargetPlayer, DOOM::Enum::KeyColor::KeyColorBlue>>(doom, linedef);
  case 135: // Switched once (red)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, false, DOOM::EnumLinedef::Target::TargetPlayer, DOOM::Enum::KeyColor::KeyColorRed>>(doom, linedef);
  case 137: // Switched once (yellow)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, false, DOOM::EnumLinedef::Target::TargetPlayer, DOOM::Enum::KeyColor::KeyColorYellow>>(doom, linedef);
  case 99:  // Switched repeatable (blue)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, true, DOOM::EnumLinedef::Target::TargetPlayer, DOOM::Enum::KeyColor::KeyColorBlue>>(doom, linedef);
  case 134: // Switched repeatable (red)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, true, DOOM::EnumLinedef::Target::TargetPlayer, DOOM::Enum::KeyColor::KeyColorRed>>(doom, linedef);
  case 136: // Switched repeatable (yellow)
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, true, DOOM::EnumLinedef::Target::TargetPlayer, DOOM::Enum::KeyColor::KeyColorYellow>>(doom, linedef);

    // Regular floor types
  case 82: case 83: case 84: case 91: case 92: case 93: case 96: case 98: case 128: case 129:         // Walkover repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, true>>(doom, linedef);
  case 5: case 19: case 30: case 36: case 37: case 38: case 56: case 58: case 59: case 119: case 130: // Walkover once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, false>>(doom, linedef);
  case 45: case 64: case 65: case 69: case 70: case 60: case 132:                                     // Switched repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, true>>(doom, linedef);
  case 18: case 23: case 55: case 71: case 101: case 102: case 131: case 140:                         // Switched once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, false>>(doom, linedef);
  case 24:                                                                                            // Gunfire once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerGunfire, false>>(doom, linedef);

    // Regular ceiling types
  case 72:          // Walkover repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, true>>(doom, linedef);
  case 40: case 44: // Walkover once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, false>>(doom, linedef);
  case 43:          // Switched repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, true>>(doom, linedef);
  case 41:          // Switched once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, false>>(doom, linedef);

    // Regular platform lift
  case 87: case 88: case 95: case 120:          // Walkover repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, true>>(doom, linedef);
  case 10: case 22: case 53: case 121:          // Walkover once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, false>>(doom, linedef);
  case 62: case 66: case 67: case 68: case 123: // Switched repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, true>>(doom, linedef);
  case 14: case 15: case 20: case 21: case 122: // Switched once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, false>>(doom, linedef);
  case 47:                                      // Gunfire once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerGunfire, false>>(doom, linedef);

    // Regular crusher
  case 73: case 77:           // Walkover repeat
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, true>>(doom, linedef);
  case 6: case 25: case 141:  // Walkover once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, false>>(doom, linedef);
  case 49:                    // Switched once
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerSwitched, false>>(doom, linedef);

    // Stop lift or crusher
  case 74: case 89: // Walkover repeat
    return std::make_unique<DOOM::StopTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, true>>(doom, linedef);
  case 57: case 54: // Walkover once
    return std::make_unique<DOOM::StopTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Leveling, DOOM::EnumLinedef::Trigger::TriggerWalkover, false>>(doom, linedef);

    // Regular lighting
  case 12:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::LightMaximum, DOOM::EnumLinedef::Trigger::TriggerWalkover, false>>(doom, linedef);
  case 13:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::Light255, DOOM::EnumLinedef::Trigger::TriggerWalkover, false>>(doom, linedef);
  case 17:
    return std::make_unique<DOOM::ActionTriggerableLinedef<DOOM::Doom::Level::Sector::Action::Lighting, DOOM::EnumLinedef::Trigger::TriggerWalkover, false>>(doom, linedef);
  case 35:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::Light35, DOOM::EnumLinedef::Trigger::TriggerWalkover, false>>(doom, linedef);
  case 79:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::Light35, DOOM::EnumLinedef::Trigger::TriggerWalkover, true>>(doom, linedef);
  case 80:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::LightMaximum, DOOM::EnumLinedef::Trigger::TriggerWalkover, true>>(doom, linedef);
  case 81:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::Light255, DOOM::EnumLinedef::Trigger::TriggerWalkover, true>>(doom, linedef);
  case 104:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::LightMinimum, DOOM::EnumLinedef::Trigger::TriggerWalkover, false>>(doom, linedef);
  case 138:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::Light255, DOOM::EnumLinedef::Trigger::TriggerSwitched, true>>(doom, linedef);
  case 139:
    return std::make_unique<DOOM::LightTriggerableLinedef<DOOM::EnumLinedef::Light::Light35, DOOM::EnumLinedef::Trigger::TriggerSwitched, true>>(doom, linedef);

    // Regular stair builders
  case 7:   // Switched once, slow, 8 step
    return std::make_unique<DOOM::StairTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumAction::Speed::SpeedSlower, 8>>(doom, linedef);
  case 8:   // Walkover once, slow, 8 step
    return std::make_unique<DOOM::StairTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumAction::Speed::SpeedSlower, 8>>(doom, linedef);
  case 100: // Walkover once, fast, 16 step
    return std::make_unique<DOOM::StairTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, DOOM::EnumAction::Speed::SpeedFaster, 16>>(doom, linedef);
  case 127: // Switched once, fast, 16 step
    return std::make_unique<DOOM::StairTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, DOOM::EnumAction::Speed::SpeedFaster, 16>>(doom, linedef);

    // Regular scroller
  case 48:
    return std::make_unique<DOOM::ScrollerLinedef<>>(doom, linedef);

    // Regular donut
  case 9:
    return std::make_unique<DOOM::DonutTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerSwitched, false>>(doom, linedef);

    // Teleporter
  case 39:  // Walkover once
    return std::make_unique<DOOM::TeleporterTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, false, DOOM::EnumLinedef::Target::TargetPlayerMonster>>(doom, linedef);
  case 97:  // Walkover repeat
    return std::make_unique<DOOM::TeleporterTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, true, DOOM::EnumLinedef::Target::TargetPlayerMonster>>(doom, linedef);
  case 125: // Walkover once, monters only
    return std::make_unique<DOOM::TeleporterTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, false, DOOM::EnumLinedef::Target::TargetMonster>>(doom, linedef);
  case 126: // Walkover repeat, monters only
    return std::make_unique<DOOM::TeleporterTriggerableLinedef<DOOM::EnumLinedef::Trigger::TriggerWalkover, true, DOOM::EnumLinedef::Target::TargetMonster>>(doom, linedef);

    // Exit
  case 11:  // Switched, normal
    return std::make_unique<DOOM::ExitLinedef<DOOM::EnumLinedef::Exit::ExitNormal, DOOM::EnumLinedef::Trigger::TriggerSwitched>>(doom, linedef);
  case 52:  // Walkover, normal
    return std::make_unique<DOOM::ExitLinedef<DOOM::EnumLinedef::Exit::ExitNormal, DOOM::EnumLinedef::Trigger::TriggerWalkover>>(doom, linedef);
  case 197: // Gunfire, normal
    return std::make_unique<DOOM::ExitLinedef<DOOM::EnumLinedef::Exit::ExitNormal, DOOM::EnumLinedef::Trigger::TriggerGunfire>>(doom, linedef);
  case 51:  // Switched, secret
    return std::make_unique<DOOM::ExitLinedef<DOOM::EnumLinedef::Exit::ExitSecret, DOOM::EnumLinedef::Trigger::TriggerSwitched, true>>(doom, linedef);
  case 124: // Walkover, secret
    return std::make_unique<DOOM::ExitLinedef<DOOM::EnumLinedef::Exit::ExitSecret, DOOM::EnumLinedef::Trigger::TriggerWalkover, true>>(doom, linedef);
  case 198: // Gunfire, secret
    return std::make_unique<DOOM::ExitLinedef<DOOM::EnumLinedef::Exit::ExitSecret, DOOM::EnumLinedef::Trigger::TriggerGunfire, true>>(doom, linedef);

  default:  // Error
    std::cout << "[AbstractLinedef::factory] Warning, unknown type '" << linedef.type << "'." << std::endl;
    return std::make_unique<DOOM::NullLinedef>(doom, linedef);
  }
}

bool  DOOM::AbstractLinedef::switched(DOOM::Doom& doom, DOOM::AbstractThing& thing)
{
  return false;
}

bool  DOOM::AbstractLinedef::walkover(DOOM::Doom& doom, DOOM::AbstractThing& thing)
{
  return false;
}

bool  DOOM::AbstractLinedef::gunfire(DOOM::Doom& doom, DOOM::AbstractThing& thing)
{
  return false;
}
