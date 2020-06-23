#pragma once

#include "Doom/Action/AbstractStoppableAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumAction::Speed Speed,
    bool Silent = false
  >
    class CrusherLevelingAction : public DOOM::AbstractStoppableAction<DOOM::Doom::Level::Sector::Action::Leveling>
  {
  private:
    enum State
    {
      Raise,  // Raise the crusher
      Lower   // Lower the crusher
    };

    State _state; // Crusher current state

    sf::Time  updateRaise(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed)
    {
      // Raise crusher
      sector.ceiling_current += elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds();

      // Compute exceding time
      if (sector.ceiling_current > sector.ceiling_base) {
        sf::Time  exceding = std::min(sf::seconds((sector.ceiling_current - sector.ceiling_base) / Speed * DOOM::Doom::Tic.asSeconds()), elapsed);

        sector.ceiling_current = sector.ceiling_base;
        _state = State::Lower;
        if (Silent == true)
          sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);

        return exceding;
      }
      else {
        return sf::Time::Zero;
      }
    }

    sf::Time  updateLower(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed)
    {
      // Lower crusher
      sector.ceiling_current -= elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds();

      // TODO: handle collision

      // Compute exceding time
      if (sector.ceiling_current < sector.floor_base + 8.f) {
        sf::Time  exceding = std::min(sf::seconds((sector.floor_base + 8.f - sector.ceiling_current) / Speed * DOOM::Doom::Tic.asSeconds()), elapsed);

        sector.ceiling_current = sector.floor_base + 8.f;
        _state = State::Raise;
        if (Silent == true)
          sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);

        return exceding;
      }
      else {
        return sf::Time::Zero;
      }
    }

  public:
    CrusherLevelingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector) :
      DOOM::AbstractStoppableAction<DOOM::Doom::Level::Sector::Action::Leveling>(doom, sector),
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
      if (_run == false) {
        return;
      }

      // Update action states
      while (elapsed != sf::Time::Zero)
        switch (_state) {
        case State::Raise:
          elapsed = updateRaise(doom, sector, elapsed);
          break;
        case State::Lower:
          elapsed = updateLower(doom, sector, elapsed);
          break;

        default:  // Handle error (should not happen)
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
        }
    }
  };
}