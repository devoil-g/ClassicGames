#ifndef _ABSTRACT_NONE_PICKUP_THING_HPP_
#define _ABSTRACT_NONE_PICKUP_THING_HPP_

#include "Doom/Doom.hpp"
#include "Doom/Thing/AbstractThing.hpp"

namespace DOOM
{
  class AbstractNonePickupThing : public virtual DOOM::AbstractThing
  {
  protected:
    AbstractNonePickupThing(DOOM::Doom & doom, int16_t height, int16_t radius, int16_t properties);	// Special constructor for player only

  public:
    AbstractNonePickupThing(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing, int16_t height, int16_t radius, int16_t properties);
    virtual ~AbstractNonePickupThing() = 0;

    virtual void	pickup(DOOM::Doom & doom, DOOM::AbstractThing & monster) override;	// Does nothing
    virtual bool	update(DOOM::Doom & doom, sf::Time elapsed) override;			// Update thing, return true if thing should be deleted
  };
};

#endif