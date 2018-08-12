#ifndef _ACTION_TRIGGERABLE_LINEDEF_HPP_
#define _ACTION_TRIGGERABLE_LINEDEF_HPP_

#include <iostream>

#include "Doom/Linedef/AbstractTriggerableLinedef.hpp"

namespace DOOM
{
  template<
    DOOM::Doom::Level::Sector::Action Type,
    DOOM::EnumLinedef::Trigger Trigger,
    bool Repeat,
    bool Monster = false,
    DOOM::EnumLinedef::Key Key = DOOM::EnumLinedef::Key::KeyNone
  >
  class ActionTriggerableLinedef : public DOOM::AbstractTriggerableLinedef<Trigger, Repeat, Monster, Key>
  {
  private:
    void	trigger(DOOM::Doom & doom, DOOM::AbstractThing & thing, int16_t sector_index) override	// Action of the linedef
    {
      std::unique_ptr<DOOM::AbstractAction> &	action = doom.level.sectors[sector_index].action<Type>();

      // Push action in sector or trigger current action
      if (action.get() == nullptr)
	doom.level.sectors[sector_index].action<Type>(doom, type, doom.level.sidedefs[front].sector);
      else if (Trigger == DOOM::EnumLinedef::Trigger::TriggerPushed)
	action->start(doom, thing);
    }

  public:
    ActionTriggerableLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
      DOOM::AbstractTriggerableLinedef<Trigger, Repeat, Monster, Key>(doom, linedef)
    {}

    ~ActionTriggerableLinedef() = default;
  };
};

#endif