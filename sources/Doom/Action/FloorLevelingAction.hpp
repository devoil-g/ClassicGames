#pragma once

#include "Doom/Action/AbstractLevelingAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumAction::Direction Direction,
    DOOM::EnumAction::Speed Speed,
    DOOM::EnumAction::Change::Type ChangeType = DOOM::EnumAction::Change::Type::None,
    DOOM::EnumAction::Change::Time ChangeTime = DOOM::EnumAction::Change::Time::Before,
    bool Crush = false
  >
    class FloorLevelingAction : public DOOM::AbstractLevelingAction<false, ChangeType, ChangeTime>
  {
  private:
    const float _target;  // Floor target height

  public:
    FloorLevelingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float target, int16_t model = -1) :
      DOOM::AbstractLevelingAction<false, ChangeType, ChangeTime>(doom, sector, model),
      _target(target)
    {
      // Floor sound
      DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>::sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_stnmov, true);
    }

    ~FloorLevelingAction() override = default;

    void  update(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed) override  // Update door action
    {
      // Lower ceiling
      if (Direction == DOOM::EnumAction::Direction::DirectionUp) {
        sf::Time  original = elapsed;

        elapsed = DOOM::AbstractLevelingAction<false, ChangeType, ChangeTime>::updateFloorRaise(doom, sector, elapsed, _target, (float)Speed / 8.f);

        // Crush things
        if (Crush == true && elapsed > sf::Time::Zero)
          elapsed = DOOM::AbstractLevelingAction<false, ChangeType, ChangeTime>::updateFloorCrush(doom, sector, elapsed, _target, (float)Speed / 8.f);

        // Collision, stop sound
        if (elapsed > sf::Time::Zero && sector.floor_current != _target)
          DOOM::AbstractLevelingAction<false, ChangeType, ChangeTime>::_sound.sound.pause();

        // Moving, play sound
        else if (elapsed != original && DOOM::AbstractLevelingAction<false, ChangeType, ChangeTime>::_sound.sound.getStatus() == sf::SoundSource::Status::Paused)
          DOOM::AbstractLevelingAction<false, ChangeType, ChangeTime>::_sound.sound.play();
      }

      // Raise ceiling
      else
        DOOM::AbstractLevelingAction<false, ChangeType, ChangeTime>::updateFloorLower(doom, sector, elapsed, _target, (float)Speed / 8.f);

      // Detect end of action
      if (sector.floor_current == _target) {
        sector.floor_base = sector.floor_current;
        DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>::sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);
        DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>::remove(doom, sector);
      }
    }
  };
}