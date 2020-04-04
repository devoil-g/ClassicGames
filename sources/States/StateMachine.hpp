#pragma once

#include <mutex>
#include <list>

#include "States/AbstractState.hpp"

namespace Game
{
  class StateMachine
  {
  private:
    class DefaultState : public Game::AbstractState
    {
    public:
      bool  update(sf::Time) override { return true; }; // Force quit
      void  draw() override {};                         // Do nothing
    };

    std::list<Game::AbstractState*> _states;  // Stack of states
    std::recursive_mutex            _lock;    // Lock for multi-threading

    StateMachine();
    ~StateMachine();

  public:
    inline static Game::StateMachine& Instance() { static Game::StateMachine singleton; return singleton; };  // Return unique instance (singleton)

    void  run();  // Run state machine

    void  swap(Game::AbstractState*); // Change current state to another
    void  push(Game::AbstractState*); // Push a state on the stack
    void  pop();                      // Pop top state of the stack
  };
}