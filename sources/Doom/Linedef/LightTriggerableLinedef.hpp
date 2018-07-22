#ifndef _LIGHT_TRIGGERABLE_LINEDEF_HPP_
#define _LIGHT_TRIGGERABLE_LINEDEF_HPP_

#include "Doom/Linedef/AbstractTriggerableLinedef.hpp"

namespace DOOM
{
  template<
    DOOM::EnumLinedef::Light Light,
    DOOM::EnumLinedef::Trigger Trigger,
    DOOM::EnumLinedef::Repeat Repeat
  >
  class LightTriggerableLinedef : public DOOM::AbstractTriggerableLinedef<Trigger, Repeat>
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

    template<DOOM::EnumLinedef::Trigger _Trigger = DOOM::EnumLinedef::Trigger::TriggerPushed>
    inline std::enable_if_t<(Trigger & _Trigger) != 0>	triggerSector(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Trigger sector of second sidedef
    {
      // Handle error
      if (back == -1)
      {
	std::cerr << "[LightTriggerableLinedef:trigger] Warning, invalid pushed type (" << type << ")." << std::endl;
	return;

	// TODO: solve problem of level 20
	throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      }

      // Trigger sector of second sidedef
      triggerLight(doom, doom.level.sectors[_doom.level.sidedefs[back].sector]);
    }

    template<DOOM::EnumLinedef::Trigger _Trigger = DOOM::EnumLinedef::Trigger::TriggerPushed>
    inline std::enable_if_t<(Trigger & _Trigger) == 0>	triggerSector(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Trigger tagged sectors
    {
      // Handle error
      if (tag == 0)
      {
	std::cerr << "[LightTriggerableLinedef:trigger] Warning, invalid pushed tag (" << tag << ")." << std::endl;
	return;

	// TODO: solve problem of level 20
	throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      }

      // Trigger tagged sectors
      for (DOOM::Doom::Level::Sector & sector : doom.level.sectors)
	if (sector.tag == tag)
	  triggerLight(doom, sector);
    }

    void	trigger(DOOM::Doom & doom, DOOM::AbstractThing & thing) override	// Action of the linedef
    {
      // Push action in sector(s)
      triggerSector(doom, thing);
    }

  public:
    LightTriggerableLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
      DOOM::AbstractTriggerableLinedef<Trigger, Repeat>(doom, linedef)
    {}

    ~LightTriggerableLinedef() = default;
  };
};

#endif