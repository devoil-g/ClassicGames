#ifndef _STOP_TRIGGERABLE_LINEDEF_HPP_
#define _STOP_TRIGGERABLE_LINEDEF_HPP_

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
  class StopTriggerableLinedef : public DOOM::AbstractTriggerableLinedef<Trigger, Repeat, Monster, Key>
  {
  private:
    template<DOOM::EnumLinedef::Trigger _Trigger = DOOM::EnumLinedef::Trigger::TriggerPushed>
    inline std::enable_if_t<(Trigger & _Trigger) != 0>	triggerSector(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Trigger sector of second sidedef
    {
      // Handle invalid sidedef index
      if (back == -1)
	return;

      const std::unique_ptr<DOOM::AbstractAction> &	action = doom.level.sectors[doom.level.sidedefs[back].sector].action<Type>();

      // Trigger sector of second sidedef
      if (action.get() != nullptr)
	action->stop();
    }

    template<DOOM::EnumLinedef::Trigger _Trigger = DOOM::EnumLinedef::Trigger::TriggerPushed>
    inline std::enable_if_t<(Trigger & _Trigger) == 0>	triggerSector(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Trigger tagged sectors
    {
      // Handle invalid tag
      if (tag == 0)
	return;

      // Trigger tagged sectors
      for (DOOM::Doom::Level::Sector & sector : doom.level.sectors)
	if (sector.tag == tag && sector.action<Type>().get() != nullptr)
	  sector.action<Type>()->stop();
    }

    void	trigger(DOOM::Doom & doom, DOOM::AbstractThing & thing) override	// Action of the linedef
    {
      // Push action in sector(s)
      triggerSector(doom, thing);
    }

  public:
    StopTriggerableLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
      DOOM::AbstractTriggerableLinedef<Trigger, Repeat, Monster, Key>(doom, linedef)
    {
      // Check for inconstitency
      if (Trigger & DOOM::EnumLinedef::Trigger::TriggerPushed) {
	if (back == -1)
	  std::cerr << "[StopTriggerableLinedef:constructor] Warning, no sector for action (type #" << type << ")." << std::endl;
	if (tag != 0)
	  std::cerr << "[StopTriggerableLinedef:constructor] Warning, useless tag for sector action (type #" << type << ")." << std::endl;
      }
      else {
	if (tag == 0)
	  std::cerr << "[StopTriggerableLinedef:constructor] Warning, no tag for sector action (type #" << type << ")." << std::endl;
      }
    }

    ~StopTriggerableLinedef() = default;
  };
};

#endif