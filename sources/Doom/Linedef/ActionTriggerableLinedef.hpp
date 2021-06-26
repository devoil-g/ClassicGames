#pragma once

#include "Doom/Linedef/AbstractTriggerableLinedef.hpp"

namespace DOOM
{
  template<
    DOOM::Doom::Level::Sector::Action Type,
    DOOM::EnumLinedef::Trigger Trigger,
    bool Repeat,
    DOOM::EnumLinedef::Target Target = DOOM::EnumLinedef::Target::TargetPlayer,
    DOOM::Enum::KeyColor Key = DOOM::Enum::KeyColor::KeyColorNone
  >
    class ActionTriggerableLinedef : public DOOM::AbstractTriggerableLinedef<Trigger, Repeat, Target, Key>
  {
  private:
    bool  trigger(DOOM::Doom& doom, DOOM::AbstractThing& thing, int16_t sector_index) override  // Action of the linedef
    {
      std::unique_ptr<DOOM::AbstractAction>& action = doom.level.sectors[sector_index].action<Type>();

      // Push action in sector or trigger current action
      if (action.get() == nullptr) {
        doom.level.sectors[sector_index].action<Type>(doom, DOOM::AbstractLinedef::type, doom.level.sidedefs[DOOM::AbstractLinedef::front].sector);
        return true;
      }
      else if (Trigger == DOOM::EnumLinedef::Trigger::TriggerPushed)
        return action->start(doom, thing);
      else
        return false;
    }

  public:
    ActionTriggerableLinedef(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Linedef& linedef) :
      DOOM::AbstractTriggerableLinedef<Trigger, Repeat, Target, Key>(doom, linedef)
    {}

    ~ActionTriggerableLinedef() = default;
  };
}