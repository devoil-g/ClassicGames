#ifndef _ABSTRACT_STATIC_PICKUP_THING_HPP_
#define _ABSTRACT_STATIC_PICKUP_THING_HPP_

#include "Doom/Thing/AbstractThing.hpp"

namespace DOOM
{
  class AbstractStaticPickupThing : public virtual DOOM::AbstractThing
  {
  private:
    bool	_pickup;	// True if thing has been picked up

  protected:
    AbstractStaticPickupThing(DOOM::Doom & doom, int16_t height, int16_t radius, int16_t properties);	// Special constructor for player only

  public:
    AbstractStaticPickupThing(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing, int16_t height, int16_t radius, int16_t properties);
    virtual ~AbstractStaticPickupThing() = 0;

    virtual void	pickup(DOOM::Doom & doom, DOOM::AbstractThing & monster) override;	// Does nothing
    virtual bool	update(DOOM::Doom & doom, sf::Time elapsed) override;			// Update thing, return true if thing should be deleted
  };
};

#endif