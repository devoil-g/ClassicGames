#include <cassert>

#include "States/StateMachine.hpp"
#include "System/Window.hpp"
#include "System/Sound.hpp"

Game::StateMachine::StateMachine()
{
  // Push default state
  push(new Game::StateMachine::DefaultState());
}

Game::StateMachine::~StateMachine()
{
  // Clear remaining states
  while (!_states.empty())
  {
    // Avoid to delete twice the same pointer
    if (std::find(_states.begin(), _states.end(), _states.front()) == _states.end())
      delete _states.front();

    _states.pop_front();
  }
}

void			Game::StateMachine::run()
{
  sf::Clock		clock;
  
  while (Game::Window::Instance().window().isOpen())
  {
    sf::Time		elapsed = clock.restart();

    // Stop if update return true
    if (Game::Window::Instance().update(elapsed) == true ||
      Game::Sound::Instance().update(elapsed) == true ||
      _states.front()->update(elapsed) == true)
      return;

    // Draw image
    Game::Window::Instance().window().clear();
    _states.front()->draw();

    // Display image
    Game::Window::Instance().window().display();
  }
}

void			Game::StateMachine::swap(Game::AbstractState * state)
{
  _lock.lock();

  // Check for invalid parameter
  assert(state != nullptr);

  // Swap current state
  pop();
  push(state);

  _lock.unlock();
}

void			Game::StateMachine::push(Game::AbstractState * state)
{
  _lock.lock();

  // Check for invalid parameter
  assert(state != nullptr);

  // Push state in stack
  _states.push_front(state);
  
  _lock.unlock();
}

void			Game::StateMachine::pop()
{
  _lock.lock();

  // Check if stack not empty
  assert(_states.size() > 1);

  // Delete current state only if already in stack
  if (std::find(std::next(_states.begin()), _states.end(), _states.front()) == _states.end())
    delete _states.front();

  // Pop state
  _states.pop_front();

  _lock.unlock();
}
