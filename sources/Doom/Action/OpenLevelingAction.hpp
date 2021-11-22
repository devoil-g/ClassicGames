#pragma once

#include "Doom/Action/AbstractLevelingAction.hpp"

namespace DOOM
{
  template<DOOM::EnumAction::Speed Speed>
    class OpenLevelingAction : public DOOM::AbstractLevelingAction<>
  {
  private:
    const float _target_floor;    // Floor target height
    const float _target_ceiling;  // Ceiling target height

  public:
    OpenLevelingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float target_floor, float target_ceiling) :
      DOOM::AbstractLevelingAction<>(doom, sector),
      _target_floor(target_floor), _target_ceiling(target_ceiling)
    {
      // Ceiling sound
      sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_stnmov, true);
    }

    ~OpenLevelingAction() override = default;

    void  update(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed) override  // Update door action
    {
      // Update floor and ceiling height
      updateFloorLower(doom, sector, elapsed, _target_floor, (float)Speed / 8.f);
      updateCeilingRaise(doom, sector, elapsed, _target_ceiling, (float)Speed / 8.f);

      // Detect end of action
      if (sector.floor_current == _target_floor && sector.ceiling_current == _target_ceiling) {
        sector.ceiling_base = sector.ceiling_current;
        sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);
        remove(doom, sector);
      }
    }
  };
}