#ifndef _ABSTRACT_ACTION_HPP_
#define _ABSTRACT_ACTION_HPP_

#include "Doom/Doom.hpp"

namespace DOOM
{
  // TODO: refactor actions
  class AbstractAction
  {
  public:
    static std::unique_ptr<DOOM::AbstractAction>	factory(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, int16_t type, int16_t model = -1);	// Factory of sector action build from type

  protected:
    virtual void	remove(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector) = 0;	// Remove action from sector

  public:
    AbstractAction(DOOM::Doom & doom);
    virtual ~AbstractAction() = default;

    virtual void	update(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed) = 0;	// Update sector's action
    virtual void	stop();											// Request action to stop (for lift & crusher)
  };
};

#endif