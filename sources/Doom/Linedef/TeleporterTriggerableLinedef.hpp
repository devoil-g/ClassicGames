#ifndef _TELEPORTER_TRIGGERABLE_LINEDEF_HPP_
#define _TELEPORTER_TRIGGERABLE_LINEDEF_HPP_

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
    inline bool	trigger(DOOM::Doom & doom, DOOM::AbstractThing & thing, int16_t sector_index)	// Teleport thing to sector
    {
      DOOM::Doom::Level::Vertex	linedef_start = doom.level.vertexes[start];
      DOOM::Doom::Level::Vertex	linedef_end = doom.level.vertexes[end];

      // Ignore teleportation if coming from the wrong direction
      if (Math::Vector<2>::determinant(linedef_end.convert<2>() - linedef_start.convert<2>(), thing.position.convert<2>() - linedef_start.convert<2>()) > 0.f)
	return false;

      // Search for teleporter target in sector
      for (const std::unique_ptr<DOOM::AbstractThing> & sector_thing : doom.level.things) {
	// TODO: do not hardcode 14 for teleporter type
	if (sector_thing->attributs.id == 14 && doom.level.getSector(sector_thing->position.convert<2>()).first == sector_index) {
	  // TODO: telefrag monster on landing position
	  thing.teleport(doom, sector_thing->position.convert<2>(), sector_thing->angle);
	  return true;
	}
      }

      // Failed to find teleporter landing
      return false;
    }

  public:
    TeleporterTriggerableLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
      DOOM::AbstractTriggerableLinedef<Trigger, Repeat, Target>(doom, linedef)
    {}

    ~TeleporterTriggerableLinedef() = default;
  };
};

#endif