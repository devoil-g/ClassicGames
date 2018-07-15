#ifndef _ACTION_TRIGGERABLE_LINEDEF_HPP_
#define _ACTION_TRIGGERABLE_LINEDEF_HPP_

#include <SFML/System/Time.hpp>

#include "Doom/Doom.hpp"
#include "Doom/Linedef/AbstractLinedef.hpp"
#include "Doom/Linedef/NullLinedef.hpp"

namespace DOOM
{
  template<
    DOOM::EnumLinedef::Trigger Trigger,
    DOOM::EnumLinedef::Action Action,
    DOOM::EnumLinedef::Repeat Repeat,
    DOOM::EnumLinedef::Monster Monster = DOOM::EnumLinedef::Monster::MonsterFalse,
    DOOM::EnumLinedef::Key Key = DOOM::EnumLinedef::Key::KeyNone
  >
  class ActionTriggerableLinedef : public DOOM::AbstractLinedef
  {
  private:
    DOOM::Doom &	_doom;	// Reference to game instance

    template<DOOM::EnumLinedef::Action _Action = DOOM::EnumLinedef::Action::ActionLeveling>
    inline std::enable_if_t<Action == _Action>	triggerAction(DOOM::Doom::Level::Sector & sector)	// Push leveling action on sector
    {
      sector.leveling(type);
    }

    template<DOOM::EnumLinedef::Action _Action = DOOM::EnumLinedef::Action::ActionLeveling>
    inline std::enable_if_t<Action != _Action>	triggerAction(DOOM::Doom::Level::Sector & sector)	// Push lighting action on sector
    {
      sector.lighting(type);
    }

    template<DOOM::EnumLinedef::Key _Key = DOOM::EnumLinedef::Key::KeyNone>
    inline std::enable_if_t<Key == _Key, bool>	triggerKey(const DOOM::AbstractThing & thing)	// No check for key
    {
      return true;
    }

    template<DOOM::EnumLinedef::Key _Key = DOOM::EnumLinedef::Key::KeyNone>
    inline std::enable_if_t<Key != _Key, bool>	triggerKey(const DOOM::AbstractThing & thing)	// Check if player has the correct key
    {
      // TODO: check key
      return true;
    }
    
    template<DOOM::EnumLinedef::Monster _Monster = DOOM::EnumLinedef::Monster::MonsterTrue>
    inline std::enable_if_t<Monster == _Monster, bool>	triggerMonster(const DOOM::AbstractThing & thing)	// Trigger sector of second sidedef
    {
      // TODO: check monster
      return true;
    }

    template<DOOM::EnumLinedef::Monster _Monster = DOOM::EnumLinedef::Monster::MonsterTrue>
    inline std::enable_if_t<Monster != _Monster, bool>	triggerMonster(const DOOM::AbstractThing & thing)	// Trigger sector of second sidedef
    {
      // TODO: check monster
      return true;
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
      {
	return;

	// TODO: solve problem of level 20
	throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      }

      // Trigger sector of second sidedef
      triggerAction(_doom.level.sectors[_doom.level.sidedefs[back].sector]);
    }

    template<DOOM::EnumLinedef::Trigger _Trigger = DOOM::EnumLinedef::Trigger::TriggerPushed>
    inline std::enable_if_t<(Trigger & _Trigger) == 0>	triggerSector(const DOOM::AbstractThing & thing)	// Trigger tagged sectors
    {
      // Trigger tagged sectors
      for (DOOM::Doom::Level::Sector & sector : _doom.level.sectors)
	if (sector.tag == tag)
	  triggerAction(sector);
    }

    template<DOOM::EnumLinedef::Trigger _Trigger>
    inline std::enable_if_t<(Trigger & _Trigger) == 0>	trigger(const DOOM::AbstractThing & thing)	// Does nothing if wrong event triggered
    {}

    template<DOOM::EnumLinedef::Trigger _Trigger>
    inline std::enable_if_t<(Trigger & _Trigger) != 0>	trigger(const DOOM::AbstractThing & thing)	// Trigger event if correct event triggered
    {
      // Check for monster condition
      if (triggerMonster(thing) == false)
	return;

      // Check for key condition
      if (triggerKey(thing) == false)
	return;

      // Trigger associated sector(s)
      triggerSector(thing);

      // Handle non-repeatable trigger
      triggerRepeat();
    }

  public:
    ActionTriggerableLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
      DOOM::AbstractLinedef(linedef),
      _doom(doom)
    {}

    ~ActionTriggerableLinedef()
    {}

    virtual void	update(sf::Time elapsed) override	// Update linedef
    {
      pushed(*_doom.level.things.front().get());
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