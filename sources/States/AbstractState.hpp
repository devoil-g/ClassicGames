#pragma once

#include <SFML/System/Time.hpp>

namespace Game
{
  // Forward declaration of external components
  class StateMachine;

  class AbstractState
  {
  protected:
    Game::StateMachine& _machine; // Reference to state's master

  public:
    AbstractState(Game::StateMachine& machine);
    virtual ~AbstractState() = 0;

    virtual bool  update(sf::Time) = 0; // You should override this method
    virtual void  draw() = 0;           // You should override this method
  };
}

// Definition of class forward-declared (NOTE: we shouldn't do this)
#include "States/StateMachine.hpp"