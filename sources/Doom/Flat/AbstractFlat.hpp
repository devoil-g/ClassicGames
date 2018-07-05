#ifndef _ABSTRACT_FLAT_HPP_
#define _ABSTRACT_FLAT_HPP_

#include <vector>

#include <SFML/System/Time.hpp>

#include "Doom/Wad.hpp"
#include "Math/Vector.hpp"

namespace DOOM
{
  class AbstractFlat
  {
  public:
    static DOOM::AbstractFlat *	factory(const DOOM::Wad & wad, uint64_t, const DOOM::Wad::RawResources::Flat & flat);

    AbstractFlat();
    virtual ~AbstractFlat() = 0;

    virtual void				update(sf::Time elapsed);	// Update flat
    virtual const std::vector<uint8_t> &	flat() const = 0;		// Return flat to be displayed
  };
};

#endif