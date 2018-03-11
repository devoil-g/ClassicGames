#ifndef _NULL_THING_HPP_
#define _NULL_THING_HPP_

#include <utility>

#include <SFML/System/Time.hpp>

#include "Doom/Thing/AbstractThing.hpp"
#include "Doom/Doom.hpp"

namespace Game
{
  class NullThing : public virtual AbstractThing
  {    
  public:
    NullThing(const Game::Doom &, const Game::Wad::RawLevel::Thing &);
    virtual ~NullThing();

    const std::pair<Game::Doom::Resources::Texture const *, bool> &	sprite(float) const override;
  };
};

#endif