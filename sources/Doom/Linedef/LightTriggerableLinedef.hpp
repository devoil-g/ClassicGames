#pragma once

#include "Doom/Linedef/AbstractTriggerableLinedef.hpp"

namespace DOOM
{
  namespace EnumLinedef
  {
    enum Light
    {
      Light35,
      Light255,
      LightMinimum,
      LightMaximum
    };
  };

  template<
    DOOM::EnumLinedef::Light Light,
    DOOM::EnumLinedef::Trigger Trigger,
    bool Repeat
  >
    class LightTriggerableLinedef : public DOOM::AbstractTriggerableLinedef<Trigger, Repeat>
  {
  private:
    bool  trigger(DOOM::Doom& doom, DOOM::AbstractThing& thing, int16_t sector_index) override  // Perform light change on sector
    {
      DOOM::Doom::Level::Sector& sector = doom.level.sectors[sector_index];

      // Cancel if a lighting action is already running
      if (sector.action<DOOM::Doom::Level::Sector::Action::Lighting>().get() != nullptr)
        return false;

      int16_t	light;

      // Get new light level of sector
      switch (Light)
      {
      case DOOM::EnumLinedef::Light::Light35:
        light = 35;
        break;
      case DOOM::EnumLinedef::Light::Light255:
        light = 255;
        break;
      case DOOM::EnumLinedef::Light::LightMinimum:
        light = sector.getNeighborLowestLight(doom);
        break;
      case DOOM::EnumLinedef::Light::LightMaximum:
        light = sector.getNeighborHighestLight(doom);
        break;
      }

      // Set new light level of sector
      sector.light_base = light;
      sector.light_current = light;

      return true;
    }

  public:
    LightTriggerableLinedef(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Linedef& linedef) :
      DOOM::AbstractTriggerableLinedef<Trigger, Repeat>(doom, linedef)
    {}

    ~LightTriggerableLinedef() = default;
  };
}