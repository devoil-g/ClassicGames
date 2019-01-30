#ifndef _DECORATION_THING_HPP_
#define _DECORATION_THING_HPP_

#include "Doom/Thing/AbstractDynamicAnimationThing.hpp"
#include "Doom/Thing/AbstractStaticPhysicsThing.hpp"

namespace DOOM
{
  template<unsigned int FrameDuration = 6>
  class DecorationThing : public DOOM::AbstractStaticPhysicsThing, public DOOM::AbstractDynamicAnimationThing<FrameDuration>
  {    
  public:
    DecorationThing(DOOM::Doom & doom, const std::string & sprite, const std::string & sequence, const DOOM::Wad::RawLevel::Thing & thing, int16_t height, int16_t radius, int16_t properties) :
      DOOM::AbstractThing(doom, thing, height, radius, properties),
      DOOM::AbstractStaticPhysicsThing(doom, thing, height, radius, properties),
      DOOM::AbstractDynamicAnimationThing<FrameDuration>(doom, sprite, sequence, thing, height, radius, properties)
    {}

    virtual ~DecorationThing() = default;

    virtual bool	update(DOOM::Doom & doom, sf::Time elapsed) override
    {
      return DOOM::AbstractStaticPhysicsThing::update(doom, elapsed) || DOOM::AbstractDynamicAnimationThing<FrameDuration>::update(doom, elapsed);
    }

    using DOOM::AbstractDynamicAnimationThing<FrameDuration>::sprite;
    using DOOM::AbstractStaticPhysicsThing::thrust;
  };
};

#endif