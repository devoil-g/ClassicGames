#ifndef _ABSTRACT_NONE_PHYSICS_THING_HPP_
#define _ABSTRACT_NONE_PHYSICS_THING_HPP_

#include "Doom/Doom.hpp"
#include "Math/Vector.hpp"

namespace DOOM
{
  class AbstractNonePhysicsThing : public virtual DOOM::AbstractThing
  {
  public:
    AbstractNonePhysicsThing(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing, int16_t height, int16_t radius, int16_t properties);
    virtual ~AbstractNonePhysicsThing() = 0;

    virtual void	thrust(const Math::Vector<2> & acceleation) override;					// Does nothing
    virtual void	teleport(DOOM::Doom & doom, const Math::Vector<2> & destination, float angle) override;	// Apply acceleration to thing
  };
};

#endif