#ifndef _ABSTRACT_STOPPABLE_ACTION_HPP_
#define _ABSTRACT_STOPPABLE_ACTION_HPP_

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<DOOM::EnumAction::Type Type>
  class AbstractStoppableAction : public DOOM::AbstractTypeAction<Type>
  {
  protected:
    bool	_stop;

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