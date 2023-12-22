#pragma once

#include "Doom/Action/AbstractLevelingAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumAction::Speed Speed,
    DOOM::EnumAction::Change::Type ChangeType = DOOM::EnumAction::Change::Type::None,
    DOOM::EnumAction::Change::Time ChangeTime = DOOM::EnumAction::Change::Time::Before
  >
    class PlatformLevelingAction : public DOOM::AbstractLevelingAction<false, ChangeType, ChangeTime>
  {
  private:
    enum State
    {
      Raise,  // Raise the platform
      Lower   // Lower the platform
    };

    float _target;  // Floor target height
    State _state;   // Platform current state
    
  public:
    PlatformLevelingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float target, int16_t model = -1) :
      DOOM::AbstractLevelingAction<false, ChangeType, ChangeTime>(doom, sector, model),
      _target(target),
      _state(State::Raise)
    {
      // Platform sound
      DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>::sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_stnmov);
    }

    ~PlatformLevelingAction() override = default;

    void  update(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float elapsed) override  // Update door action
    {
      // Update action states
      while (elapsed != 0.f) {
        switch (_state)
        {
        case State::Raise:
          // Raise floor
          elapsed = DOOM::AbstractLevelingAction<false, ChangeType, ChangeTime>::updateFloorRaise(doom, sector, elapsed, _target, (float)Speed / 8.f);

          // End of state
          if (sector.floor_current == _target) {
            sector.floor_base = sector.floor_current;
            DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>::sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);
            DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>::remove(doom, sector);
            return;
          }

          // Obstacle
          else if (elapsed > 0.f) {
            _target = sector.floor_base;
            _state = State::Lower;
            DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>::sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstart);
          }
          break;

        case State::Lower:
          // Lower floor
          elapsed = DOOM::AbstractLevelingAction<false, ChangeType, ChangeTime>::updateFloorLower(doom, sector, elapsed, _target, (float)Speed / 8.f);

          // End of state
          if (sector.floor_current == _target) {
            sector.floor_base = sector.floor_current;
            DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>::sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);
            DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>::remove(doom, sector);
            return;
          }
          break;

        default:  // Handle error (should not happen)
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
        }
      }
    }
  };
}