#include "Doom/Sector/NormalSector.hpp"

DOOM::NormalSector::NormalSector(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector) :
  AbstractSector(doom, sector)
{}

DOOM::NormalSector::~NormalSector()
{}

void	DOOM::NormalSector::update(sf::Time)
{}