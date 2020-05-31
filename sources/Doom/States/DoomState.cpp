#include <iostream>

#include "Doom/States/DoomState.hpp"
#include "Doom/States/StartDoomState.hpp"
#include "System/Config.hpp"

DOOM::DoomState::DoomState(Game::StateMachine& machine) :
  Game::AbstractState(machine),
  _doom(),
  _game()
{
  // Load WAD
  _doom.load(Game::Config::ExecutablePath + "assets/levels/doom.wad");

  // Check that at least one level is available
  if (_doom.getLevels().empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Push initial state
  _game.push<DOOM::StartDoomState>(_doom);
}

bool	DOOM::DoomState::update(sf::Time elapsed)
{
  // Update game
  if (_game.update(elapsed) == true)
    _machine.pop();

  return false;
}

void	DOOM::DoomState::draw()
{
  // Draw game
  _game.draw();
}