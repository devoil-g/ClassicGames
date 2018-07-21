#ifndef _NULL_LINEDEF_HPP_
#define _NULL_LINEDEF_HPP_

#include "Doom/Linedef/AbstractLinedef.hpp"

namespace DOOM
{
  class NullLinedef : public DOOM::AbstractLinedef
  {
  public:
    NullLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef);
    NullLinedef(DOOM::Doom & doom, const DOOM::AbstractLinedef & linedef);
    ~NullLinedef() override = default;

    void	update(DOOM::Doom & doom, sf::Time elapsed) override;	// Update linedef
  };
};

#endif