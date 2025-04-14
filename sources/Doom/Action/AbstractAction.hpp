#pragma once

#include <cstdint>
#include <memory>

#include "Doom/Doom.hpp"

namespace DOOM
{
  namespace EnumAction
  {
    enum Speed
    {
      SpeedSlowest = 1,   // Move 1/8 units/tic
      SpeedSlower = 2,    // Move 1/4 units/tic
      SpeedSlow = 4,      // Move 1/2 units/tic
      SpeedNormal = 8,    // Move 1 units/tic
      SpeedFast = 16,     // Move 2 units/tic
      SpeedFaster = 32,   // Move 4 units/tic
      SpeedFastest = 64   // Move 8 units/tic
    };

    enum Direction
    {
      DirectionUp,  // Move upward
      DirectionDown // Move downward
    };
  };

  class AbstractAction
  {
  public:
    static std::unique_ptr<DOOM::AbstractAction>  factory(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, std::int16_t type, std::int16_t model = -1); // Factory of sector action build from type

  protected:
    virtual void  remove(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector) = 0;  // Remove action from sector

  public:
    AbstractAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector);
    virtual ~AbstractAction() = default;

    virtual void  update(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float elapsed) = 0; // Update sector's action

    virtual bool  stop(DOOM::Doom& doom, DOOM::AbstractThing& thing);   // Request action to stop (for lift & crusher), return true if an action has been stopped
    virtual bool  start(DOOM::Doom& doom, DOOM::AbstractThing& thing);  // Request action to start or re-trigger (for door, lift & crusher), return true if an action has been started
  };
};