#pragma once

#include <list>

#include "Doom/Action/AbstractLevelingAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumAction::Speed Speed,
    bool Repeat = false,
    unsigned int TickWait = 105
  >
    class LiftLevelingAction : public DOOM::AbstractLevelingAction<true>
  {
  private:
    enum State
    {
      Raise,  // Raise the lift
      Lower,  // Lower the lift
      Wait,   // Wait for the specified time
    };

    const float _low, _high;  // Low and high height of lift
    State       _state;       // Lift current state
    float       _elapsed;     // Elapsed time since beginning of state

  public:
    LiftLevelingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float low, float high) :
      DOOM::AbstractLevelingAction<true>(doom, sector),
      _low(low),
      _high(high),
      _state(State::Lower),
      _elapsed(0.f)
    {
      sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstart);
    }

    ~LiftLevelingAction() override = default;

    void  update(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float elapsed) override  // Update door action
    {
      if (Repeat == true && _run == false)
        return;

      // Update action states
      while (elapsed != 0.f) {
        switch (_state)
        {
        case State::Raise:
          // Raise floor
          elapsed = updateFloorRaise(doom, sector, elapsed, _high, (float)Speed / 8.f);

          // Reached top
          if (sector.floor_current == _high) {
            sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);
            _state = State::Wait;

            // Stop if not repeatable
            if (Repeat == false) {
              remove(doom, sector);
              return;
            }
          }

          // Obstacle
          else if (elapsed != 0.f) {
            _state = State::Lower;
            sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstart);
          }
          break;

        case State::Lower:
          // Lower floor
          elapsed = updateFloorLower(doom, sector, elapsed, _low, (float)Speed / 8.f);

          // Reached bottom
          if (sector.floor_current == _low) {
            sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);
            _state = State::Wait;
          }
          break;

        case State::Wait:
          // Add elapsed time to counter
          _elapsed += elapsed;

          // Compute remaining time if any
          elapsed = std::max(0.f, _elapsed - (DOOM::Doom::Tic * (float)TickWait));

          // Switch to next state if wait finished
          if (elapsed != 0.f) {
            _state = (sector.floor_current == _low ? State::Raise : State::Lower);
            sound(doom, sector, DOOM::Doom::Resources::Sound::EnumSound::Sound_pstart);
          }
          break;

        default:  // Handle error (should not happen)
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
        }

        // Reset internal elapsed time between states
        if (elapsed != 0.f)
          _elapsed = 0.f;
      }
    }
  };
}