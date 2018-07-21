#ifndef _LIGHT_TRIGGERABLE_LINEDEF_HPP_
#define _LIGHT_TRIGGERABLE_LINEDEF_HPP_

#include "Doom/Doom.hpp"
#include "Doom/Linedef/NullLinedef.hpp"

namespace DOOM
{
  template<
    DOOM::EnumLinedef::Trigger Trigger,
    DOOM::EnumLinedef::Light Light,
    DOOM::EnumLinedef::Repeat Repeat
  >
  class LightTriggerableLinedef : public DOOM::Doom::Level::AbstractLinedef
  {
  private:
    inline void	triggerLight(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector)	// Perform light change on sector
    {
      // Cancel if a lighting action is already running
      if (sector.action<DOOM::EnumAction::Type::TypeLighting>().get() != nullptr)
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
	light = sector.getNeighborLowestLight(doom);
	break;
      case DOOM::EnumLinedef::Light::LightMaximum:
	light = sector.getNeighborHighestLight(doom);
	break;
      }

      // Set new light level of sector
      sector.light_base = light;
      sector.light_current = light;
    }

    template<DOOM::EnumLinedef::Repeat _Repeat = DOOM::EnumLinedef::Repeat::RepeatTrue>
    inline std::enable_if_t<Repeat != _Repeat>	triggerRepeat(DOOM::Doom & doom)	// Replace linedef with null if not repeatable
    {
      // Replace current sector by a normal sector
      for (std::unique_ptr<DOOM::Doom::Level::AbstractLinedef> & linedef : doom.level.linedefs)
	if (linedef.get() == this) {
	  linedef = std::make_unique<DOOM::NullLinedef>(doom, *this);
	  return;
	}

      // Linedef not found
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }

    template<DOOM::EnumLinedef::Repeat _Repeat = DOOM::EnumLinedef::Repeat::RepeatTrue>
    inline std::enable_if_t<Repeat == _Repeat>	triggerRepeat(DOOM::Doom & doom)	// Does nothing if repeatable
    {}

    template<DOOM::EnumLinedef::Trigger _Trigger = DOOM::EnumLinedef::Trigger::TriggerPushed>
    inline std::enable_if_t<(Trigger & _Trigger) != 0>	triggerSector(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing)	// Trigger sector of second sidedef
    {
      // Handle error
      if (back == -1)
	throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

      // Trigger sector of second sidedef
      triggerLight(doom, *doom.level.sectors[_doom.level.sidedefs[back].sector].get());
    }

    template<DOOM::EnumLinedef::Trigger _Trigger = DOOM::EnumLinedef::Trigger::TriggerPushed>
    inline std::enable_if_t<(Trigger & _Trigger) == 0>	triggerSector(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing)	// Trigger tagged sectors
    {
      // Trigger tagged sectors
      for (DOOM::Doom::Level::Sector & sector : doom.level.sectors)
	if (sector.tag == tag)
	  triggerLight(doom, sector);
    }

    template<DOOM::EnumLinedef::Trigger _Trigger>
    inline std::enable_if_t<(Trigger & _Trigger) == 0>	trigger(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing)	// Does nothing if wrong event triggered
    {}

    template<DOOM::EnumLinedef::Trigger _Trigger>
    inline std::enable_if_t<(Trigger & _Trigger) != 0>	trigger(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing)	// Trigger event if correct event triggered
    {
      // Trigger associated sector(s)
      triggerSector(doom, thing);

      // Handle non-repeatable trigger
      triggerRepeat(doom);
    }

  public:
    LightTriggerableLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
      DOOM::Doom::Level::AbstractLinedef(doom, linedef)
    {}

    ~LightTriggerableLinedef() = default;

    virtual void	update(DOOM::Doom & doom, sf::Time elapsed) override	// Update linedef
    {
      // TODO: remove this (note: may crash because of deleted instance)
      pushed(doom, *doom.level.things.front().get());
      switched(doom, *doom.level.things.front().get());
      walkover(doom, *doom.level.things.front().get());
      gunfire(doom, *doom.level.things.front().get());
    }

    virtual void	pushed(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing) override	// To call when linedef is pushed by thing
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerPushed>(doom, thing);
    }

    virtual void	switched(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing) override	// To call when linedef is switched (used) by thing
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerSwitched>(doom, thing);
    }

    virtual void	walkover(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing) override	// To call when thing walk over the linedef
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerWalkover>(doom, thing);
    }

    virtual void	gunfire(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing) override	// To call when thing shot the linedef
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerGunfire>(doom, thing);
    }
  };
};

#endif