#ifndef _FLICKER_LIGHT_SECTOR_HPP_
#define _FLICKER_LIGHT_SECTOR_HPP_

#include "Doom/Sector/AbstractLightSector.hpp"

namespace DOOM
{
  class FlickerLightSector : public DOOM::AbstractLightSector
  {
  private:
    int16_t	_cycle;		// Cycle duration
    int16_t	_flicker;	// Flicker duration
    sf::Time	_last;		// Last cycle timestamp

  public:
    FlickerLightSector(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector);
    virtual ~FlickerLightSector();

    virtual void	update(sf::Time elapsed) override;	// Update light sector
  };
};

#endif