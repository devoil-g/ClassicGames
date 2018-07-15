#include "Doom/Linedef/NullLinedef.hpp"

DOOM::NullLinedef::NullLinedef(const DOOM::Wad::RawLevel::Linedef & linedef) :
  DOOM::AbstractLinedef(linedef)
{}

DOOM::NullLinedef::NullLinedef(const DOOM::AbstractLinedef & linedef) :
  DOOM::AbstractLinedef(linedef)
{}

DOOM::NullLinedef::~NullLinedef()
{}

void	DOOM::NullLinedef::update(sf::Time)
{}