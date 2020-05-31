#include <iostream>

#include "Doom/States/SplashDoomState.hpp"

DOOM::SplashDoomState::SplashDoomState(Game::StateMachine& machine, DOOM::Doom& doom) :
  Game::AbstractState(machine),
  _doom(doom)
{}

bool	DOOM::SplashDoomState::update(sf::Time elapsed)
{
  // TODO:

  return false;
}

void	DOOM::SplashDoomState::draw()
{
  // TODO:

}