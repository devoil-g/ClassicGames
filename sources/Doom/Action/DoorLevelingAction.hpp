#pragma once

#include <list>

#include <SFML/System/Time.hpp>

#include "Doom/Action/AbstractLevelingAction.hpp"

namespace DOOM
{
  namespace EnumAction
  {
    enum Door
    {
      DoorWaitOpen,       // Open the door
      DoorWaitClose,      // Close the door
      DoorOpenWaitClose,  // Open, wait then close the door
      DoorCloseWaitOpen   // Close, wait then re-open to door
    };
  };

  template<
    DOOM::EnumAction::Door Door,
    DOOM::EnumAction::Speed Speed = DOOM::EnumAction::Speed::SpeedNormal,
    unsigned int TickWait = 140,
    unsigned int TickForceWait = 140
  >
    class DoorLevelingAction : public DOOM::AbstractLevelingAction<>
  {
  private:
    enum State
    {
      Noop,       // Does nothing (dummy state for sound)
      Open,       // Open the door
      Close,      // Close the door
      ForceClose, // Close the door without bouncing on thing
      Wait,       // Wait for the specified time
      ForceWait   // Wait for a fixed time
    };

    std::list<State>  _states;  // Door states to perform
    sf::Time          _elapsed; // Time elapsed since begining of state

    void      updateSound(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector)  // Play state sound
    {
      // Does nothing when no states
      if (_states.empty() == true)
        return;

      // Select sound to play
      switch (_states.front()) {
      case State::Open:
        sound(doom, sector, (Speed > DOOM::EnumAction::Speed::SpeedFast) ? DOOM::Doom::Resources::Sound::EnumSound::Sound_bdopn : DOOM::Doom::Resources::Sound::EnumSound::Sound_doropn);
        break;
      case State::Close:
      case State::ForceClose:
        sound(doom, sector, (Speed > DOOM::EnumAction::Speed::SpeedFast) ? DOOM::Doom::Resources::Sound::EnumSound::Sound_bdcls : DOOM::Doom::Resources::Sound::EnumSound::Sound_dorcls);
        break;
      default:
        return;
      }
    }

  public:
    DoorLevelingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector) :
      DOOM::AbstractLevelingAction<>(doom, sector),
      _states(),
      _elapsed(sf::Time::Zero)
    {
      // Map of states according to door type (initial Noop to force sound)
      static const std::unordered_map<DOOM::EnumAction::Door, std::list<State>> states = {
        { DOOM::EnumAction::Door::DoorOpenWaitClose, { State::Noop, State::Open, State::Wait, State::Close } },
        { DOOM::EnumAction::Door::DoorCloseWaitOpen, { State::Noop, State::Close, State::Wait, State::Open } },
        { DOOM::EnumAction::Door::DoorWaitOpen, { State::Noop, State::Wait, State::Open } },
        { DOOM::EnumAction::Door::DoorWaitClose, { State::Noop, State::Wait, State::ForceClose } }
      };

      auto  iterator = states.find(Door);

      // Check for errors
      if (iterator != states.end())
        _states = iterator->second;
      else
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }

    ~DoorLevelingAction() override = default;

    void  update(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed) override  // Update door action
    {
      // Update action states
      while (_states.empty() == false) {
        switch (_states.front())
        {
        case State::Open:
          // Raise door
          elapsed = updateCeilingRaise(doom, sector, elapsed, sector.getNeighborLowestCeiling(doom).second - 4.f, (float)Speed / 8.f);
          break;

        case State::Close:
          // Lower door
          elapsed = updateCeilingLower(doom, sector, elapsed, sector.floor_base, (float)Speed / 8.f);

          // Obstacles
          if (elapsed > sf::Time::Zero && sector.ceiling_current != sector.floor_base) {
            switch (Door) {
            case DOOM::EnumAction::Door::DoorOpenWaitClose:
              _states = { State::Noop, State::Open, State::ForceWait, State::Close };
              break;
            case DOOM::EnumAction::Door::DoorCloseWaitOpen:
              _states = { State::Noop, State::Open, State::ForceWait, State::Close, State::Wait, State::Open };
              break;
            default:
              throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
            }
          }
          break;

        case State::ForceClose:
          // Lower door
          elapsed = updateCeilingLower(doom, sector, elapsed, sector.floor_base, (float)Speed / 8.f);

          // Stop on obstacles
          if (sector.ceiling_current != sector.floor_base)
            elapsed = sf::Time::Zero;
          break;

        case State::Wait:
          _elapsed += elapsed;

          // Get remaining time if any
          elapsed = std::max(sf::Time::Zero, _elapsed - (DOOM::Doom::Tic * (float)TickWait));
          break;

        case State::ForceWait:
          _elapsed += elapsed;

          // Get remaining time if any
          elapsed = std::max(sf::Time::Zero, _elapsed - (DOOM::Doom::Tic * (float)TickForceWait));
          break;

        case State::Noop:
          // Does nothing
          break;

        default:  // Handle error (should not happen)
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
        }

        // Stop if no more elapsed time
        if (elapsed == sf::Time::Zero)
          break;

        // Pop ended state
        _states.pop_front();
        _elapsed = sf::Time::Zero;

        // Play new state sound
        updateSound(doom, sector);
      }

      // Update ceiling base value when animation ended and remove action from sector
      if (_states.empty() == true) {
        sector.ceiling_base = sector.ceiling_current;
        remove(doom, sector);
      }
    }

    bool  start(DOOM::Doom& doom, DOOM::AbstractThing& thing) override  // Request door to re-trigger
    {
      if (thing.type != DOOM::Enum::ThingType::ThingType_PLAYER)
        return true;

      switch (Door) {
      case DOOM::EnumAction::Door::DoorOpenWaitClose:
        switch (_states.front()) {
        case State::Open:
          _states = { State::Noop, State::Close };
          return true;
        case State::Close: case State::ForceClose:
          _states = { State::Noop, State::Open, State::Wait, State::Close };
          return true;
        case State::Wait: case State::ForceWait:
          _states.front() = State::Noop;
          return true;
        }
        return false;
      case DOOM::EnumAction::Door::DoorCloseWaitOpen:
        switch (_states.front()) {
        case State::Open:
          _states = { State::Noop, State::Close, State::Wait, State::Open };
          return true;
        case State::Close: case State::ForceClose:
          _states = { State::Noop, State::Open };
          return true;
        case State::Wait: case State::ForceWait:
          _states.front() = State::Noop;
          return true;
        }
        return false;
      case DOOM::EnumAction::Door::DoorWaitOpen:
        return false;
      case DOOM::EnumAction::Door::DoorWaitClose:
        return false;
      default:
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      }
    }
  };
}