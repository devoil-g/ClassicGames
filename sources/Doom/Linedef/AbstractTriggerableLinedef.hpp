#ifndef _ABSTRACT_TRIGGERABLE_LINEDEF_HPP_
#define _ABSTRACT_TRIGGERABLE_LINEDEF_HPP_

#include "Doom/Linedef/AbstractLinedef.hpp"
#include "Doom/Linedef/NullLinedef.hpp"

namespace DOOM
{
  namespace EnumLinedef
  {
    enum Trigger
    {
      TriggerNone = 0b0000,
      TriggerPushed = 0b0001,
      TriggerSwitched = 0b0010,
      TriggerWalkover = 0b0100,
      TriggerGunfire = 0b1000
    };

    enum Key
    {
      KeyNone,
      KeyBlue,
      KeyRed,
      KeyYellow,
    };
  };

  template<
    DOOM::EnumLinedef::Trigger Trigger,
    bool Repeat,
    bool Monster = false,
    DOOM::EnumLinedef::Key Key = DOOM::EnumLinedef::Key::KeyNone
  >
  class AbstractTriggerableLinedef : public DOOM::AbstractLinedef
  {
  private:
    virtual void	trigger(DOOM::Doom & doom, DOOM::AbstractThing & thing, int16_t sector_index) = 0;	// Action of the linedef

    template<DOOM::EnumLinedef::Key _Key = DOOM::EnumLinedef::Key::KeyNone>
    inline std::enable_if_t<Key == _Key, bool>	triggerKey(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// No check for key
    {
      // No need for check
      return true;
    }

    template<DOOM::EnumLinedef::Key _Key = DOOM::EnumLinedef::Key::KeyNone>
    inline std::enable_if_t<Key != _Key, bool>	triggerKey(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Check if player has the correct key
    {
      // TODO: check key
      return true;
    }

    template<bool _Monster = true>
    inline std::enable_if_t<Monster == _Monster, bool>	triggerMonster(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Trigger sector of second sidedef
    {
      // TODO: check monster & player
      return true;
    }

    template<bool _Monster = true>
    inline std::enable_if_t<Monster != _Monster, bool>	triggerMonster(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Trigger sector of second sidedef
    {
      // TODO: check player
      return true;
    }

    template<bool _Repeat = true>
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

    template<bool _Repeat = true>
    inline std::enable_if_t<Repeat == _Repeat>	triggerRepeat(DOOM::Doom & doom)	// Does nothing if repeatable
    {}

    template<DOOM::EnumLinedef::Trigger _Trigger = DOOM::EnumLinedef::Trigger::TriggerPushed>
    inline std::enable_if_t<(Trigger & _Trigger) != 0>	triggerSector(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Trigger sector of second sidedef
    {
      // Handle error
      if (back == -1)
	return;

      // Trigger sector of second sidedef
      trigger(doom, thing, doom.level.sidedefs[back].sector);
    }

    template<DOOM::EnumLinedef::Trigger _Trigger = DOOM::EnumLinedef::Trigger::TriggerPushed>
    inline std::enable_if_t<(Trigger & _Trigger) == 0>	triggerSector(DOOM::Doom & doom, DOOM::AbstractThing & thing)	// Trigger tagged sectors
    {
      // Handle invalid tag
      if (tag == 0)
	return;

      // Trigger tagged sectors
      for (int16_t index = 0; index < doom.level.sectors.size(); index++)
	if (doom.level.sectors[index].tag == tag)
	  trigger(doom, thing, index);
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
    AbstractTriggerableLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
      DOOM::AbstractLinedef(doom, linedef)
    {
      // Check for inconstitency
      if (Trigger & DOOM::EnumLinedef::Trigger::TriggerPushed) {
	if (back == -1)
	  std::cerr << "[DOOM::AbstractTriggerableLinedef] Warning, no sector for action (type #" << type << ")." << std::endl;
	if (tag != 0)
	  std::cerr << "[DOOM::AbstractTriggerableLinedef] Warning, useless tag for sector action (type #" << type << ")." << std::endl;
      }
      else {
	if (tag == 0)
	  std::cerr << "[DOOM::AbstractTriggerableLinedef] Warning, no tag for sector action (type #" << type << ")." << std::endl;
      }
    }

    ~AbstractTriggerableLinedef() = default;

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