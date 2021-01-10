#pragma once

#include <type_traits>
#include <iostream>

#include "Doom/Action/AbstractStoppableAction.hpp"

namespace DOOM
{
  template<
    bool Stoppable = false,
    DOOM::EnumAction::Change::Type ChangeType = DOOM::EnumAction::Change::Type::None,
    DOOM::EnumAction::Change::Time ChangeTime = DOOM::EnumAction::Change::Time::Before
  >
  class AbstractLevelingAction : public DOOM::AbstractStoppableAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>
  {
  protected:
    sf::Time  updateFloorLower(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed, float target, float speed)
    {
      // Limit target to ceiling
      target = std::min(target, sector.ceiling_current);
      sector.floor_current = std::max(sector.floor_current, target);

      float start = sector.floor_current;

      // Lower floor
      sector.floor_current = std::max(target, sector.floor_current - elapsed.asSeconds() * (speed / DOOM::Doom::Tic.asSeconds()));

      // Lower things standing in the sector
      for (const auto& thing : doom.level.getThings(sector, DOOM::Enum::ThingProperty::ThingProperty_Shootable))
      {
        // Ignore if thing doesn't touch the floor or no gravity
        if (thing.get().position.z() > start || thing.get().flags & DOOM::Enum::ThingProperty::ThingProperty_Float)
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
        return sf::seconds(elapsed.asSeconds() - std::max(0.f, (start - sector.floor_current) / (speed / DOOM::Doom::Tic.asSeconds())));
      else
        return sf::Time::Zero;
    }

    sf::Time  updateFloorRaise(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed, float target, float speed)
    {
      // Limit target to ceiling
      target = std::min(target, sector.ceiling_current);
      sector.floor_current = std::min(sector.floor_current, target);

      float start = sector.floor_current;
      auto  things = doom.level.getThings(sector, DOOM::Enum::ThingProperty::ThingProperty_Shootable);
      
      // Lower target to lowest thing obstacle
      for (const auto& thing : things)
        for (auto thing_index : doom.level.getSectors(thing.get()))
          target = std::min(target, doom.level.sectors[thing_index].ceiling_current - thing.get().height);

      // Stop if already crushing things
      if (sector.floor_current >= target)
        return elapsed;

      // Raise floor
      sector.floor_current = std::min(target, sector.floor_current + elapsed.asSeconds() * (speed / DOOM::Doom::Tic.asSeconds()));

      // Raise things
      for (const auto& thing : things)
        thing.get().position.z() = std::max(thing.get().position.z(), std::min(thing.get().position.z() + sector.floor_current - start, sector.floor_current));

      // Compute remaining time
      if (sector.floor_current == target)
        return sf::seconds(elapsed.asSeconds() - std::max(0.f, (sector.floor_current - start) / (speed / DOOM::Doom::Tic.asSeconds())));
      else
        return sf::Time::Zero;
    }

    sf::Time  updateFloorCrush(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed, float target, float speed)
    {
      // Limit target to ceiling
      target = std::min(target, sector.ceiling_current);
      sector.floor_current = std::min(sector.floor_current, target);

      float start = sector.floor_current;
      auto  things = doom.level.getThings(sector, DOOM::Enum::ThingProperty::ThingProperty_Shootable);

      // Check that something is getting crushed
      {
        float thing_floor = sector.ceiling_current;

        // Get lowest thing to crush
        for (const auto& thing : things)
          for (auto thing_index : doom.level.getSectors(thing.get()))
            thing_floor = std::min(thing_floor, doom.level.sectors[thing_index].ceiling_current - thing.get().height);

        // Stop if nothing to crush
        if (sector.floor_current < thing_floor)
          return elapsed;
      }

      // Raise floor
      sector.floor_current = std::min(target, sector.floor_current + elapsed.asSeconds() * (speed / DOOM::Doom::Tic.asSeconds()));

      // Raise & crush things
      for (const auto& thing : things)
      {
        // Raise thing
        thing.get().position.z() = std::max(thing.get().position.z(), std::min(sector.floor_current, thing.get().position.z() + sector.floor_current - start));

        float thing_ceiling = sector.ceiling_current;

        // Get thing lowest ceiling
        for (auto thing_index : doom.level.getSectors(thing.get()))
          thing_ceiling = std::min(thing_ceiling, doom.level.sectors[thing_index].ceiling_current);

        float damage = std::min(sector.floor_current + thing.get().height - thing_ceiling, sector.floor_current - start) / (speed / DOOM::Doom::Tic.asSeconds()) * 87.5f;

        // Deal crush damage (87.5/s)
        if (damage > 0.f)
          thing.get().damage(doom, damage);
      }

      // Compute remaining time
      if (sector.floor_current == target)
        return sf::seconds(elapsed.asSeconds() - std::max(0.f, (sector.floor_current - start) / (speed / DOOM::Doom::Tic.asSeconds())));
      else
        return sf::Time::Zero;
    }

    sf::Time  updateCeilingLower(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed, float target, float speed)
    {
      // Limit target to floor
      target = std::max(target, sector.floor_current);
      sector.ceiling_current = std::max(sector.ceiling_current, target);

      float start = sector.ceiling_current;
      auto  things = doom.level.getThings(sector, DOOM::Enum::ThingProperty::ThingProperty_Shootable);

      // Raise target to highest thing obstacle
      for (const auto& thing : things)
        for (auto thing_index : doom.level.getSectors(thing.get()))
          target = std::max(target, doom.level.sectors[thing_index].floor_current + thing.get().height);

      // Stop if already crushing things
      if (sector.ceiling_current <= target)
        return elapsed;

      // Lower ceiling
      sector.ceiling_current = std::max(target, sector.ceiling_current - elapsed.asSeconds() * (speed / DOOM::Doom::Tic.asSeconds()));

      // Lower things
      for (const auto& thing : things)
        thing.get().position.z() = std::min(thing.get().position.z(), std::max(thing.get().position.z() + sector.ceiling_current - start, sector.ceiling_current - thing.get().height));

      // Compute remaining time
      if (sector.ceiling_current == target)
        return sf::seconds(elapsed.asSeconds() - std::max(0.f, (start - sector.ceiling_current) / (speed / DOOM::Doom::Tic.asSeconds())));
      else
        return sf::Time::Zero;
    }

    sf::Time  updateCeilingRaise(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed, float target, float speed)
    {
      // Limit target to floor
      target = std::max(target, sector.floor_current);
      sector.ceiling_current = std::min(sector.ceiling_current, target);

      float start = sector.ceiling_current;

      // Raise ceiling
      sector.ceiling_current = std::min(target, sector.ceiling_current + elapsed.asSeconds() * (speed / DOOM::Doom::Tic.asSeconds()));

      // Raise things standing in the sector
      for (const auto& thing : doom.level.getThings(sector, DOOM::Enum::ThingProperty::ThingProperty_Shootable))
      {
        // Ignore if thing doesn't touch the floor or no gravity
        if (thing.get().position.z() + thing.get().height < start || thing.get().flags & DOOM::Enum::ThingProperty::ThingProperty_Float)
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
        return sf::seconds(elapsed.asSeconds() - std::max(0.f, (sector.ceiling_current - start) / (speed / DOOM::Doom::Tic.asSeconds())));
      else
        return sf::Time::Zero;
    }

    sf::Time  updateCeilingCrush(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed, float target, float speed)
    {
      // Limit target to floor
      target = std::max(target, sector.floor_current);
      sector.ceiling_current = std::max(sector.ceiling_current, target);

      float start = sector.floor_current;
      auto  things = doom.level.getThings(sector, DOOM::Enum::ThingProperty::ThingProperty_Shootable);
      float thing_ceiling = sector.floor_current;

      // Get highest thing to crush
      for (const auto& thing : things)
        for (auto thing_index : doom.level.getSectors(thing.get()))
          thing_ceiling = std::max(thing_ceiling, doom.level.sectors[thing_index].floor_current + thing.get().height);

      // Stop if nothing to crush
      if (sector.ceiling_current > thing_ceiling)
        return elapsed;

      // Lower ceiling
      sector.ceiling_current = std::max(target, sector.ceiling_current - elapsed.asSeconds() * (speed / DOOM::Doom::Tic.asSeconds()));

      // Lower & crush things
      for (const auto& thing : things) {
        float thing_floor = sector.ceiling_current - thing.get().height;

        // Get thing highest floor
        for (auto thing_index : doom.level.getSectors(thing.get()))
          thing_floor = std::max(thing_floor, doom.level.sectors[thing_index].floor_current);

        // Lower thing
        thing.get().position.z() = std::min(thing.get().position.z(), std::max(thing_floor, thing.get().position.z() + sector.ceiling_current - start));

        float damage = std::min(thing_floor + thing.get().height - sector.ceiling_current, start - sector.ceiling_current) / (speed / DOOM::Doom::Tic.asSeconds()) * 87.5f;

        // Deal crush damage (87.5/s)
        if (damage > 0.f)
          thing.get().damage(doom, damage);
      }

      // Compute remaining time
      if (sector.floor_current == target)
        return sf::seconds(elapsed.asSeconds() - std::max(0.f, (start - sector.ceiling_current) / (speed / DOOM::Doom::Tic.asSeconds())));
      else
        return sf::Time::Zero;

      // Not implemented
      return elapsed;
    }

  public:
    AbstractLevelingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, int16_t model = -1) :
      DOOM::AbstractStoppableAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>(doom, sector, model)
    {}

    virtual ~AbstractLevelingAction() = default;

    virtual bool  stop(DOOM::Doom& doom, DOOM::AbstractThing& thing)
    {
      if (Stoppable == true)
        return DOOM::AbstractStoppableAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>::stop(doom, thing);
      else
        return false;
    }

    virtual bool  start(DOOM::Doom& doom, DOOM::AbstractThing& thing)
    {
      if (Stoppable == true)
        return DOOM::AbstractStoppableAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>::start(doom, thing);
      else
        return false;
    }
  };
}