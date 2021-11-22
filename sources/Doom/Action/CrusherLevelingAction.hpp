#pragma once

#include "Doom/Action/AbstractLevelingAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumAction::Speed Speed,
    bool Silent = false
  >
    class CrusherLevelingAction : public DOOM::AbstractLevelingAction<true>
  {
  private:
    enum State
    {
      Raise,  // Raise the crusher
      Lower   // Lower the crusher
    };

    State _state; // Crusher current state

  public:
    CrusherLevelingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector) :
      DOOM::AbstractLevelingAction<true>(doom, sector),
      _state(State::Lower)
    {
      // Crusher sound
      if (Silent == false)
        sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_stnmov, true);
    }

    ~CrusherLevelingAction() override = default;

    void  update(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed) override  // Update door action
    {
      // Don't perform action if not running
      if (_run == false)
        return;

      // Update action states
      while (elapsed != sf::Time::Zero) {
        switch (_state) {
        case State::Raise:
          // Raise ceiling
          elapsed = updateCeilingRaise(doom, sector, elapsed, sector.ceiling_base, (float)Speed / 8.f);

          // End of state
          if (elapsed > sf::Time::Zero) {
            _state = State::Lower;
            if (Silent == true)
              sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);
          }

          break;
        case State::Lower:
          // Lower ceiling
          elapsed = updateCeilingLower(doom, sector, elapsed, sector.floor_base + 8.f, (float)Speed / 8.f);

          // Crush things
          if (elapsed > sf::Time::Zero)
            elapsed = updateCeilingCrush(doom, sector, elapsed, sector.floor_base + 8.f, (float)Speed / 8.f / (Speed <= DOOM::EnumAction::Speed::SpeedNormal ? 8.f : 1.f));

          // End of state
          if (elapsed > sf::Time::Zero) {
            _state = State::Raise;
            if (Silent == true)
              sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);
          }

          break;

        default:  // Handle error (should not happen)
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
        }
      }
    }
  };
}