#ifndef _NORMAL_LINEDEF_HPP_
#define _NORMAL_LINEDEF_HPP_

#include "Doom/Linedef/ActionTriggerableLinedef.hpp"

namespace DOOM
{
  class NullLinedef : public DOOM::AbstractLinedef
  {
  public:
    NullLinedef(const DOOM::Wad::RawLevel::Linedef & linedef);
    NullLinedef(const DOOM::AbstractLinedef & linedef);
    ~NullLinedef() override;

    void	update(sf::Time elapsed) override;	// Update linedef
  };
};

#endif