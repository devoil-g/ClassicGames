#pragma once

#include <iostream>

#include "Doom/Linedef/AbstractTriggerableLinedef.hpp"
#include "Doom/Action/FloorLevelingAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumLinedef::Trigger Trigger,
    bool Repeat,
    DOOM::EnumAction::Speed Speed = DOOM::EnumAction::Speed::SpeedNormal
  >
    class DonutTriggerableLinedef : public DOOM::AbstractTriggerableLinedef<Trigger, Repeat>
  {
  private:
    inline bool trigger(DOOM::Doom& doom, DOOM::AbstractThing& thing, int16_t pillar_index) // Perform donut action
    {
      // Find pool sector
      int16_t pool_index = -1;

      for (const std::unique_ptr<DOOM::AbstractLinedef>& linedef : doom.level.linedefs) {
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
        return false;
      }

      // Find model sector
      int16_t model_index = -1;

      for (const std::unique_ptr<DOOM::AbstractLinedef>& linedef : doom.level.linedefs) {
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
        return false;
      }

      std::reference_wrapper<DOOM::Doom::Level::Sector> pillar = std::ref(doom.level.sectors[pillar_index]);
      std::reference_wrapper<DOOM::Doom::Level::Sector> pool = std::ref(doom.level.sectors[pool_index]);
      std::reference_wrapper<DOOM::Doom::Level::Sector> model = std::ref(doom.level.sectors[model_index]);

      // Raise/lower pillar
      if (pillar.get().floor_base > model.get().floor_base)
        pillar.get().action<DOOM::Doom::Level::Sector::Action::Leveling>(std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionDown, Speed>>(doom, pillar, model.get().floor_base));
      else
        pillar.get().action<DOOM::Doom::Level::Sector::Action::Leveling>(std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, Speed>>(doom, pillar, model.get().floor_base));

      // Raise/lower pool
      if (pool.get().floor_base > model.get().floor_base)
        pool.get().action<DOOM::Doom::Level::Sector::Action::Leveling>(std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionDown, Speed, DOOM::EnumAction::Change::Type::Texture, DOOM::EnumAction::Change::Time::Before>>(doom, pool, model.get().floor_base, model_index));
      else
        pool.get().action<DOOM::Doom::Level::Sector::Action::Leveling>(std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, Speed, DOOM::EnumAction::Change::Type::Texture, DOOM::EnumAction::Change::Time::After>>(doom, pool, model.get().floor_base, model_index));

      return true;
    }

  public:
    DonutTriggerableLinedef(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Linedef& linedef) :
      DOOM::AbstractTriggerableLinedef<Trigger, Repeat>(doom, linedef)
    {}

    ~DonutTriggerableLinedef() = default;
  };
}