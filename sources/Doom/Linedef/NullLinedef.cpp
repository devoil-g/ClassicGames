#include "Doom/Linedef/NullLinedef.hpp"

DOOM::NullLinedef::NullLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
  DOOM::AbstractLinedef(doom, linedef)
{}

DOOM::NullLinedef::NullLinedef(DOOM::Doom & doom, const DOOM::AbstractLinedef & linedef) :
  DOOM::AbstractLinedef(doom, linedef)
{}

void	DOOM::NullLinedef::update(DOOM::Doom & doom, sf::Time)
{}