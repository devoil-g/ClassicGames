#include "Doom/Linedef/NullLinedef.hpp"

DOOM::NullLinedef::NullLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
  DOOM::Doom::Level::AbstractLinedef(doom, linedef)
{}

DOOM::NullLinedef::NullLinedef(DOOM::Doom & doom, const DOOM::Doom::Level::AbstractLinedef & linedef) :
  DOOM::Doom::Level::AbstractLinedef(doom, linedef)
{}

void	DOOM::NullLinedef::update(DOOM::Doom & doom, sf::Time)
{}