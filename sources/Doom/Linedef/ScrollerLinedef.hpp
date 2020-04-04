#pragma once

#include "Doom/Linedef/AbstractLinedef.hpp"

namespace DOOM
{
  template<DOOM::EnumAction::Speed Speed = DOOM::EnumAction::Speed::SpeedSlow>
  class ScrollerLinedef : public DOOM::AbstractLinedef
  {
  public:
    ScrollerLinedef(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Linedef& linedef) :
      DOOM::AbstractLinedef(doom, linedef)
    {}

    ~ScrollerLinedef() override = default;

    void  update(DOOM::Doom& doom, sf::Time elapsed) override // Update linedef
    {
      // Scroll sidedef texture using offset
      doom.level.sidedefs[front].x += elapsed.asSeconds() / DOOM::Doom::Tic.asSeconds() * Speed;
    }
  };
}