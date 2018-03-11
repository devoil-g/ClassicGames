#include "Doom/Thing/NullThing.hpp"	

Game::NullThing::NullThing(Game::Doom const & doom, Game::Wad::RawLevel::Thing const & thing) :
  AbstractThing(doom, thing, 0, None)
{}

Game::NullThing::~NullThing()
{}

std::pair<Game::Doom::Resources::Texture const *, bool> const &	Game::NullThing::sprite(float) const
{
  static std::pair<Game::Doom::Resources::Texture const *, bool> const	frame = { &Game::Doom::Resources::NullTexture, false };

  // Return a default empty texture
  return frame;
}
