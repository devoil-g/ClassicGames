#ifndef _ABSTRACT_PHYSICS_THING_HPP_
#define _ABSTRACT_PHYSICS_THING_HPP_

#include "Doom/Doom.hpp"
#include "Math/Vector.hpp"

#include <iostream>

namespace DOOM
{
  template<unsigned int Mass>
  class AbstractPhysicsThing : public DOOM::AbstractThing
  {
  private:
    Math::Vector<2>	_thrust;	// Thrust vector (unit/tic)

  protected:
    AbstractPhysicsThing(DOOM::Doom & doom, int16_t radius, int16_t properties) :	// Special constructor for player only
      DOOM::AbstractThing(doom, radius, properties)
    {}

  public:
    AbstractPhysicsThing(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing, int16_t radius, int16_t properties) :
      DOOM::AbstractThing(doom, thing, radius, properties),
      _thrust()
    {}

    virtual ~AbstractPhysicsThing() = default;

    virtual void	thrust(sf::Time elapsed, const Math::Vector<2> & acceleration)	// Apply acceleration to thing
    {
      // Apply thrust vector to player based on acceleration and elapsed time
      _thrust += acceleration * std::powf(0.06f / (float)Mass, elapsed.asSeconds() / DOOM::Doom::Tic.asSeconds());
    }

    virtual bool	update(DOOM::Doom & doom, sf::Time elapsed) override	// Update thing physics
    {
      // Update base class
      DOOM::AbstractThing::update(doom, elapsed);
      
      // Clip player's speed if it is too large
      // TODO: should we implement speed limitation as engine support any speed ?

      // Does nothing if no movements
      if (std::fabsf(_thrust.x()) < 0.001f && std::fabsf(_thrust.y()) < 0.001f)
	return false;

      // Actual checks, calculations, and movements
      // TODO: magic stuff
      position += ((_thrust.length() > 30.f) ? (_thrust * 30.f / _thrust.length()) : _thrust) * elapsed.asSeconds() / DOOM::Doom::Tic.asSeconds();
      
      std::cout
	<< "["
	<< ((_thrust.length() > 30.f) ? (_thrust * 30.f / _thrust.length()) : _thrust).x()
	<< ", "
	<< ((_thrust.length() > 30.f) ? (_thrust * 30.f / _thrust.length()) : _thrust).y()
	<< "]: "
	<< ((_thrust.length() > 30.f) ? (_thrust * 30.f / _thrust.length()) : _thrust).length() * 35
	<< " ("
	<< ((_thrust.length() > 30.f) ? (_thrust * 30.f / _thrust.length()) : _thrust).length()
	<< ")"
	<< std::endl;

      // Apply friction slowdown to player for next tic (hard coded drag factor of 0.90625)
      _thrust *= std::powf(0.90625f, elapsed.asSeconds() / DOOM::Doom::Tic.asSeconds());

      return false;
    }
  };
};

#endif