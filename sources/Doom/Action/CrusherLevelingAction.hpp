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
      auto  things = doom.level.getThings(sector, DOOM::Enum::ThingProperty::ThingProperty_Shootable);
      float obstacle = std::numeric_limits<float>::lowest();
      float start = sector.ceiling_current;
      float end = std::max(sector.floor_current + 8.f, sector.ceiling_current - elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds());

      // Get highest obstacle
      for (const auto& thing : things) {
        float highest_floor = std::numeric_limits<float>::lowest();

        // Find highest floor under thing
        for (int sector_index : doom.level.getSectors(thing.get().position.convert<2>(), thing.get().attributs.radius / 2.f))
          highest_floor = std::max(highest_floor, doom.level.sectors[sector_index].floor_current);

        obstacle = std::min(start, std::max(obstacle, highest_floor + thing.get().height));
      }

      // Stop at highest obstacle
      end = std::min(start, std::max(end, obstacle));

      // Lower things collinding with ceiling
      for (const auto& thing : things)
        thing.get().position.z() = std::min(thing.get().position.z(), end - thing.get().height);

      sf::Time  exceding = DOOM::Doom::Tic * (start - end) / (float)Speed;

      if (exceding != sf::Time::Zero)
      {
        // Obstacle collided
        if (obstacle > end) {
          _state = State::Crush;
        }

        // Floor reached
        else {
          _state = State::Raise;
          if (Silent == true)
            sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);
        }
      }

      // Update crusher height
      sector.ceiling_current = end;

      return exceding;
    }

    sf::Time  updateCrush(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed)
    {
      auto  things = doom.level.getThings(sector, DOOM::Enum::ThingProperty::ThingProperty_Shootable);
      float obstacle = std::numeric_limits<float>::lowest();

      // Check that there is something to crush
      for (const auto& thing : things) {
        float highest_floor = std::numeric_limits<float>::lowest();

        // Find highest floor under thing
        for (int sector_index : doom.level.getSectors(thing.get().position.convert<2>(), thing.get().attributs.radius / 2.f))
          highest_floor = std::max(highest_floor, doom.level.sectors[sector_index].floor_current);

        obstacle = std::max(obstacle, highest_floor + thing.get().height);
      }
      
      // Cancel if there is nothing to crush
      if (sector.ceiling_current < obstacle) {
        _state = State::Lower;
        return elapsed;
      }

      float speed = Speed / (Speed == DOOM::EnumAction::Speed::SpeedSlow ? 8.f : 1.f);
      float start = sector.ceiling_current;
      float end = std::max(sector.floor_current + 8.f, sector.ceiling_current - elapsed.asSeconds() * speed / DOOM::Doom::Tic.asSeconds());

      // Crush obstacles
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

      // Update ceiling height
      sector.ceiling_current = end;

      // Compute exceding time
      if (sector.ceiling_current <= sector.floor_current + 8.f) {
        _state = State::Raise;
        if (Silent == true)
          sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);

        return sf::seconds(elapsed.asSeconds() - (start - end) / speed * DOOM::Doom::Tic.asSeconds());
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