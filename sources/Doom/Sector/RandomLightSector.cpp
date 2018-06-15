#include "Doom/Doom.hpp"
#include "Doom/Sector/RandomLightSector.hpp"

DOOM::RandomLightSector::RandomLightSector(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector) :
  AbstractLightSector(doom, sector),
  _cycle((int16_t)(Math::Random() * 24.f) + 16),
  _flash((int16_t)(Math::Random() * 4.f) + 1),
  _last(sf::Time::Zero)
{}

DOOM::RandomLightSector::~RandomLightSector()
{}

void	DOOM::RandomLightSector::update(sf::Time elapsed)
{
  // Update light sector
  DOOM::AbstractLightSector::update(elapsed);

  while (_elapsed - _last > DOOM::Doom::Tic * (float)_cycle) {
    _last += DOOM::Doom::Tic * (float)(_cycle);
    _cycle = (int16_t)(Math::Random() * 24.f) + 16;
    _flash = (int16_t)(Math::Random() * 4.f) + 1;
  }

  if (_elapsed.asMicroseconds() / DOOM::Doom::Tic.asMicroseconds() % _cycle < _flash)
    _current = _light;
  else
    _current = getNeighborLowestLight();
}