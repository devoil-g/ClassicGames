#ifndef _NULL_LINEDEF_HPP_
#define _NULL_LINEDEF_HPP_

#include "Doom/Doom.hpp"

namespace DOOM
{
  class NullLinedef : public DOOM::Doom::Level::AbstractLinedef
  {
  public:
    NullLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef);
    NullLinedef(DOOM::Doom & doom, const DOOM::Doom::Level::AbstractLinedef & linedef);
    ~NullLinedef() override = default;

    void	update(DOOM::Doom & doom, sf::Time elapsed) override;	// Update linedef
  };
};

#endif