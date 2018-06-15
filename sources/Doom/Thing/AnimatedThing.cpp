#include "Doom/Doom.hpp"
#include "Doom/Thing/AnimatedThing.hpp"

DOOM::AnimatedThing::AnimatedThing(DOOM::Doom const & doom, std::string const & sprite, std::string const & sequence, DOOM::Wad::RawLevel::Thing const & thing, int16_t radius, int16_t properties) :
  AbstractThing(doom, thing, radius, properties),
  _elapsed(),
  _sprites(sequence.length())
{
  // Initialize sprites
  for (int index = 0; index < sequence.length(); index++)
    for (int angle = 0; angle < 8; angle++)
      _sprites[index][angle] = { &DOOM::Doom::Resources::NullTexture, false };

  // Find texture in DOOM resources associated with sprite and sequence
  for (std::pair<int64_t, DOOM::Doom::Resources::Texture const &> const & texture : doom.resources.sprites)
    if ((texture.first & 0x00000000FFFFFFFF) == DOOM::str_to_key(sprite))
      for (int index = 0; index < sequence.length(); index++)
      {
	// Match texture
	if (((texture.first & 0x000000FF00000000) >> 32) == sequence[index] &&
	  ((texture.first & 0x0000FF0000000000) >> 40) >= '0' && ((texture.first & 0x0000FF0000000000) >> 40) <= '8')
	{
	  if (((texture.first & 0x0000FF0000000000) >> 40) == '0')
	    for (int angle = 0; angle < 8; angle++)
	      _sprites[index][angle] = { &texture.second, false };
	  else
	    _sprites[index][((texture.first & 0x0000FF0000000000) >> 40) - '0'] = { &texture.second, false };
	}

	// Match mirrored texture
	if (((texture.first & 0x00FF000000000000) >> 48) == sequence[index] &&
	  ((texture.first & 0xFF00000000000000) >> 56) >= '0' && ((texture.first & 0xFF00000000000000) >> 56) <= '8')
	{
	  if (((texture.first & 0xFF00000000000000) >> 56) == '0')
	    for (int angle = 0; angle < 8; angle++)
	      _sprites[index][angle] = { &texture.second, true };
	  else
	    _sprites[index][((texture.first & 0xFF00000000000000) >> 56) - '0'] = { &texture.second, true };
	}
      }
}

DOOM::AnimatedThing::~AnimatedThing()
{}

bool	DOOM::AnimatedThing::update(sf::Time elapsed)
{
  // Add elapsed time to counter
  _elapsed += elapsed;

  return false;
}

std::pair<DOOM::Doom::Resources::Texture const *, bool> const &	DOOM::AnimatedThing::sprite(float angle) const
{
  // TODO: find angle
  return _sprites[_elapsed.asMicroseconds() / (DOOM::Doom::Tic.asMicroseconds() * DOOM::AnimatedThing::FrameDuration) % _sprites.size()][0];
}