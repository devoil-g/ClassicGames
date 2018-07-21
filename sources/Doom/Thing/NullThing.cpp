#include "Doom/Thing/NullThing.hpp"

DOOM::NullThing::NullThing(DOOM::Doom & doom, DOOM::Wad::RawLevel::Thing const & thing) :
  DOOM::AbstractThing(doom, thing, 0, None)
{}

std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool> const &	DOOM::NullThing::sprite(float) const
{
  static const std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool>	frame = { std::ref(DOOM::Doom::Resources::Texture::Null), false };

  // Return a default empty texture
  return frame;
}
