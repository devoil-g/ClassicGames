#pragma once

#include "Doom/Linedef/AbstractLinedef.hpp"
#include "Doom/Linedef/AbstractTriggerableLinedef.hpp"

#include <iostream>

namespace DOOM
{
  namespace EnumLinedef
  {
    enum Exit
    {
      ExitNormal,
      ExitSecret
    };
  };

  template<
    DOOM::EnumLinedef::Exit Exit,
    DOOM::EnumLinedef::Trigger Trigger
  >
  class ExitLinedef : public DOOM::AbstractLinedef
  {
  private:
    template<DOOM::EnumLinedef::Trigger _Trigger>
    inline std::enable_if_t<(Trigger& _Trigger) == 0, bool> trigger(DOOM::Doom& doom, DOOM::AbstractThing& thing) // Does nothing if wrong event triggered
    {
      return false;
    }

    template<DOOM::EnumLinedef::Trigger _Trigger>
    inline std::enable_if_t<(Trigger& _Trigger) != 0, bool> trigger(DOOM::Doom& doom, DOOM::AbstractThing& thing) // Trigger event if correct event triggered
    {
      // Check if thing is a player
      if (thing.type != DOOM::Enum::ThingType::ThingType_PLAYER)
	return false;

      // TODO: push level transition state
      std::cout << "[DOOM::ExitLinedef] Exit level (not implemented)." << std::endl;

      // Find which sidedef textures should be switched
      int16_t	sidedef = ((Math::Vector<2>::determinant(doom.level.vertexes[start] - thing.position.convert<2>(), doom.level.vertexes[end] - doom.level.vertexes[start]) < 0.f) ? front : back);

      // Switch sidedef if trigger pushed or switched, definitively if not repeatable
      if (_Trigger == DOOM::EnumLinedef::Trigger::TriggerPushed || _Trigger == DOOM::EnumLinedef::Trigger::TriggerSwitched && sidedef != -1)
	doom.level.sidedefs[sidedef].switched(doom, sf::Time::Zero);

      return true;
    }

  public:
    ExitLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
      DOOM::AbstractLinedef(doom, linedef)
    {}

    ~ExitLinedef() = default;

    virtual void  update(DOOM::Doom& doom, sf::Time elapsed) override // Update linedef
    {}

    virtual bool  switched(DOOM::Doom& doom, DOOM::AbstractThing& thing) override // To call when linedef is switched (used) by thing
    {
      return trigger<DOOM::EnumLinedef::Trigger::TriggerPushed>(doom, thing) ||
	trigger<DOOM::EnumLinedef::Trigger::TriggerSwitched>(doom, thing);
    }

    virtual bool  walkover(DOOM::Doom& doom, DOOM::AbstractThing& thing) override // To call when thing walk over the linedef
    {
      return trigger<DOOM::EnumLinedef::Trigger::TriggerWalkover>(doom, thing);
    }

    virtual bool  gunfire(DOOM::Doom& doom, DOOM::AbstractThing& thing) override  // To call when thing shot the linedef
    {
      return trigger<DOOM::EnumLinedef::Trigger::TriggerGunfire>(doom, thing);
    }
  };
};