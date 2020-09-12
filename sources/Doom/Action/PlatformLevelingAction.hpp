#pragma once

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumAction::Speed Speed,
    DOOM::EnumAction::Change::Type ChangeType = DOOM::EnumAction::Change::Type::None,
    DOOM::EnumAction::Change::Time ChangeTime = DOOM::EnumAction::Change::Time::Before
  >
    class PlatformLevelingAction : public DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>
  {
  private:
    enum State
    {
      Raise,  // Raise the platform
      Lower,  // Lower the platform
      Stop    // Stop platform
    };

    float _target;  // Floor target height
    State _state;   // Platform current state
    
    sf::Time  updateRaise(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed)
    {
      float obstacle = std::numeric_limits<float>::max();

      // Get lowest obstacle
      // TODO: is shootable enough to identify a monster ?
      for (const std::reference_wrapper<DOOM::AbstractThing>& thing : doom.level.getThings(sector, DOOM::Enum::ThingProperty::ThingProperty_Shootable))
        for (int16_t sector_index : doom.level.getSectors(thing.get().position.convert<2>(), thing.get().attributs.radius / 2.f))
          obstacle = std::min(obstacle, doom.level.sectors[sector_index].ceiling_current - thing.get().height);

      // Handle collision with obstacle, changing target height to original
      if (sector.floor_current + elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds() > obstacle) {
        if (sector.floor_current > obstacle) {
          _target = sector.floor_base;
          _state = State::Lower;
          sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstart);
          return elapsed;
        }
        else {
          sf::Time  exceding = std::min(sf::seconds((obstacle - sector.floor_current) / Speed * DOOM::Doom::Tic.asSeconds()), elapsed);

          sector.floor_current = obstacle;
          _target = sector.floor_base;
          _state = State::Lower;
          sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstart);
          return exceding;
        }
      }

      // Raise door
      sector.floor_current += elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds();

      // Compute exceding time
      if (sector.floor_current > _target) {
        sf::Time  exceding = std::min(sf::seconds((sector.floor_current - _target) / Speed * DOOM::Doom::Tic.asSeconds()), elapsed);

        sector.floor_current = _target;
        _state = State::Stop;
        sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);
        return exceding;
      }
      else {
        return sf::Time::Zero;
      }
    }

    sf::Time  updateLower(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed)
    {
      sf::Time  remaining = sf::Time::Zero;

      // Lower door
      float floor_new = sector.floor_current - elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds();

      // Compute exceding time
      if (floor_new < _target) {
        remaining = std::min(sf::seconds((_target - floor_new) / Speed * DOOM::Doom::Tic.asSeconds()), elapsed);
        floor_new = _target;
        _state = State::Stop;
        sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);
      }

      // Lower things that stand on the ground of the sector
      // TODO: is Shootable enough to identify a monster ?
      for (const std::reference_wrapper<DOOM::AbstractThing>& thing : doom.level.getThings(sector, DOOM::Enum::ThingProperty::ThingProperty_Shootable))
        if (!(thing.get().attributs.properties & DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling) && thing.get().position.z() == sector.floor_current) {
          float floor_next = std::numeric_limits<float>::lowest();

          // Find next lower floor
          for (int16_t sector_index : doom.level.getSectors(thing.get().position.convert<2>(), thing.get().attributs.radius / 2.f)) {
            const DOOM::Doom::Level::Sector& sector_current = doom.level.sectors[sector_index];

            if (&sector_current != &sector)
              floor_next = std::max(floor_next, sector_current.floor_current);
          }

          // Move thing to new floor position
          thing.get().position.z() = std::max(floor_next, floor_new);
        }

      sector.floor_current = floor_new;
      return remaining;
    }

    sf::Time  updateStop(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed)
    {
      // Set new floor base value
      sector.floor_base = sector.floor_current;

      // Remove sector action
      remove(doom, sector);

      return sf::Time::Zero;
    }

  public:
    PlatformLevelingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float target, int16_t model = -1) :
      DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>(doom, sector, model),
      _target(target),
      _state(State::Raise)
    {
      // Platform sound
      sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_stnmov);
    }

    ~PlatformLevelingAction() override = default;

    void  update(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed) override  // Update door action
    {
      // Update action states
      while (elapsed != sf::Time::Zero) {
        switch (_state)
        {
        case State::Raise:
          elapsed = updateRaise(doom, sector, elapsed);
          break;
        case State::Lower:
          elapsed = updateLower(doom, sector, elapsed);
          break;
        case State::Stop:
          elapsed = updateStop(doom, sector, elapsed);
          break;

        default:  // Handle error (should not happen)
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
        }
      }
    }
  };
}