#pragma once

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumAction::Direction Direction,
    DOOM::EnumAction::Speed Speed,
    DOOM::EnumAction::Change::Type ChangeType = DOOM::EnumAction::Change::Type::None,
    DOOM::EnumAction::Change::Time ChangeTime = DOOM::EnumAction::Change::Time::Before,
    bool Crush = false
  >
    class FloorLevelingAction : public DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>
  {
  private:
    const float _target;  // Floor target height

  public:
    FloorLevelingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float target, int16_t model = -1) :
      DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>(doom, sector, model),
      _target(target)
    {
      // Floor sound
      sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_stnmov, true);
    }

    ~FloorLevelingAction() override = default;

    void  update(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed) override  // Update door action
    {
      auto  things = doom.level.getThings(sector, DOOM::Enum::ThingProperty::ThingProperty_Shootable);

      // Compute start and end height
      float start = sector.floor_current;
      float end = (Direction == DOOM::EnumAction::Direction::DirectionUp) ?
        std::min(sector.floor_current + Speed / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds(), _target) :
        std::max(sector.floor_current - Speed / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds(), _target);

      // Check for collision for raising floor
      if (Direction == DOOM::EnumAction::Direction::DirectionUp) {
        float tallest = std::numeric_limits<float>::lowest();
        
        // Find tallest thing in sector
        for (const auto& thing : things)
          tallest = std::max(tallest, (float)thing.get().height);

        // Crush obstacles
        if (Crush == true) {
          for (const auto& thing : things) {
            float distance = std::min(end + thing.get().height - sector.ceiling_base, end - start);

            if (distance > 0.f)
              thing.get().damage(doom, distance / Speed * 2.5f);
          }
        }

        // Stop on obstacles
        else {
          end = std::max(sector.floor_current, std::min(end, sector.ceiling_base - tallest));
        }

        // Raise things standing on the floor
        for (const auto& thing : things)
          thing.get().position.z() = std::max(thing.get().position.z(), end);
      }

      // Lower things standing on the floor
      else {
        for (const auto& thing : things)
          if (!(thing.get().flags & DOOM::Enum::ThingProperty::ThingProperty_Float) && thing.get().position.z() == start)
            thing.get().position.z() = end;
      }

      // Update floor height
      sector.floor_current = end;

      // Detect end of action
      if (sector.floor_current == _target) {
        sector.floor_base = sector.floor_current;
        sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);
        remove(doom, sector);
      }
    }
  };
}