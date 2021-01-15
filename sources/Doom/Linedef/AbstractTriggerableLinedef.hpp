#pragma once

#include <iostream>

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

    enum Target
    {
      TargetNone = 0b0000,
      TargetPlayer = 0b0001,
      TargetMonster = 0b0010,
      TargetPlayerMonster = 0b0011
    };
  };

  template<
    DOOM::EnumLinedef::Trigger Trigger,
    bool Repeat,
    DOOM::EnumLinedef::Target Target = DOOM::EnumLinedef::Target::TargetPlayer,
    DOOM::Enum::KeyColor Key = DOOM::Enum::KeyColor::KeyColorNone
  >
    class AbstractTriggerableLinedef : public DOOM::AbstractLinedef
  {
  private:
    virtual bool  trigger(DOOM::Doom& doom, DOOM::AbstractThing& thing, int16_t sector_index) = 0;  // Action of the linedef

    template<DOOM::Enum::KeyColor _Key = DOOM::Enum::KeyColor::KeyColorNone>
    inline std::enable_if_t<Key == _Key, bool>  triggerKey(DOOM::Doom& doom, DOOM::AbstractThing& thing)  // No check for key
    {
      // No need for check
      return true;
    }

    template<DOOM::Enum::KeyColor _Key = DOOM::Enum::KeyColor::KeyColorNone>
    inline std::enable_if_t<Key != _Key, bool>  triggerKey(DOOM::Doom& doom, DOOM::AbstractThing& thing)  // Check if player has the correct key
    {
      // Only players can open locked doors
      if (thing.type != DOOM::Enum::ThingType::ThingType_PLAYER)
        return false;

      // Check if thing has the key
      if (thing.key(Key) == false) {
        doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_oof);
        return false;
      }

      return true;
    }

    template<bool _Repeat = true>
    inline std::enable_if_t<Repeat != _Repeat>  triggerRepeat(DOOM::Doom& doom) // Replace linedef with null if not repeatable
    {
      // Replace current linedef by a normal linedef
      for (std::unique_ptr<DOOM::AbstractLinedef>& linedef : doom.level.linedefs)
        if (linedef.get() == this) {
          linedef = std::make_unique<DOOM::NullLinedef>(doom, *this);
          return;
        }

      // Linedef not found
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }

    template<bool _Repeat = true>
    inline std::enable_if_t<Repeat == _Repeat>  triggerRepeat(DOOM::Doom& doom) // Does nothing if repeatable
    {}

    template<DOOM::EnumLinedef::Trigger _Trigger = DOOM::EnumLinedef::Trigger::TriggerPushed>
    inline std::enable_if_t<(Trigger & _Trigger) != 0, bool>  triggerSector(DOOM::Doom& doom, DOOM::AbstractThing& thing) // Trigger sector of second sidedef
    {
      // Handle error
      if (back == -1)
        return false;

      // Trigger sector of second sidedef
      return trigger(doom, thing, doom.level.sidedefs[back].sector);
    }

    template<DOOM::EnumLinedef::Trigger _Trigger = DOOM::EnumLinedef::Trigger::TriggerPushed>
    inline std::enable_if_t<(Trigger & _Trigger) == 0, bool>  triggerSector(DOOM::Doom& doom, DOOM::AbstractThing& thing) // Trigger tagged sectors
    {
      // Handle invalid tag
      if (tag == 0)
        return false;

      bool	result = false;

      // Trigger tagged sectors
      for (int16_t index = 0; index < doom.level.sectors.size(); index++)
        if (doom.level.sectors[index].tag == tag)
          result |= trigger(doom, thing, index);

      return result;
    }

    template<DOOM::EnumLinedef::Trigger _Trigger>
    inline std::enable_if_t<(Trigger & _Trigger) == 0, bool>  trigger(DOOM::Doom& doom, DOOM::AbstractThing& thing) // Does nothing if wrong event triggered
    {
      return false;
    }

    template<DOOM::EnumLinedef::Trigger _Trigger>
    inline std::enable_if_t<(Trigger & _Trigger) != 0, bool>  trigger(DOOM::Doom& doom, DOOM::AbstractThing& thing) // Trigger event if correct event triggered
    {
      // Check for monster condition
      if (!(((Target & DOOM::EnumLinedef::Target::TargetPlayer) && (thing.type == DOOM::Enum::ThingType::ThingType_PLAYER)) ||
        ((Target & DOOM::EnumLinedef::Target::TargetMonster) && (thing.type != DOOM::Enum::ThingType::ThingType_PLAYER) && (thing.attributs.properties & DOOM::Enum::ThingProperty::ThingProperty_Shootable))))
        return false;

      // Check for key condition
      if (triggerKey(doom, thing) == false)
        return false;

      // Trigger associated sector(s) and handle non-repeatable trigger
      if (triggerSector(doom, thing) == false)
        return false;

      // Find which sidedef textures should be switched
      int16_t	sidedef = ((Math::Vector<2>::determinant(doom.level.vertexes[start] - thing.position.convert<2>(), doom.level.vertexes[end] - doom.level.vertexes[start]) < 0.f) ? front : back);

      // Switch sidedef if trigger pushed or switched, definitively if not repeatable
      if (_Trigger == DOOM::EnumLinedef::Trigger::TriggerPushed || _Trigger == DOOM::EnumLinedef::Trigger::TriggerSwitched && sidedef != -1)
        doom.level.sidedefs[sidedef].switched(doom, (Repeat == false) ? (sf::Time::Zero) : (sf::seconds(DOOM::Doom::Tic.asSeconds() * 35)));

      // Remove lindef if non-repeatable
      triggerRepeat(doom);

      return true;
    }

  public:
    AbstractTriggerableLinedef(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Linedef& linedef) :
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

    virtual void  update(DOOM::Doom& doom, sf::Time elapsed) override // Update linedef
    {}

    virtual bool  switched(DOOM::Doom& doom, DOOM::AbstractThing& thing) override // To call when linedef is switched (used) by thing
    {
      return trigger<DOOM::EnumLinedef::Trigger::TriggerPushed>(doom, thing) ||
        trigger<DOOM::EnumLinedef::Trigger::TriggerSwitched>(doom, thing);
    }

    virtual bool  walkover(DOOM::Doom& doom, DOOM::AbstractThing& thing) override // To call when thing walk over the linedef
    {
      return trigger<DOOM::EnumLinedef::Trigger::TriggerWalkover>(doom, thing);
    }

    virtual bool  gunfire(DOOM::Doom& doom, DOOM::AbstractThing& thing) override  // To call when thing shot the linedef
    {
      return trigger<DOOM::EnumLinedef::Trigger::TriggerGunfire>(doom, thing);
    }
  };
}