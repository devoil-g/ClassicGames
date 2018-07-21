#ifndef _ACTION_TRIGGERABLE_LINEDEF_HPP_
#define _ACTION_TRIGGERABLE_LINEDEF_HPP_

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
    template<DOOM::EnumLinedef::Action _Action = DOOM::EnumLinedef::Action::ActionLeveling>
    inline std::enable_if_t<Action == _Action>	triggerAction(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector)	// Push leveling action on sector
    {
      sector.action<DOOM::EnumAction::Type::TypeLeveling>(doom, type);
    }

    template<DOOM::EnumLinedef::Action _Action = DOOM::EnumLinedef::Action::ActionLeveling>
    inline std::enable_if_t<Action != _Action>	triggerAction(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector)	// Push lighting action on sector
    {
      sector.action<DOOM::EnumAction::Type::TypeLighting>(doom, type);
    }

    template<DOOM::EnumLinedef::Key _Key = DOOM::EnumLinedef::Key::KeyNone>
    inline std::enable_if_t<Key == _Key, bool>	triggerKey(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// No check for key
    {
      return true;
    }

    template<DOOM::EnumLinedef::Key _Key = DOOM::EnumLinedef::Key::KeyNone>
    inline std::enable_if_t<Key != _Key, bool>	triggerKey(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Check if player has the correct key
    {
      // TODO: check key
      return true;
    }
    
    template<DOOM::EnumLinedef::Monster _Monster = DOOM::EnumLinedef::Monster::MonsterTrue>
    inline std::enable_if_t<Monster == _Monster, bool>	triggerMonster(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Trigger sector of second sidedef
    {
      // TODO: check monster
      return true;
    }

    template<DOOM::EnumLinedef::Monster _Monster = DOOM::EnumLinedef::Monster::MonsterTrue>
    inline std::enable_if_t<Monster != _Monster, bool>	triggerMonster(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Trigger sector of second sidedef
    {
      // TODO: check monster
      return true;
    }

    template<DOOM::EnumLinedef::Repeat _Repeat = DOOM::EnumLinedef::Repeat::RepeatTrue>
    inline std::enable_if_t<Repeat != _Repeat>	triggerRepeat(DOOM::Doom & doom)	// Replace linedef with null if not repeatable
    {
      // Replace current sector by a normal sector
      for (std::unique_ptr<DOOM::AbstractLinedef> & linedef : doom.level.linedefs)
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
    inline std::enable_if_t<(Trigger & _Trigger) != 0>	triggerSector(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Trigger sector of second sidedef
    {
      // Handle error
      if (back == -1)
      {
	return;

	// TODO: solve problem of level 20
	throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      }

      // Trigger sector of second sidedef
      triggerAction(doom, doom.level.sectors[doom.level.sidedefs[back].sector]);
    }

    template<DOOM::EnumLinedef::Trigger _Trigger = DOOM::EnumLinedef::Trigger::TriggerPushed>
    inline std::enable_if_t<(Trigger & _Trigger) == 0>	triggerSector(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Trigger tagged sectors
    {
      // Trigger tagged sectors
      for (DOOM::Doom::Level::Sector & sector : doom.level.sectors)
	if (sector.tag == tag)
	  triggerAction(doom, sector);
    }

    template<DOOM::EnumLinedef::Trigger _Trigger>
    inline std::enable_if_t<(Trigger & _Trigger) == 0>	trigger(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Does nothing if wrong event triggered
    {}

    template<DOOM::EnumLinedef::Trigger _Trigger>
    inline std::enable_if_t<(Trigger & _Trigger) != 0>	trigger(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Trigger event if correct event triggered
    {
      // Check for monster condition
      if (triggerMonster(doom, thing) == false)
	return;

      // Check for key condition
      if (triggerKey(doom, thing) == false)
	return;

      // Trigger associated sector(s)
      triggerSector(doom, thing);

      // Handle non-repeatable trigger
      triggerRepeat(doom);
    }

  public:
    ActionTriggerableLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
      DOOM::AbstractLinedef(doom, linedef)
    {}

    ~ActionTriggerableLinedef() = default;

    virtual void	update(DOOM::Doom & doom, sf::Time elapsed) override	// Update linedef
    {
      // TODO: remove this (note: may crash because of deleted instance)
      pushed(doom, *doom.level.things.front().get());
      switched(doom, *doom.level.things.front().get());
      walkover(doom, *doom.level.things.front().get());
      gunfire(doom, *doom.level.things.front().get());
    }

    virtual void	pushed(DOOM::Doom & doom, DOOM::AbstractThing & thing) override	// To call when linedef is pushed by thing
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerPushed>(doom, thing);
    }

    virtual void	switched(DOOM::Doom & doom, DOOM::AbstractThing & thing) override	// To call when linedef is switched (used) by thing
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerSwitched>(doom, thing);
    }

    virtual void	walkover(DOOM::Doom & doom, DOOM::AbstractThing & thing) override	// To call when thing walk over the linedef
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerWalkover>(doom, thing);
    }

    virtual void	gunfire(DOOM::Doom & doom, DOOM::AbstractThing & thing) override	// To call when thing shot the linedef
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerGunfire>(doom, thing);
    }
  };
};

#endif