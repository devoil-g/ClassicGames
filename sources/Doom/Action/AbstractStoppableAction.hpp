#ifndef _ABSTRACT_STOPPABLE_ACTION_HPP_
#define _ABSTRACT_STOPPABLE_ACTION_HPP_

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<
    DOOM::Doom::Level::Sector::Action Type,
    DOOM::EnumAction::Change Change = DOOM::EnumAction::Change::ChangeNone
  >
  class AbstractStoppableAction : public DOOM::AbstractTypeAction<Type, Change>
  {
  protected:
    bool	_run;	// Flag for stop

  public:
    AbstractStoppableAction(DOOM::Doom & doom) :
      AbstractTypeAction(doom),
      _run(true)
    {}

    virtual ~AbstractStoppableAction() = default;

    void	stop(DOOM::Doom & doom, DOOM::AbstractThing & thing) override	// Request action to stop (for lift & crusher)
    {
      // TODO: add check of thing type ?

      // Set stop flag
      _run = false;
    }

    void	start(DOOM::Doom & doom, DOOM::AbstractThing & thing) override	// Request action to start (for lift & crusher)
    {
      // TODO: add check of thing type ?

      // Set run flag
      _run = true;
    }
  };
};

#endif