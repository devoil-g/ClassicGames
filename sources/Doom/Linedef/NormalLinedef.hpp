#ifndef _NORMAL_LINEDEF_HPP_
#define _NORMAL_LINEDEF_HPP_

#include <SFML/System/Time.hpp>

#include "Doom/Doom.hpp"
#include "Doom/Linedef/AbstractLinedef.hpp"

namespace DOOM
{
  class NormalLinedef : public DOOM::AbstractLinedef
  {
  public:
    NormalLinedef(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef);
    virtual ~NormalLinedef();

    virtual void	update(sf::Time elapsed) override;	// Update linedef
  };
};

#endif