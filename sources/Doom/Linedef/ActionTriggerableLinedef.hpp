#ifndef _ACTION_TRIGGERABLE_LINEDEF_HPP_
#define _ACTION_TRIGGERABLE_LINEDEF_HPP_

#include <iostream>

#include "Doom/Linedef/AbstractTriggerableLinedef.hpp"

namespace DOOM
{
  template<
    DOOM::EnumAction::Type Type,
    DOOM::EnumLinedef::Trigger Trigger,
    DOOM::EnumLinedef::Repeat Repeat,
    DOOM::EnumLinedef::Monster Monster = DOOM::EnumLinedef::Monster::MonsterFalse,
    DOOM::EnumLinedef::Key Key = DOOM::EnumLinedef::Key::KeyNone
  >
  class ActionTriggerableLinedef : public DOOM::AbstractTriggerableLinedef<Trigger, Repeat, Monster, Key>
  {
  private:
    template<DOOM::EnumLinedef::Trigger _Trigger = DOOM::EnumLinedef::Trigger::TriggerPushed>
    inline std::enable_if_t<(Trigger & _Trigger) != 0>	triggerSector(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Trigger sector of second sidedef
    {
      // Handle invalid sidedef index
      if (back == -1)
	return;

      // Trigger sector of second sidedef
      doom.level.sectors[doom.level.sidedefs[back].sector].action<Type>(doom, type);
    }

    template<DOOM::EnumLinedef::Trigger _Trigger = DOOM::EnumLinedef::Trigger::TriggerPushed>
    inline std::enable_if_t<(Trigger & _Trigger) == 0>	triggerSector(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Trigger tagged sectors
    {
      // Handle invalid tag
      if (tag == 0)
	return;
      
      // Trigger tagged sectors
      for (DOOM::Doom::Level::Sector & sector : doom.level.sectors)
	if (sector.tag == tag)
	  sector.action<Type>(doom, type, doom.level.sidedefs[front].sector);
    }

    void	trigger(DOOM::Doom & doom, DOOM::AbstractThing & thing) override	// Action of the linedef
    {
      // Push action in sector(s)
      triggerSector(doom, thing);
    }

  public:
    ActionTriggerableLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
      DOOM::AbstractTriggerableLinedef<Trigger, Repeat, Monster, Key>(doom, linedef)
    {
      // Check for inconstitency
      if (Trigger & DOOM::EnumLinedef::Trigger::TriggerPushed) {
	if (back == -1)
	  std::cerr << "[ActionTriggerableLinedef:constructor] Warning, no sector for action (type #" << type << ")." << std::endl;
	if (tag != 0)
	  std::cerr << "[ActionTriggerableLinedef:constructor] Warning, useless tag for sector action (type #" << type << ")." << std::endl;
      }
      else {
	if (tag == 0)
	  std::cerr << "[ActionTriggerableLinedef:constructor] Warning, no tag for sector action (type #" << type << ")." << std::endl;
      }
    }

    ~ActionTriggerableLinedef() = default;
  };
};

#endif