#ifndef _SCROLLER_LINEDEF_HPP_
#define _SCROLLER_LINEDEF_HPP_

#include "Doom/Linedef/AbstractLinedef.hpp"

namespace DOOM
{
  class ScrollerLinedef : public DOOM::AbstractLinedef
  {
  public:
    ScrollerLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef);
    ~ScrollerLinedef() override = default;

    void	update(DOOM::Doom & doom, sf::Time elapsed) override;	// Update linedef
  };
};

#endif