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
      Lower,  // Lower the crusher
      Crush   // Crush things bellow
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
      float tallest = std::numeric_limits<float>::lowest();
      auto  things = doom.level.getThings(sector, DOOM::Enum::ThingProperty::ThingProperty_Shootable);

      // Find tallest thing in sector
      for (const auto& thing : things)
        tallest = std::max(tallest, (float)thing.get().height);

      // Lower crusher
      sector.ceiling_current -= elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds();

      // Stop before crushing things
      if (sector.ceiling_current - sector.floor_base <= tallest) {
        sf::Time  exceding = std::min(sf::seconds((sector.floor_base + tallest - sector.ceiling_current) / Speed * DOOM::Doom::Tic.asSeconds()), elapsed);

        sector.ceiling_current = sector.floor_base + tallest;
        _state = State::Crush;

        // Lower things collinding with ceiling
        for (const auto& thing : things)
          thing.get().position.z() = std::max(std::min(thing.get().position.z(), sector.ceiling_current - thing.get().height), sector.floor_base);

        return exceding;
      }

      // Lower things collinding with ceiling
      for (const auto& thing : things)
        thing.get().position.z() = std::max(std::min(thing.get().position.z(), sector.ceiling_current - thing.get().height), sector.floor_base);

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

    sf::Time  updateCrush(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed)
    {
      float tallest = std::numeric_limits<float>::lowest();
      auto  things = doom.level.getThings(sector, DOOM::Enum::ThingProperty::ThingProperty_Shootable);

      // Find tallest thing in sector
      for (const auto& thing : things)
        tallest = std::max(tallest, (float)thing.get().height);

      // Nothing to crush, return to normal state
      if (sector.ceiling_current > sector.floor_base + tallest) {
        _state = State::Lower;
        return elapsed;
      }

      // Lower crusher
      float speed = Speed / (Speed == DOOM::EnumAction::Speed::SpeedSlow ? 8.f : 1.f);
      sector.ceiling_current -= elapsed.asSeconds() * speed / DOOM::Doom::Tic.asSeconds();

      // Lower things collinding with ceiling
      for (const auto& thing : things)
        thing.get().position.z() = std::max(std::min(thing.get().position.z(), sector.ceiling_current - thing.get().height), sector.floor_base);

      // Damage things
      for (const auto& thing : things) {
        float distance = std::min(sector.floor_base + thing.get().height, sector.ceiling_current + elapsed.asSeconds() * speed / DOOM::Doom::Tic.asSeconds()) - std::max(sector.floor_base + 8.f, sector.ceiling_current);
        float damage = distance / speed * 2.5f;

        thing.get().damage(doom, damage);
      }

      // Compute exceding time
      if (sector.ceiling_current < sector.floor_base + 8.f) {
        sf::Time  exceding = std::min(sf::seconds((sector.floor_base + 8.f - sector.ceiling_current) / (Speed / (Speed == DOOM::EnumAction::Speed::SpeedSlow ? 8.f : 1.f)) * DOOM::Doom::Tic.asSeconds()), elapsed);

        sector.ceiling_current = sector.floor_base + 8.f;
        _state = State::Raise;
        if (Silent == true)
          sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);

        return exceding;
      }
      else
        return sf::Time::Zero;
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
        case State::Crush:
          elapsed = updateCrush(doom, sector, elapsed);
          break;

        default:  // Handle error (should not happen)
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
        }
    }
  };
}