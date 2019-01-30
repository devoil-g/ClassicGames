#include "Doom/Thing/AbstractNoneAnimationThing.hpp"

DOOM::AbstractNoneAnimationThing::AbstractNoneAnimationThing(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing, int16_t height, int16_t radius, int16_t properties) :
  DOOM::AbstractThing(doom, thing, height, radius, properties)
{}

bool	DOOM::AbstractNoneAnimationThing::update(DOOM::Doom & doom, sf::Time elapsed)
{
  // Does nothing
  return false;
}

const std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool> &	DOOM::AbstractNoneAnimationThing::sprite(float angle) const
{
  static const std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool>	frame = { std::ref(DOOM::Doom::Resources::Texture::Null), false };

  // Return a default empty texture
  return frame;
}