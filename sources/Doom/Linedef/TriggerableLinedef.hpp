#ifndef _TRIGGERABLE_LINEDEF_HPP_
#define _TRIGGERABLE_LINEDEF_HPP_

#include <type_traits>

#include <SFML/System/Time.hpp>

#include "Doom/Doom.hpp"
#include "Doom/Linedef/AbstractLinedef.hpp"
#include "Doom/Sector/AbstractSector.hpp"

namespace DOOM
{
  template<int Trigger = 0, bool Repeat = false>
  class TriggerableLinedef : public DOOM::AbstractLinedef
  {
  public:
    enum EnumTrigger
    {
      TriggerNone = 0b0000,
      TriggerPushed = 0b0001,
      TriggerSwitched = 0b0010,
      TriggerWalkover = 0b0100,
      TriggerGunfire = 0b1000
    };

  private:
    DOOM::Doom &	_doom;		// Reference to game instance
    bool		_triggered;	// True if already triggered

    inline bool	triggerable()	// Check if action can be triggered
    {
      if (Repeat == true)
	return true;
      else
	return (_triggered == true) ? (false) : (_triggered = true);
    }

    template<EnumTrigger _Trigger>
    void	trigger()
    {
      if (!(_Trigger & Trigger))
	return;

      // Return if action can't be repeated
      if (triggerable() == false)
	return;

      // SWITCHED, WALKOVER & GUNFIRE: Trigger tagged sectors
      if ((_Trigger & TriggerSwitched) || (_Trigger & TriggerWalkover) || (_Trigger & TriggerGunfire)) {
	for (const std::unique_ptr<DOOM::AbstractSector> & sector : _doom.level.sectors)
	  if (sector->tag == tag)
	    sector->action(type);
      }

      // PUSHED: Trigger sector of second sidedef
      else if (_Trigger & TriggerPushed) {
	if (back != -1)
	  _doom.level.sectors[_doom.level.sidedefs[back].sector]->action(type);
      }


      // TODO: pushed affect sector on the other side (no tag)
    }

  public:
    TriggerableLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
      DOOM::AbstractLinedef(linedef),
      _doom(doom),
      _triggered(false)
    {}

    ~TriggerableLinedef()
    {}

    void	update(sf::Time elapsed)	// Update linedef
    {}

    void	pushed(const DOOM::AbstractThing & thing) override	// To call when linedef is pushed by thing
    {
      trigger<TriggerPushed>();
    }

    void	switched(const DOOM::AbstractThing & thing) override	// To call when linedef is switched (used) by thing
    {
      trigger<TriggerSwitched>();
    }

    void	walkover(const DOOM::AbstractThing & thing) override	// To call when thing walk over the linedef
    {
      trigger<TriggerWalkover>();
    }

    void	gunfire(const DOOM::AbstractThing & thing) override	// To call when thing shot the linedef
    {
      trigger<TriggerGunfire>();
    }
  };
};

#endif