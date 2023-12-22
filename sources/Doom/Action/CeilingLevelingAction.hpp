#pragma once

#include "Doom/Action/AbstractLevelingAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumAction::Direction Direction,
    DOOM::EnumAction::Speed Speed,
    bool Crush = false
  >
    class CeilingLevelingAction : public DOOM::AbstractLevelingAction<>
  {
  private:
    const float _target;  // Ceiling target height

  public:
    CeilingLevelingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float target) :
      DOOM::AbstractLevelingAction<>(doom, sector),
      _target(target)
    {
      // Ceiling sound
      sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_stnmov, true);
    }

    ~CeilingLevelingAction() override = default;

    void  update(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float elapsed) override  // Update door action
    {
      // Lower ceiling
      if (Direction == DOOM::EnumAction::Direction::DirectionDown) {
        float original = elapsed;

        elapsed = updateCeilingLower(doom, sector, elapsed, _target, (float)Speed / 8.f);

        // Crush things
        if (Crush == true && elapsed > 0.f)
          elapsed = updateCeilingCrush(doom, sector, elapsed, _target, (float)Speed / 8.f);

        // Collision, stop sound
        if (elapsed > 0.f && sector.ceiling_current != _target)
          _sound.sound.pause();

        // Moving, play sound
        else if (elapsed != original && _sound.sound.getStatus() == sf::SoundSource::Status::Paused)
          _sound.sound.play();
      }

      // Raise ceiling
      else
        updateCeilingRaise(doom, sector, elapsed, _target, (float)Speed / 8.f);

      // Detect end of action
      if (sector.ceiling_current == _target) {
        sector.ceiling_base = sector.ceiling_current;
        sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);
        remove(doom, sector);
      }
    }
  };
}