#pragma once

#include <cstdint>

#include "Doom/Linedef/AbstractTriggerableLinedef.hpp"
#include "Doom/Thing/AbstractThing.hpp"

namespace DOOM
{
  template<
    DOOM::EnumLinedef::Trigger Trigger,
    bool Repeat,
    DOOM::EnumLinedef::Target Target
  >
    class TeleporterTriggerableLinedef : public DOOM::AbstractTriggerableLinedef<Trigger, Repeat, Target>
  {
  private:
    inline bool trigger(DOOM::Doom& doom, DOOM::AbstractThing& thing, std::int16_t sector_index) // Teleport thing to sector
    {
      // Missiles doesn't teleport
      if (thing.flags & DOOM::Enum::ThingProperty::ThingProperty_Missile)
        return false;

      auto& linedef_start = doom.level.vertexes[DOOM::AbstractLinedef::start];
      auto& linedef_end = doom.level.vertexes[DOOM::AbstractLinedef::end];

      // Ignore teleportation if coming from the wrong direction
      if (Math::Vector<2>::determinant(linedef_end.convert<2>() - linedef_start.convert<2>(), thing.position.convert<2>() - linedef_start.convert<2>()) > 0.f)
        return false;

      // Search for teleporter target in sector
      for (const auto& sector_thing : doom.level.things) {
        if (sector_thing->type == DOOM::Enum::ThingType::ThingType_TELEPORTMAN && doom.level.getSector(sector_thing->position.convert<2>()).first == sector_index) {
          return thing.teleport(doom, sector_thing->position.convert<2>(), sector_thing->angle, thing.type == DOOM::Enum::ThingType::ThingType_PLAYER);
        }
      }

      // Failed to find teleporter landing
      return false;
    }

  public:
    TeleporterTriggerableLinedef(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Linedef& linedef) :
      DOOM::AbstractTriggerableLinedef<Trigger, Repeat, Target>(doom, linedef)
    {}

    ~TeleporterTriggerableLinedef() = default;
  };
}