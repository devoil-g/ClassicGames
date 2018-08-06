#ifndef _DONUT_TRIGGERABLE_LINEDEF_HPP_
#define _DONUT_TRIGGERABLE_LINEDEF_HPP_

#include <iostream>

#include "Doom/Linedef/AbstractTriggerableLinedef.hpp"
#include "Doom/Action/FloorLevelingAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumLinedef::Trigger Trigger,
    DOOM::EnumLinedef::Repeat Repeat,
    DOOM::EnumAction::Speed Speed = DOOM::EnumAction::Speed::SpeedSlow
  >
  class DonutTriggerableLinedef : public DOOM::AbstractTriggerableLinedef<Trigger, DOOM::EnumLinedef::Repeat::RepeatFalse>
  {
  private:
    inline void	triggerDonut(DOOM::Doom & doom, int16_t pillar_index)	// Build stairs from sector
    {
      // Find pool sector
      int16_t	pool_index = -1;

      for (const std::unique_ptr<DOOM::AbstractLinedef> & linedef : doom.level.linedefs) {
	if (linedef->front != -1 && doom.level.sidedefs[linedef->front].sector == pillar_index &&
	  linedef->back != -1) {
	  pool_index = doom.level.sidedefs[linedef->back].sector;
	  break;
	}
	else if (linedef->back != -1 && doom.level.sidedefs[linedef->back].sector == pillar_index &&
	  linedef->front != -1) {
	  pool_index = doom.level.sidedefs[linedef->front].sector;
	  break;
	}
      }

      // Cancel if pool not found
      if (pool_index == -1) {
	std::cerr << "[DOOM::DonutTriggerableLinedef] Warning, pool not found." << std::endl;
	return;
      }

      // Find model sector
      int16_t	model_index = -1;

      for (const std::unique_ptr<DOOM::AbstractLinedef> & linedef : doom.level.linedefs) {
	if (linedef->front != -1 && doom.level.sidedefs[linedef->front].sector == pool_index &&
	  linedef->back != -1 && doom.level.sidedefs[linedef->back].sector != pillar_index) {
	  model_index = doom.level.sidedefs[linedef->back].sector;
	  break;
	}
	else if (linedef->back != -1 && doom.level.sidedefs[linedef->back].sector == pool_index &&
	  linedef->front != -1 && doom.level.sidedefs[linedef->front].sector != pillar_index) {
	  model_index = doom.level.sidedefs[linedef->front].sector;
	  break;
	}
      }

      // Cancel if model not found
      if (model_index == -1) {
	std::cerr << "[DOOM::DonutTriggerableLinedef] Warning, model not found." << std::endl;
	return;
      }

      std::reference_wrapper<DOOM::Doom::Level::Sector>	pillar = std::ref(doom.level.sectors[pillar_index]);
      std::reference_wrapper<DOOM::Doom::Level::Sector>	pool = std::ref(doom.level.sectors[pool_index]);
      std::reference_wrapper<DOOM::Doom::Level::Sector>	model = std::ref(doom.level.sectors[model_index]);

      // Raise/lower pillar
      if (pillar.get().floor_base > model.get().floor_base)
	pillar.get().action<DOOM::EnumAction::Type::TypeLeveling>(std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionDown, Speed>>(doom, model.get().floor_base));
      else
	pillar.get().action<DOOM::EnumAction::Type::TypeLeveling>(std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, Speed>>(doom, model.get().floor_base));

      // Raise/lower pool
      if (pool.get().floor_base > model.get().floor_base)
	pool.get().action<DOOM::EnumAction::Type::TypeLeveling>(std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionDown, Speed, DOOM::EnumAction::Change::ChangeTexture>>(doom, model.get().floor_base, model_index));
      else
	pool.get().action<DOOM::EnumAction::Type::TypeLeveling>(std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, Speed, DOOM::EnumAction::Change::ChangeTexture>>(doom, model.get().floor_base, model_index));
    }

    void	trigger(DOOM::Doom & doom, DOOM::AbstractThing & thing) override	// Trigger stair builder on tagged event
    {
      // Handle error
      if (tag == 0)
      {
	std::cerr << "[DonutTriggerableLinedef::trigger] Warning, invalid pushed tag (" << tag << ")." << std::endl;
	return;
      }

      // Trigger tagged sectors
      for (unsigned int sector_index = 0; sector_index < doom.level.sectors.size(); sector_index++)
	if (doom.level.sectors[sector_index].tag == tag)
	  triggerDonut(doom, sector_index);
    }

  public:
    DonutTriggerableLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
      DOOM::AbstractTriggerableLinedef<Trigger, DOOM::EnumLinedef::Repeat::RepeatFalse>(doom, linedef)
    {}

    ~DonutTriggerableLinedef() = default;
  };
};

#endif