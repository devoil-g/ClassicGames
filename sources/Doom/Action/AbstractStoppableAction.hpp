#pragma once

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<
    DOOM::Doom::Level::Sector::Action Type,
    DOOM::EnumAction::Change::Type ChangeType = DOOM::EnumAction::Change::Type::None
  >
    class AbstractStoppableAction : public DOOM::AbstractTypeAction<Type, ChangeType>
  {
  protected:
    bool  _run; // Flag for stop

  public:
    AbstractStoppableAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector) :
      DOOM::AbstractTypeAction<Type, ChangeType>(doom, sector),
      _run(true)
    {}

    virtual ~AbstractStoppableAction() = default;

    bool  stop(DOOM::Doom& doom, DOOM::AbstractThing& thing) override // Request action to stop (for lift & crusher)
    {
      // TODO: add check of thing type ?
      if (thing.type != DOOM::Enum::ThingType::ThingType_PLAYER)
        return true;

      // Set stop flag
      if (_run == true) {
        _run = false;
        return true;
      }
      else {
        return false;
      }
    }

    bool  start(DOOM::Doom& doom, DOOM::AbstractThing& thing) override  // Request action to start (for lift & crusher)
    {
      // TODO: add check of thing type ?
      if (thing.type != DOOM::Enum::ThingType::ThingType_PLAYER)
        return true;

      // Set run flag
      if (_run == false) {
        _run = true;
        return true;
      }
      else {
        return false;
      }
    }
  };
}