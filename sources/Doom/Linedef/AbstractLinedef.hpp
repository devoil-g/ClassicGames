#pragma once

#include <memory>

#include "Doom/Doom.hpp"

namespace DOOM
{
  class Doom;

  class AbstractLinedef
  {
  public:
    static std::unique_ptr<DOOM::AbstractLinedef> factory(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Linedef& linedef);

    enum Flag
    {
      Impassible = 0x0001,    // Players and monsters cannot cross this line
      BlockMonsters = 0x0002, // Monsters cannot cross this line
      TwoSided = 0x0004,      // See documentation
      UpperUnpegged = 0x0008, // The upper texture is pasted onto the wall from the top down instead of from the bottom up like usual
      LowerUnpegged = 0x0010, // Lower and middle textures are drawn from the bottom up, instead of from the top down like usual
      Secret = 0x0020,        // See documentation
      BlockSound = 0x0040,    // For purposes of monsters hearing sounds and thus becoming alerted
      NotOnMap = 0x0080,      // The line is not shown on the automap
      AlreadyOnMap = 0x0100,  // When the level is begun, this line is already on the automap

      OnMap = 0x0200          // [CUSTOM] Enabled when wall has been rendered (for automap)
    };

    int16_t start, end;   // Start and end vertexes
    int16_t flag;         // Linedef flag (see enum)
    int16_t type;         // Linedef type (see enum)
    int16_t tag;          // Linedef sector tag
    int16_t front, back;  // Front (right) and back (left) sidedefs indexes (-1 if no sidedef)

    AbstractLinedef(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Linedef& linedef);
    AbstractLinedef(DOOM::Doom& doom, const DOOM::AbstractLinedef& linedef);
    virtual ~AbstractLinedef() = default;

    virtual void  update(DOOM::Doom& doom, sf::Time elapsed) = 0; // Update linedef

    virtual bool  switched(DOOM::Doom& doom, DOOM::AbstractThing& thing); // To call when linedef is switched (used) by thing, return true if an action has been executed
    virtual bool  walkover(DOOM::Doom& doom, DOOM::AbstractThing& thing); // To call when thing walk over the linedef, return true if an action has been executed
    virtual bool  gunfire(DOOM::Doom& doom, DOOM::AbstractThing& thing);  // To call when thing shot the linedef, return true if an action has been executed
  };
}