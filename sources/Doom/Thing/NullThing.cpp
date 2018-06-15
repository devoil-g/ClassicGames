#include "Doom/Doom.hpp"
#include "Doom/Thing/NullThing.hpp"

DOOM::NullThing::NullThing(DOOM::Doom const & doom, DOOM::Wad::RawLevel::Thing const & thing) :
  AbstractThing(doom, thing, 0, None)
{}

DOOM::NullThing::~NullThing()
{}

std::pair<DOOM::Doom::Resources::Texture const *, bool> const &	DOOM::NullThing::sprite(float) const
{
  static std::pair<DOOM::Doom::Resources::Texture const *, bool> const	frame = { &DOOM::Doom::Resources::NullTexture, false };

  // Return a default empty texture
  return frame;
}
