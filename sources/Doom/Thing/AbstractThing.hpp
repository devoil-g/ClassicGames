#ifndef _ABSTRACT_THING_HPP_
#define _ABSTRACT_THING_HPP_

#include <utility>
#include <vector>

#include <SFML/System/Time.hpp>

#include "Doom/Doom.hpp"
#include "Math/Vector.hpp"

namespace DOOM
{
  class AbstractThing
  {
  public:
    static std::unique_ptr<DOOM::AbstractThing>	factory(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing);

    enum Flag : int16_t
    {
      SkillLevel12 = 0x0001,	// Active on skill level 1 & 2
      SkillLevel3 = 0x0002,	// Active on skill level 3
      SkillLevel45 = 0x0004,	// Active on skill level 4 & 5
      Ambush = 0x0008,		// Monster is in ambush mode
      Multiplayer = 0x0010	// Only active in multiplayer mode
    };

    enum Properties : int16_t
    {
      None = 0x0000,		// No properties
      Artifact = 0x0001,	// Artifact item, counts toward ITEMS percentage at the end of a level
      Hanging = 0x0002,		// Hangs from ceiling, or floats if a monster
      Monster = 0x0004,		// Monster, counts towards kill percentage
      Obstacle = 0x0008,	// Obstacle, players and monsters must walk around
      Pickup = 0x0010		// Pickup, player can pick the thing up by walking over it
    };

    Math::Vector<2>	position;	// Thing position
    float		height;		// Thing height from floor (or ceiling is hanging)
    float		angle;		// Thing orientation (rad.)
    const int16_t	type;		// Thing flags (see enum)
    const int16_t	flag;		// Thing flags (see enum)
    const int16_t	radius;		// Thing radius (square box)
    const int16_t	properties;	// Thing properties (see enum)

    AbstractThing(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing, int16_t radius, int16_t properties);
    virtual ~AbstractThing() = 0;

    virtual bool								update(sf::Time elapsed);	// Update thing, return true if thing should be deleted
    virtual const std::pair<DOOM::Doom::Resources::Texture const *, bool> &	sprite(float angle) const = 0;	// Return sprite to be displayed
  };
};

#endif