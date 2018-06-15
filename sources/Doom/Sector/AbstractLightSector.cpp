#include "Doom/Doom.hpp"
#include "Doom/Sector/AbstractLightSector.hpp"

DOOM::AbstractLightSector::AbstractLightSector(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector) :
  AbstractSector(doom, sector),
  _current(sector.light)
{}

DOOM::AbstractLightSector::~AbstractLightSector()
{}

void	DOOM::AbstractLightSector::update(sf::Time elapsed)
{
  // Update elapsed time
  _elapsed += elapsed;
}

int16_t	DOOM::AbstractLightSector::light() const
{
  return _current;
}