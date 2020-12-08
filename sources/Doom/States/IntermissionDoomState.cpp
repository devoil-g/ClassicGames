#include "Doom/States/IntermissionDoomState.hpp"

DOOM::IntermissionDoomState::IntermissionDoomState(Game::StateMachine& machine, DOOM::Doom& doom) :
  Game::AbstractState(machine),
  _doom(doom),
  _image(),
  _texture(),
  _sprite()
{
  // TODO
}

bool	DOOM::IntermissionDoomState::update(sf::Time elapsed)
{
  // TODO
  return false;
}

void	DOOM::IntermissionDoomState::draw()
{
  // TODO
}