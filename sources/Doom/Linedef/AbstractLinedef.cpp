#include "Doom/Linedef/AbstractLinedef.hpp"
#include "Doom/Linedef/NormalLinedef.hpp"

DOOM::AbstractLinedef::AbstractLinedef(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
  start(linedef.start), end(linedef.end),
  flag(linedef.flag),
  type(linedef.type),
  tag(linedef.tag),
  front(linedef.front), back(linedef.back)
{}

DOOM::AbstractLinedef::~AbstractLinedef()
{}

DOOM::AbstractLinedef *	DOOM::AbstractLinedef::factory(DOOM::Doom const & doom, DOOM::Wad::RawLevel::Linedef const & linedef)
{
  switch (linedef.type)
  {
  case 0:	// Normal linedef
    return new DOOM::NormalLinedef(doom, linedef);
  default:	// TODO: return nullptr for error
    return new DOOM::NormalLinedef(doom, linedef);
  }
}

void	DOOM::AbstractLinedef::pushed(const DOOM::AbstractThing &)
{}

void	DOOM::AbstractLinedef::switched(const DOOM::AbstractThing &)
{}

void	DOOM::AbstractLinedef::walkover(const DOOM::AbstractThing &)
{}

void	DOOM::AbstractLinedef::gunfire(const DOOM::AbstractThing &)
{}