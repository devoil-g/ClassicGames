#pragma once

#include "Doom/Linedef/AbstractTriggerableLinedef.hpp"
#include "Doom/Action/FloorLevelingAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumLinedef::Trigger Trigger,
    DOOM::EnumAction::Speed Speed,
    unsigned int Step
  >
    class StairTriggerableLinedef : public DOOM::AbstractTriggerableLinedef<Trigger, false>
  {
  private:
    inline bool trigger(DOOM::Doom& doom, DOOM::AbstractThing& thing, int16_t sector_index) // Build stairs from sector
    {
      // Does nothing if action already running
      if (doom.level.sectors[sector_index].action<DOOM::Doom::Level::Sector::Action::Leveling>().get() != nullptr)
        return false;

      int16_t step_index = sector_index;
      float   step_height = doom.level.sectors[sector_index].floor_base;

      // Start stair building from tagged sector
      do
      {
        // Increment step height
        step_height += Step;

        // Start step rising
        doom.level.sectors[step_index].action<DOOM::Doom::Level::Sector::Action::Leveling>(std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, Speed>>(doom, doom.level.sectors[step_index], (float)step_height));

        int16_t	step_new = -1;
        for (const std::unique_ptr<DOOM::AbstractLinedef>& linedef : doom.level.linedefs)
          if (linedef->front != -1 && linedef->back != -1 &&
            doom.level.sidedefs[linedef->front].sector == step_index &&
            doom.level.sectors[doom.level.sidedefs[linedef->back].sector].action<DOOM::Doom::Level::Sector::Action::Leveling>().get() == nullptr &&
            doom.level.sectors[doom.level.sidedefs[linedef->front].sector].floor_name == doom.level.sectors[doom.level.sidedefs[linedef->back].sector].floor_name)
          {
            step_new = doom.level.sidedefs[linedef->back].sector;
            break;
          }

        // Continue stair building if new step found
        if (step_new != -1)
          step_index = step_new;

        // No next step found, interrupt
        else
          return true;

      } while (true);
    }

  public:
    StairTriggerableLinedef(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Linedef& linedef) :
      DOOM::AbstractTriggerableLinedef<Trigger, false>(doom, linedef)
    {}

    ~StairTriggerableLinedef() = default;
  };
}