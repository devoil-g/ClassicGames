#ifndef _NULL_THING_HPP_
#define _NULL_THING_HPP_

#include <utility>

#include <SFML/System/Time.hpp>

#include "Doom/Thing/AbstractThing.hpp"
#include "Doom/Doom.hpp"

namespace DOOM
{
  class NullThing : public virtual AbstractThing
  {    
  public:
    NullThing(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing);
    virtual ~NullThing();

    const std::pair<DOOM::Doom::Resources::Texture const *, bool> &	sprite(float angle) const override;
  };
};

#endif