#pragma once

#include <iostream>

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
    class StopTriggerableLinedef : public DOOM::AbstractTriggerableLinedef<Trigger, Repeat, Target, Key>
  {
  private:
    bool  trigger(DOOM::Doom& doom, DOOM::AbstractThing& thing, int16_t sector_index) override  // Action of the linedef
    {
      std::unique_ptr<DOOM::AbstractAction>& action = doom.level.sectors[sector_index].action<Type>();

      // Request stop of sector action
      if (action.get() != nullptr)
        return action->stop(doom, thing);
      else
        return false;
    }

  public:
    StopTriggerableLinedef(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Linedef& linedef) :
      DOOM::AbstractTriggerableLinedef<Trigger, Repeat, Target, Key>(doom, linedef)
    {}

    ~StopTriggerableLinedef() = default;
  };
}