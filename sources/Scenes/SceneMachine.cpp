#include <cassert>

#include "Scenes/SceneMachine.hpp"

Game::SceneMachine::SceneMachine() :
  Game::AbstractScene(*this)
{
  // NOTE: the machine is referencing itself, shoud we do this ?
}

void  Game::SceneMachine::pop()
{
  // Thread-lock the machine
  std::unique_lock  lock(_lock);

  // Check if stack not empty
  if (_states.empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Pop state
  _states.pop();
}

void  Game::SceneMachine::clear()
{
  // Thread-lock the machine
  std::unique_lock  lock(_lock);

  // Empty stack
  while (_states.empty() == false)
    _states.pop();
}

bool  Game::SceneMachine::update(sf::Time elapsed)
{
  // Thread-lock the machine
  std::unique_lock  lock(_lock);

  // Check if stack not empty
  if (_states.empty() == true)
    return true;

  // Update top state
  return _states.top()->update(elapsed) || _states.empty();
}

void  Game::SceneMachine::draw()
{
  // Thread-lock the machine
  std::unique_lock  lock(_lock);

  // Check if stack not empty
  if (_states.empty() == true)
    return;

  // Draw front state
  _states.top()->draw();
}