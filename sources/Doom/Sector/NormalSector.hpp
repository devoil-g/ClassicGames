#ifndef _NORMAL_SECTOR_HPP_
#define _NORMAL_SECTOR_HPP_

#include "Doom/Sector/AbstractSector.hpp"

namespace DOOM
{
  class NormalSector : public DOOM::AbstractSector
  {
  public:
    NormalSector(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector);
    virtual ~NormalSector();

    virtual void	update(sf::Time elapsed) override;	// Update sector
  };
};

#endif