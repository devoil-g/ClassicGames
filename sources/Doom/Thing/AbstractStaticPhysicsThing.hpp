#ifndef _ABSTRACT_STATIC_PHYSICS_THING_HPP_
#define _ABSTRACT_STATIC_PHYSICS_THING_HPP_

#include "Doom/Doom.hpp"
#include "Math/Vector.hpp"

namespace DOOM
{
  class AbstractStaticPhysicsThing : public virtual DOOM::AbstractThing
  {
  private:
    int16_t	_sector;	// Sector index of thing

  public:
    AbstractStaticPhysicsThing(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing, int16_t height, int16_t radius, int16_t properties);
    virtual ~AbstractStaticPhysicsThing() = 0;

    virtual bool	update(DOOM::Doom & doom, sf::Time elapsed) override;					// Only update Z position
    virtual void	thrust(const Math::Vector<2> & acceleation) override;					// Ignore acceleration
    virtual void	teleport(DOOM::Doom & doom, const Math::Vector<2> & destination, float angle) override;	// Apply acceleration to thing
  };
};

#endif