#ifndef _ABSTRACT_ACTION_HPP_
#define _ABSTRACT_ACTION_HPP_

#include "Doom/Doom.hpp"

namespace DOOM
{
  namespace EnumAction
  {
    enum Speed
    {
      SpeedSlow = 1,	// Move 1 units/tic
      SpeedNormal = 2,	// Move 2 units/tic
      SpeedFast = 4,	// Move 4 units/tic
      SpeedTurbo = 8	// Move 8 units/tic
    };

    enum Direction
    {
      DirectionUp,	// Move upward
      DirectionDown	// Move downward
    };
  };

  class AbstractAction
  {
  public:
    static std::unique_ptr<DOOM::AbstractAction>	factory(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, int16_t type, int16_t model = -1);	// Factory of sector action build from type

  protected:
    virtual void	remove(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector) = 0;	// Remove action from sector

  public:
    AbstractAction(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector);
    virtual ~AbstractAction() = default;

    virtual void	update(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed) = 0;	// Update sector's action

    virtual void	stop(DOOM::Doom & doom, DOOM::AbstractThing & thing);	// Request action to stop (for lift & crusher)
    virtual void	start(DOOM::Doom & doom, DOOM::AbstractThing & thing);	// Request action to start or re-trigger (for door, lift & crusher)
  };
};

#endif