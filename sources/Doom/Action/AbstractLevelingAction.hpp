#pragma once

#include <cstdint>
#include <type_traits>
#include <iostream>

#include "Doom/Action/AbstractStoppableAction.hpp"
#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<
    bool Stoppable = false,
    DOOM::EnumAction::Change::Type ChangeType = DOOM::EnumAction::Change::Type::None,
    DOOM::EnumAction::Change::Time ChangeTime = DOOM::EnumAction::Change::Time::Before
  >
  class AbstractLevelingAction : public std::conditional<Stoppable == true,
    DOOM::AbstractStoppableAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>,
    DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>
  >::type
  {
  protected:
    float   updateFloorLower(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float elapsed, float target, float speed)
    {
      // Limit target to ceiling
      target = std::min(target, sector.ceiling_current);
      sector.floor_current = std::max(sector.floor_current, target);

      float start = sector.floor_current;

      // Lower floor
      sector.floor_current = std::max(target, sector.floor_current - elapsed * (speed / DOOM::Doom::Tic));

      // Lower things standing in the sector
      for (const auto& thing : doom.level.getThings(sector))
      {
        // Ignore if thing doesn't touch the floor or no gravity
        if (thing.get().position.z() > start || (thing.get().flags & (DOOM::Enum::ThingProperty::ThingProperty_Float | DOOM::Enum::ThingProperty::ThingProperty_NoGravity)))
          continue;

        float thing_floor = sector.floor_current;

        // Find highest floor below thing
        for (auto thing_index : doom.level.getSectors(thing.get()))
          thing_floor = std::max(thing_floor, doom.level.sectors[thing_index].floor_current);

        // Lower thing to new highest floor
        thing.get().position.z() = std::min(thing.get().position.z(), thing_floor);
      }

      // Compute remaining time
      if (sector.floor_current == target)
        return elapsed - std::max(0.f, (start - sector.floor_current) / (speed / DOOM::Doom::Tic));
      else
        return 0.f;
    }

    float   updateFloorRaise(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float elapsed, float target, float speed)
    {
      // Limit target to ceiling
      target = std::min(target, sector.ceiling_current);
      sector.floor_current = std::min(sector.floor_current, target);

      float start = sector.floor_current;
      auto  things = doom.level.getThings(sector);
      
      // Lower target to lowest shootable thing obstacle
      for (const auto& thing : things)
        if (thing.get().flags & DOOM::Enum::ThingProperty::ThingProperty_Shootable)
          for (auto thing_index : doom.level.getSectors(thing.get()))
            target = std::min(target, doom.level.sectors[thing_index].ceiling_current - thing.get().height);

      // Stop if already crushing things
      if (sector.floor_current >= target)
        return elapsed;

      // Raise floor
      sector.floor_current = std::min(target, sector.floor_current + elapsed * (speed / DOOM::Doom::Tic));

      // Raise things
      for (const auto& thing : things)
        thing.get().position.z() = std::max(thing.get().position.z(), std::min(thing.get().position.z() + sector.floor_current - start, sector.floor_current));

      // Compute remaining time
      if (sector.floor_current == target)
        return elapsed - std::max(0.f, (sector.floor_current - start) / (speed / DOOM::Doom::Tic));
      else
        return 0.f;
    }

    float   updateFloorCrush(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float elapsed, float target, float speed)
    {
      // Limit target to ceiling
      target = std::min(target, sector.ceiling_current);
      sector.floor_current = std::min(sector.floor_current, target);

      float start = sector.floor_current;
      auto  things = doom.level.getThings(sector);

      // Check that something is getting crushed
      {
        float thing_floor = sector.ceiling_current;

        // Get lowest thing to crush
        for (const auto& thing : things)
          if (thing.get().flags & DOOM::Enum::ThingProperty::ThingProperty_Shootable)
            for (auto thing_index : doom.level.getSectors(thing.get()))
              thing_floor = std::min(thing_floor, doom.level.sectors[thing_index].ceiling_current - thing.get().height);

        // Stop if nothing to crush
        if (sector.floor_current < thing_floor)
          return elapsed;
      }

      // Raise floor
      sector.floor_current = std::min(target, sector.floor_current + elapsed * (speed / DOOM::Doom::Tic));

      // Raise & crush things
      for (const auto& thing : things)
      {
        // Raise thing
        thing.get().position.z() = std::max(thing.get().position.z(), std::min(sector.floor_current, thing.get().position.z() + sector.floor_current - start));

        // Only crush shootable things
        if (!(thing.get().flags & DOOM::Enum::ThingProperty::ThingProperty_Shootable))
          continue;

        float thing_ceiling = sector.ceiling_current;

        // Get thing lowest ceiling
        for (auto thing_index : doom.level.getSectors(thing.get()))
          thing_ceiling = std::min(thing_ceiling, doom.level.sectors[thing_index].ceiling_current);

        float damage = std::min(sector.floor_current + thing.get().height - thing_ceiling, sector.floor_current - start) / (speed / DOOM::Doom::Tic) * 87.5f;

        // Deal crush damage (87.5/s)
        if (damage > 0.f)
          thing.get().damage(doom, damage);
      }

      // Compute remaining time
      if (sector.floor_current == target)
        return elapsed - std::max(0.f, (sector.floor_current - start) / (speed / DOOM::Doom::Tic));
      else
        return 0.f;
    }

    float   updateCeilingLower(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float elapsed, float target, float speed)
    {
      // Limit target to floor
      target = std::max(target, sector.floor_current);
      sector.ceiling_current = std::max(sector.ceiling_current, target);

      float start = sector.ceiling_current;
      auto  things = doom.level.getThings(sector);

      // Raise target to highest shootable thing obstacle
      for (const auto& thing : things)
        if (thing.get().flags & DOOM::Enum::ThingProperty::ThingProperty_Shootable)
          for (auto thing_index : doom.level.getSectors(thing.get()))
            target = std::max(target, doom.level.sectors[thing_index].floor_current + thing.get().height);

      // Stop if already crushing things
      if (sector.ceiling_current <= target)
        return elapsed;

      // Lower ceiling
      sector.ceiling_current = std::max(target, sector.ceiling_current - elapsed * (speed / DOOM::Doom::Tic));

      // Lower things
      for (const auto& thing : things)
        thing.get().position.z() = std::min(thing.get().position.z(), std::max(thing.get().position.z() + sector.ceiling_current - start, sector.ceiling_current - thing.get().height));

      // Compute remaining time
      if (sector.ceiling_current == target)
        return elapsed - std::max(0.f, (start - sector.ceiling_current) / (speed / DOOM::Doom::Tic));
      else
        return 0.f;
    }

    float   updateCeilingRaise(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float elapsed, float target, float speed)
    {
      // Limit target to floor
      target = std::max(target, sector.floor_current);
      sector.ceiling_current = std::min(sector.ceiling_current, target);

      float start = sector.ceiling_current;

      // Raise ceiling
      sector.ceiling_current = std::min(target, sector.ceiling_current + elapsed * (speed / DOOM::Doom::Tic));

      // Raise things standing in the sector
      for (const auto& thing : doom.level.getThings(sector))
      {
        // Ignore if thing doesn't touch the floor or no gravity
        if (thing.get().position.z() + thing.get().height < start || (thing.get().flags & (DOOM::Enum::ThingProperty::ThingProperty_Float | DOOM::Enum::ThingProperty::ThingProperty_NoGravity)))
          continue;

        float thing_ceiling = sector.ceiling_current;

        // Find lowest ceiling above thing
        for (auto thing_index : doom.level.getSectors(thing.get()))
          thing_ceiling = std::min(thing_ceiling, doom.level.sectors[thing_index].ceiling_current);

        // Raise thing to new lower ceiling
        thing.get().position.z() = std::max(thing.get().position.z(), thing_ceiling - thing.get().height);
      }

      // Compute remaining time
      if (sector.ceiling_current == target)
        return elapsed - std::max(0.f, (sector.ceiling_current - start) / (speed / DOOM::Doom::Tic));
      else
        return 0.f;
    }

    float   updateCeilingCrush(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float elapsed, float target, float speed)
    {
      // Limit target to floor
      target = std::max(target, sector.floor_current);
      sector.ceiling_current = std::max(sector.ceiling_current, target);

      float start = sector.floor_current;
      auto  things = doom.level.getThings(sector);
      float thing_ceiling = sector.floor_current;

      // Get highest shootable thing to crush
      for (const auto& thing : things)
        if (thing.get().flags & DOOM::Enum::ThingProperty::ThingProperty_Shootable)
          for (auto thing_index : doom.level.getSectors(thing.get()))
            thing_ceiling = std::max(thing_ceiling, doom.level.sectors[thing_index].floor_current + thing.get().height);

      // Stop if nothing to crush
      if (sector.ceiling_current > thing_ceiling)
        return elapsed;

      // Lower ceiling
      sector.ceiling_current = std::max(target, sector.ceiling_current - elapsed * (speed / DOOM::Doom::Tic));

      // Lower & crush things
      for (const auto& thing : things) {
        float thing_floor = sector.ceiling_current - thing.get().height;

        // Get thing highest floor
        for (auto thing_index : doom.level.getSectors(thing.get()))
          thing_floor = std::max(thing_floor, doom.level.sectors[thing_index].floor_current);

        // Lower thing
        thing.get().position.z() = std::min(thing.get().position.z(), std::max(thing_floor, thing.get().position.z() + sector.ceiling_current - start));

        // Do not crush not shootable things
        if (!(thing.get().flags & DOOM::Enum::ThingProperty::ThingProperty_Shootable))
          continue;

        float damage = std::min(thing_floor + thing.get().height - sector.ceiling_current, start - sector.ceiling_current) / (speed / DOOM::Doom::Tic) * 87.5f;

        // Deal crush damage (87.5/s)
        if (damage > 0.f)
          thing.get().damage(doom, damage);
      }

      // Compute remaining time
      if (sector.floor_current == target)
        return elapsed - std::max(0.f, (start - sector.ceiling_current) / (speed / DOOM::Doom::Tic));
      else
        return 0.f;
    }

  public:
    AbstractLevelingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, std::int16_t model = -1) :
      std::conditional<Stoppable,
        DOOM::AbstractStoppableAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>,
        DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>
      >::type(doom, sector, model)
    {}

    virtual ~AbstractLevelingAction() = default;
  };
}