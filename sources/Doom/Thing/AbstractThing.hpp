#ifndef _ABSTRACT_THING_HPP_
#define _ABSTRACT_THING_HPP_

#include "Doom/Doom.hpp"

namespace DOOM
{
  class PlayerThing;

  class AbstractThing
  {
  public:
    static std::unique_ptr<DOOM::AbstractThing>	factory(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing);

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

    Math::Vector<3>	position;	// Thing position
    float		angle;		// Thing orientation (rad.)

    const int16_t	height;		// Thing height
    const int16_t	type;		// Thing type ID
    const int16_t	flag;		// Thing flags (see enum)
    const int16_t	radius;		// Thing radius (square box)
    const int16_t	properties;	// Thing properties (see enum)

  protected:
    AbstractThing(DOOM::Doom & doom, int16_t height, int16_t radius, int16_t properties);	// Special constructor for player only

  public:
    AbstractThing(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing, int16_t height, int16_t radius, int16_t properties);
    virtual ~AbstractThing() = default;

    virtual bool											update(DOOM::Doom & doom, sf::Time elapsed) = 0;	// Update thing, return true if thing should be deleted
    virtual void											thrust(const Math::Vector<2> & acceleration) = 0;	// Apply acceleration to thing
    virtual const std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool> &	sprite(float angle) const = 0;				// Return sprite to be displayed
  };
};

#endif