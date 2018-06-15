#ifndef _ABSTRACT_LIGHT_SECTOR_HPP_
#define _ABSTRACT_LIGHT_SECTOR_HPP_

#include "Doom/Sector/AbstractSector.hpp"

namespace DOOM
{
  class AbstractLightSector : public DOOM::AbstractSector
  {
  protected:
    sf::Time	_elapsed;	// Elapsed time
    int16_t	_current;	// Current light level

  public:
    AbstractLightSector(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector);
    virtual ~AbstractLightSector();

    virtual void	update(sf::Time elapsed) override;	// Update light sector

    virtual int16_t	light() const override;	// Get current sector light level
  };
};

#endif