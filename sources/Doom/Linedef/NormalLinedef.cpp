#include "Doom/Linedef/NormalLinedef.hpp"

DOOM::NormalLinedef::NormalLinedef(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
  AbstractLinedef(doom, linedef)
{}

DOOM::NormalLinedef::~NormalLinedef()
{}

void	DOOM::NormalLinedef::update(sf::Time)
{}