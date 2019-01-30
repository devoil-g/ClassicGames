#ifndef _NULL_THING_HPP_
#define _NULL_THING_HPP_

#include "Doom/Thing/AbstractNoneAnimationThing.hpp"
#include "Doom/Thing/AbstractNonePhysicsThing.hpp"

namespace DOOM
{
  class NullThing : public DOOM::AbstractNonePhysicsThing, public DOOM::AbstractNoneAnimationThing
  {    
  public:
    NullThing(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing);
    virtual ~NullThing() = default;

    using DOOM::AbstractNoneAnimationThing::update;
    using DOOM::AbstractNoneAnimationThing::sprite;
    using DOOM::AbstractNonePhysicsThing::thrust;
  };
};

#endif