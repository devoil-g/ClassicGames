#include "Doom/Doom.hpp"
#include "Doom/Sector/FlickerLightSector.hpp"

DOOM::FlickerLightSector::FlickerLightSector(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector) :
  AbstractLightSector(doom, sector),
  _cycle((int16_t)(Math::Random() * 24.f) + 4),
  _flicker((int16_t)(Math::Random() * 4.f) + 1),
  _last(sf::Time::Zero)
{}

DOOM::FlickerLightSector::~FlickerLightSector()
{}

void	DOOM::FlickerLightSector::update(sf::Time elapsed)
{
  // Update light sector
  DOOM::AbstractLightSector::update(elapsed);

  while (_elapsed - _last > DOOM::Doom::Tic * (float)(_cycle + _flicker)) {
    _last += DOOM::Doom::Tic * (float)(_cycle + _flicker);
    _cycle = (int16_t)(Math::Random() * 24.f) + 4;
    _flicker = (int16_t)(Math::Random() * 4.f) + 1;
  }

  if (_elapsed.asMicroseconds() / DOOM::Doom::Tic.asMicroseconds() % _cycle < _flicker)
    _current = _light;
  else
    _current = std::max(_light + 16, 255);
}