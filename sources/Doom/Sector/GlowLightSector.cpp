#include "Doom/Doom.hpp"
#include "Doom/Sector/GlowLightSector.hpp"

DOOM::GlowLightSector::GlowLightSector(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector) :
  AbstractLightSector(doom, sector)
{}

DOOM::GlowLightSector::~GlowLightSector()
{}

void	DOOM::GlowLightSector::update(sf::Time elapsed)
{
  DOOM::AbstractLightSector::update(elapsed);

  int16_t	darkest = getNeighborLowestLight();

  // Not glow if all connected sectors have the same or higher brightness
  if (darkest >= _light)
    _current = _light;

  // Return glowing light level
  else
    _current = (int16_t)((std::cos(_elapsed.asSeconds() / (DOOM::Doom::Tic.asSeconds() * 32) * Math::Pi) + 1.f) / 2.f * (_light - darkest) + darkest);
}