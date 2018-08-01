#include "Doom/Linedef/ScrollerLinedef.hpp"

DOOM::ScrollerLinedef::ScrollerLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
  DOOM::AbstractLinedef(doom, linedef)
{}

void	DOOM::ScrollerLinedef::update(DOOM::Doom & doom, sf::Time elapsed)
{
  doom.level.sidedefs[front].x += elapsed.asSeconds() / DOOM::Doom::Tic.asSeconds();
}