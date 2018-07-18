#ifndef _LIGHT_TRIGGERABLE_LINEDEF_HPP_
#define _LIGHT_TRIGGERABLE_LINEDEF_HPP_

#include <SFML/System/Time.hpp>

#include "Doom/Doom.hpp"
#include "Doom/Linedef/AbstractLinedef.hpp"
#include "Doom/Linedef/NullLinedef.hpp"

#include <iostream>

namespace DOOM
{
  template<
    DOOM::EnumLinedef::Trigger Trigger,
    DOOM::EnumLinedef::Light Light,
    DOOM::EnumLinedef::Repeat Repeat
  >
  class LightTriggerableLinedef : public DOOM::AbstractLinedef
  {
  private:
    DOOM::Doom &	_doom;	// Reference to game instance

    inline void	triggerLight(DOOM::Doom::Level::Sector & sector)	// Perform light change on sector
    {
      // Cancel if a lighting action is already running
      if (sector.lighting().get() != nullptr)
	return;

      int16_t	light;

      // Get new light level of sector
      switch (Light)
      {
      case DOOM::EnumLinedef::Light::Light35:
	light = 35;
	break;
      case DOOM::EnumLinedef::Light::Light255:
	light = 255;
	break;
      case DOOM::EnumLinedef::Light::LightMinimum:
	light = sector.getNeighborLowestLight();
	break;
      case DOOM::EnumLinedef::Light::LightMaximum:
	light = sector.getNeighborHighestLight();
	break;
      }

      // Set new light level of sector
      sector.baseLight() = light;
      sector.light() = light;
    }

    template<DOOM::EnumLinedef::Repeat _Repeat = DOOM::EnumLinedef::Repeat::RepeatTrue>
    inline std::enable_if_t<Repeat != _Repeat>	triggerRepeat()	// Replace linedef with null if not repeatable
    {
      // Replace current sector by a normal sector
      for (std::unique_ptr<DOOM::AbstractLinedef> & linedef : _doom.level.linedefs)
	if (linedef.get() == this) {
	  linedef.reset(new DOOM::NullLinedef(*this));
	  return;
	}

      // Linedef not found
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }

    template<DOOM::EnumLinedef::Repeat _Repeat = DOOM::EnumLinedef::Repeat::RepeatTrue>
    inline std::enable_if_t<Repeat == _Repeat>	triggerRepeat()	// Does nothing if repeatable
    {}

    template<DOOM::EnumLinedef::Trigger _Trigger = DOOM::EnumLinedef::Trigger::TriggerPushed>
    inline std::enable_if_t<(Trigger & _Trigger) != 0>	triggerSector(const DOOM::AbstractThing & thing)	// Trigger sector of second sidedef
    {
      // Handle error
      if (back == -1)
	throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

      // Trigger sector of second sidedef
      triggerLight(*_doom.level.sectors[_doom.level.sidedefs[back].sector].get());
    }

    template<DOOM::EnumLinedef::Trigger _Trigger = DOOM::EnumLinedef::Trigger::TriggerPushed>
    inline std::enable_if_t<(Trigger & _Trigger) == 0>	triggerSector(const DOOM::AbstractThing & thing)	// Trigger tagged sectors
    {
      // Trigger tagged sectors
      for (DOOM::Doom::Level::Sector & sector : _doom.level.sectors)
	if (sector.tag == tag)
	  triggerLight(sector);
    }

    template<DOOM::EnumLinedef::Trigger _Trigger>
    inline std::enable_if_t<(Trigger & _Trigger) == 0>	trigger(const DOOM::AbstractThing & thing)	// Does nothing if wrong event triggered
    {}

    template<DOOM::EnumLinedef::Trigger _Trigger>
    inline std::enable_if_t<(Trigger & _Trigger) != 0>	trigger(const DOOM::AbstractThing & thing)	// Trigger event if correct event triggered
    {
      // Trigger associated sector(s)
      triggerSector(thing);

      // Handle non-repeatable trigger
      triggerRepeat();
    }

  public:
    LightTriggerableLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
      DOOM::AbstractLinedef(linedef),
      _doom(doom)
    {}

    ~LightTriggerableLinedef()
    {}

    virtual void	update(sf::Time elapsed) override	// Update linedef
    {
      pushed(*_doom.level.things.front().get());
      switched(*_doom.level.things.front().get());
      walkover(*_doom.level.things.front().get());
      gunfire(*_doom.level.things.front().get());
    }

    virtual void	pushed(const DOOM::AbstractThing & thing) override	// To call when linedef is pushed by thing
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerPushed>(thing);
    }

    virtual void	switched(const DOOM::AbstractThing & thing) override	// To call when linedef is switched (used) by thing
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerSwitched>(thing);
    }

    virtual void	walkover(const DOOM::AbstractThing & thing) override	// To call when thing walk over the linedef
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerWalkover>(thing);
    }

    virtual void	gunfire(const DOOM::AbstractThing & thing) override	// To call when thing shot the linedef
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerGunfire>(thing);
    }
  };
};

#endif