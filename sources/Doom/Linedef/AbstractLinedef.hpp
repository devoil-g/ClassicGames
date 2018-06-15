#ifndef _ABSTRACT_LINEDEF_HPP_
#define _ABSTRACT_LINEDEF_HPP_

#include <SFML/System/Time.hpp>

#include "Doom/Doom.hpp"
#include "Doom/Thing/AbstractThing.hpp"

namespace DOOM
{
  class AbstractLinedef
  {
  public:
    static DOOM::AbstractLinedef *	factory(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef);

    enum Flag
    {
      Impassible = 0x0001,	// Players and monsters cannot cross this line
      BlockMonsters = 0x0002,	// Monsters cannot cross this line
      TwoSided = 0x0004,	// See documentation
      UpperUnpegged = 0x0008,	// The upper texture is pasted onto the wall from the top down instead of from the bottom up like usual
      LowerUnpegged = 0x0010,	// Lower and middle textures are drawn from the bottom up, instead of from the top down like usual
      Secret = 0x0020,		// See documentation
      BlockSound = 0x0040,	// For purposes of monsters hearing sounds and thus becoming alerted
      NotOnMap = 0x0080,	// The line is not shown on the automap
      AlreadyOnMap = 0x0100	// When the level is begun, this line is already on the automap
    };

    int16_t	start, end;	// Start and end vertexes
    int16_t	flag;		// Linedef flag (see enum)
    int16_t	type;		// Linedef type (see enum)
    int16_t	tag;		// Linedef sector tag
    int16_t	front, back;	// Front and back sidedefs indexes (-1 if no sidedef)

    AbstractLinedef(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef);
    virtual ~AbstractLinedef();

    virtual void	update(sf::Time elapsed) = 0;	// Update linedef

    virtual void	pushed(const DOOM::AbstractThing & thing);	// To call when linedef is pushed by thing
    virtual void	switched(const DOOM::AbstractThing & thing);	// To call when linedef is switched (used) by thing
    virtual void	walkover(const DOOM::AbstractThing & thing);	// To call when thing walk over the linedef
    virtual void	gunfire(const DOOM::AbstractThing & thing);	// To call when thing shot the linedef
  };
};

#endif