#ifndef _NULL_THING_HPP_
#define _NULL_THING_HPP_

#include <utility>

#include <SFML/System/Time.hpp>

#include "Doom/Doom.hpp"

namespace DOOM
{
  class NullThing : public virtual DOOM::Doom::Level::AbstractThing
  {    
  public:
    NullThing(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing);
    virtual ~NullThing() = default;

    const std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool> &	sprite(float angle) const override;
  };
};

#endif