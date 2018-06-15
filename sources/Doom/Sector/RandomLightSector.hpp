#ifndef _RANDOM_LIGHT_SECTOR_HPP_
#define _RANDOM_LIGHT_SECTOR_HPP_

#include <SFML/System/Time.hpp>

#include "Doom/Sector/AbstractLightSector.hpp"

namespace DOOM
{
  class RandomLightSector : public DOOM::AbstractLightSector
  {
  private:
    int16_t	_cycle;	// Cycle duration
    int16_t	_flash;	// Flash duration
    sf::Time	_last;	// Last cycle timestamp

  public:
    RandomLightSector(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector);
    virtual ~RandomLightSector();

    virtual void	update(sf::Time elapsed) override;	// Update light sector
  };
};

#endif