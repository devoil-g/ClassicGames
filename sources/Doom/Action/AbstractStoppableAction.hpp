#ifndef _ABSTRACT_STOPPABLE_ACTION_HPP_
#define _ABSTRACT_STOPPABLE_ACTION_HPP_

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumAction::Type Type,
    DOOM::EnumAction::Change Change = DOOM::EnumAction::Change::ChangeNone
  >
  class AbstractStoppableAction : public DOOM::AbstractTypeAction<Type, Change>
  {
  protected:
    bool	_stop;	// Flag for stop

  public:
    AbstractStoppableAction(DOOM::Doom & doom) :
      AbstractTypeAction(doom),
      _stop(false)
    {}

    virtual ~AbstractStoppableAction() = default;

    void	stop() override	// Request action to stop (for lift & crusher)
    {
      // Set stop flag
      _stop = true;
    }
  };
};

#endif