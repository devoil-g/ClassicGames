#include "Doom/Linedef/AbstractLinedef.hpp"
#include "Doom/Linedef/NormalLinedef.hpp"
#include "Doom/Linedef/TriggerableLinedef.hpp"

DOOM::AbstractLinedef::AbstractLinedef(const DOOM::Wad::RawLevel::Linedef & linedef) :
  start(linedef.start), end(linedef.end),
  flag(linedef.flag),
  type(linedef.type),
  tag(linedef.tag),
  front(linedef.front), back(linedef.back)
{}

DOOM::AbstractLinedef::~AbstractLinedef()
{}

DOOM::AbstractLinedef *	DOOM::AbstractLinedef::factory(DOOM::Doom & doom, DOOM::Wad::RawLevel::Linedef const & linedef)
{
  switch (linedef.type)
  {
  case 0:	// Normal linedef
    return new DOOM::NormalLinedef(doom, linedef);
  
    // Regular and extended door types
  case 46:	// Gunfire repeatable
    return new DOOM::TriggerableLinedef<DOOM::TriggerableLinedef<>::EnumTrigger::TriggerGunfire, true>(doom, linedef);
  case 31: case 118:	// Pushed once
    return new DOOM::TriggerableLinedef<DOOM::TriggerableLinedef<>::EnumTrigger::TriggerPushed, false>(doom, linedef);
  case 1: case 117:	// Pushed repeatable
    return new DOOM::TriggerableLinedef<DOOM::TriggerableLinedef<>::EnumTrigger::TriggerPushed, true>(doom, linedef);
  case 103: case 50: case 175: case 113: case 112: case 111: case 29:	// Switched once
    return new DOOM::TriggerableLinedef<DOOM::TriggerableLinedef<>::EnumTrigger::TriggerSwitched, false>(doom, linedef);
  case 114: case 116: case 42: case 115: case 196: case 63: case 61:	// Switched repeatable
    return new DOOM::TriggerableLinedef<DOOM::TriggerableLinedef<>::EnumTrigger::TriggerSwitched, true>(doom, linedef);
  case 2: case 3: case 4: case 110: case 108: case 109: case 16:	// Walkover once
    return new DOOM::TriggerableLinedef<DOOM::TriggerableLinedef<>::EnumTrigger::TriggerSwitched, false>(doom, linedef);
  case 106: case 107: case 75: case 105: case 86: case 90: case 76:	// Walkover repeatable
    return new DOOM::TriggerableLinedef<DOOM::TriggerableLinedef<>::EnumTrigger::TriggerSwitched, true>(doom, linedef);

    // Regular and extended locked door types
  case 34: case 33: case 32:	// Pushed once
    return new DOOM::TriggerableLinedef<DOOM::TriggerableLinedef<>::EnumTrigger::TriggerPushed, false>(doom, linedef);
  case 28: case 27: case 26:	// Pushed repeatable
    return new DOOM::TriggerableLinedef<DOOM::TriggerableLinedef<>::EnumTrigger::TriggerPushed, true>(doom, linedef);
  case 133: case 135: case 137:	// Switched once
    return new DOOM::TriggerableLinedef<DOOM::TriggerableLinedef<>::EnumTrigger::TriggerSwitched, false>(doom, linedef);
  case 99: case 134: case 136:	// Switched repeatable
    return new DOOM::TriggerableLinedef<DOOM::TriggerableLinedef<>::EnumTrigger::TriggerSwitched, true>(doom, linedef);
  
  default:	// TODO: return nullptr for error
    return new DOOM::NormalLinedef(doom, linedef);
  }
}

void	DOOM::AbstractLinedef::pushed(const DOOM::AbstractThing &)
{}

void	DOOM::AbstractLinedef::switched(const DOOM::AbstractThing &)
{}

void	DOOM::AbstractLinedef::walkover(const DOOM::AbstractThing &)
{}

void	DOOM::AbstractLinedef::gunfire(const DOOM::AbstractThing &)
{}