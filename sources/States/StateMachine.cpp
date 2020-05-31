#include <cassert>

#include "States/StateMachine.hpp"
#include "System/Window.hpp"
#include "System/Sound.hpp"

Game::StateMachine::StateMachine() :
  Game::AbstractState(*this)
{
  // NOTE: the machine is referencing itself, shoud we do this ?
}

void  Game::StateMachine::pop()
{
  // Thread-lock the machine
  std::unique_lock  lock(_lock);

  // Check if stack not empty
  if (_states.empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Pop state
  _states.pop();
}

void  Game::StateMachine::clear()
{
  // Thread-lock the machine
  std::unique_lock  lock(_lock);

  // Empty stack
  while (_states.empty() == false)
    _states.pop();
}

bool  Game::StateMachine::update(sf::Time elapsed)
{
  // Thread-lock the machine
  std::unique_lock  lock(_lock);

  // Check if stack not empty
  if (_states.empty() == true)
    return true;

  // Update top state
  return _states.top()->update(elapsed);
}

void  Game::StateMachine::draw()
{
  // Thread-lock the machine
  std::unique_lock  lock(_lock);

  // Check if stack not empty
  if (_states.empty() == true)
    return;

  // Draw front state
  _states.top()->draw();
}