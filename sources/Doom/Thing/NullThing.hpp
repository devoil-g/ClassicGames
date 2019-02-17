#ifndef _NULL_THING_HPP_
#define _NULL_THING_HPP_

#include "Doom/Thing/AbstractNoneAnimationThing.hpp"
#include "Doom/Thing/AbstractNonePhysicsThing.hpp"
#include "Doom/Thing/AbstractNonePickupThing.hpp"

namespace DOOM
{
  class NullThing : public DOOM::AbstractNonePhysicsThing, public DOOM::AbstractNoneAnimationThing, public DOOM::AbstractNonePickupThing
  {    
  public:
    NullThing(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing);
    virtual ~NullThing() = default;

    virtual bool	update(DOOM::Doom & doom, sf::Time elapsed) override;	// Update thing, return true if thing should be deleted

    using DOOM::AbstractNoneAnimationThing::sprite;
    using DOOM::AbstractNonePhysicsThing::thrust;
    using DOOM::AbstractNonePickupThing::pickup;
  };
};

#endif