#ifndef _FIXED_LIGHT_SECTOR_HPP_
#define _FIXED_LIGHT_SECTOR_HPP_

#include "Doom/Sector/AbstractLightSector.hpp"

namespace DOOM
{
  template<int CycleDuration, int FlashDuration>
  class FixedLightSector : public DOOM::AbstractLightSector
  {
  public:
    FixedLightSector(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector) :
      AbstractLightSector(doom, sector)
    {}

    virtual ~FixedLightSector()
    {}

    virtual void	update(sf::Time elapsed) override	// Update sector
    {
      DOOM::AbstractLightSector::update(elapsed);

      if (_elapsed.asMicroseconds() / DOOM::Doom::Tic.asMicroseconds() % CycleDuration < FlashDuration)
	_current = _light;
      else
	_current = getNeighborLowestLight();
    }
  };
};

#endif