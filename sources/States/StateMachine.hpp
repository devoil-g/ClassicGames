#pragma once

#include <stdexcept>
#include <mutex>
#include <stack>

#include "States/AbstractState.hpp"

namespace Game
{
  class StateMachine : public Game::AbstractState
  {
  private:
    std::stack<std::unique_ptr<Game::AbstractState>>  _states;  // Stack of states
    std::recursive_mutex                              _lock;    // Machine thread lock

  public:
    StateMachine();
    ~StateMachine() = default;

    template<typename State, typename ...Args>
    void  push(Args&& ...args)  // Push a new state at the to top of the machine
    {
      // Build state before locking the machine
      auto              state = std::make_unique<State>(*this, std::forward<Args>(args)...);
      std::unique_lock  lock(_lock);

      // Push state in stack
      _states.push(std::move(state));
    }

    template<typename State, typename ...Args>
    void  swap(Args&& ...args) // Change current state to another
    {
      // Build state before locking the machine
      auto              state = std::make_unique<State>(*this, std::forward<Args>(args)...);
      std::unique_lock  lock(_lock);

      // Check that the machine isn't empty
      if (_states.empty() == true)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

      // Replace front state of the machine
      _states.top() = std::move(state);
    }

    void  pop();    // Pop top state of the stack
    void  clear();  // Remove all states of the stack

    virtual bool  update(sf::Time elapsed) override;  // Update the top state of the machine
    virtual void  draw() override;                    // Draw the top state of the machine
  };
}