#pragma once

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumAction::Direction Direction,
    DOOM::EnumAction::Speed Speed,
    bool Crush = false
  >
    class CeilingLevelingAction : public DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling>
  {
  private:
    const float _target;  // Ceiling target height

  public:
    CeilingLevelingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float target) :
      DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling>(doom, sector),
      _target(target)
    {
      // Ceiling sound
      sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_stnmov, true);
    }

    ~CeilingLevelingAction() override = default;

    void  update(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed) override  // Update door action
    {
      auto  things = doom.level.getThings(sector, DOOM::Enum::ThingProperty::ThingProperty_Shootable);

      // Compute start and end height
      float start = sector.ceiling_current;
      float end = (Direction == DOOM::EnumAction::Direction::DirectionUp) ?
        std::min(sector.ceiling_current + Speed / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds(), _target) :
        std::max(sector.ceiling_current - Speed / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds(), _target);

      // Check for collision for lowering ceiling
      if (Direction == DOOM::EnumAction::Direction::DirectionDown)
      {
        // Crush obstacles
        if (Crush == true) {
          for (const auto& thing : things) {
            float highest_floor = std::numeric_limits<float>::lowest();

            // Find highest floor under thing
            for (int sector_index : doom.level.getSectors(thing.get().position.convert<2>(), thing.get().attributs.radius / 2.f))
              highest_floor = std::max(highest_floor, doom.level.sectors[sector_index].floor_current);

            // Push toward the floor
            thing.get().position.z() = std::max(highest_floor, std::min(thing.get().position.z(), end - thing.get().height));
            
            float crush_height = std::min(thing.get().position.z() + thing.get().height, start) - end;

            // Crush!
            if (crush_height > 0.f)
              thing.get().damage(doom, crush_height / Speed * 2.5f);
          }
        }

        // Stop on obstacles
        else {
          for (const auto& thing : things) {
            float highest_floor = thing.get().position.z();

            // Find highest floor under thing
            for (int sector_index : doom.level.getSectors(thing.get().position.convert<2>(), thing.get().attributs.radius / 2.f))
              highest_floor = std::max(highest_floor, doom.level.sectors[sector_index].floor_current);

            // Stop at highest obstacle
            end = std::min(start, std::max(end, highest_floor + thing.get().height));
          }
        }
      }

      // Update ceiling height
      sector.ceiling_current = end;

      // Detect end of action
      if (sector.ceiling_current == _target) {
        sector.ceiling_base = sector.ceiling_current;
        remove(doom, sector);
      }
    }
  };
}