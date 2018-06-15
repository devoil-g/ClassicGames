#ifndef _GLOW_LIGHT_SECTOR_HPP_
#define _GLOW_LIGHT_SECTOR_HPP_

#include "Doom/Sector/AbstractLightSector.hpp"

namespace DOOM
{
  class GlowLightSector : public DOOM::AbstractLightSector
  {
  public:
    GlowLightSector(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector);
    virtual ~GlowLightSector();

    virtual void	update(sf::Time elapsed) override;	// Update sector
  };
};

#endif