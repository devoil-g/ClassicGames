#ifndef _NULL_THING_HPP_
#define _NULL_THING_HPP_

#include "Doom/Thing/AbstractThing.hpp"

namespace DOOM
{
  class NullThing : public virtual DOOM::AbstractThing
  {    
  public:
    NullThing(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing);
    virtual ~NullThing() = default;

    const std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool> &	sprite(float angle) const override;	// Return sprite to be displayed
  };
};

#endif