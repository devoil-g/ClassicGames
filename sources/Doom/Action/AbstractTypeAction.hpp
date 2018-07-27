#ifndef _ABSTRACT_TYPE_ACTION_HPP_
#define _ABSTRACT_TYPE_ACTION_HPP_

#include "Doom/Action/AbstractAction.hpp"

namespace DOOM
{
  template<DOOM::EnumAction::Type Type>
  class AbstractTypeAction : public DOOM::AbstractAction
  {
  protected:
    void	remove(DOOM::Doom::Level::Sector & sector) override	// Remove action from sector
    {
      // Remove action from sector
      sector.action<Type>().reset();
    }

  public:
    AbstractTypeAction(DOOM::Doom & doom) :
      AbstractAction(doom)
    {}

    virtual ~AbstractTypeAction() = default;
  };
};

#endif