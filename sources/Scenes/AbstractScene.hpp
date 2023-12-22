#pragma once

#include <SFML/System/Time.hpp>

namespace Game
{
  // Forward declaration of external components
  class SceneMachine;

  class AbstractScene
  {
  protected:
    Game::SceneMachine& _machine; // Reference to state's master

  public:
    AbstractScene(Game::SceneMachine& machine);
    virtual ~AbstractScene() = 0;

    virtual bool  update(float elapsed) = 0;  // You should override this method
    virtual void  draw() = 0;                 // You should override this method
  };
}

// Definition of forward-declared class (NOTE: we shouldn't do this)
#include "Scenes/SceneMachine.hpp"