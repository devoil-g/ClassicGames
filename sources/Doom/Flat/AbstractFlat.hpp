#ifndef _ABSTRACT_FLAT_HPP_
#define _ABSTRACT_FLAT_HPP_

#include <vector>

#include <SFML/System/Time.hpp>

#include "Doom/Wad.hpp"
#include "Math/Vector.hpp"

namespace Game
{
  class AbstractFlat
  {
  public:
    static Game::AbstractFlat *	factory(const Game::Wad &, uint64_t, const Game::Wad::RawResources::Flat &);

    AbstractFlat();
    virtual ~AbstractFlat();

    virtual void				update(sf::Time);	// Update flat
    virtual std::vector<uint8_t> const &	flat() const = 0;	// Return flat to be displayed
  };
};

#endif