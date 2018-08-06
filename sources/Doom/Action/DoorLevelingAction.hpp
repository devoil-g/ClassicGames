#ifndef _DOOR_LEVELING_ACTION_HPP_
#define _DOOR_LEVELING_ACTION_HPP_

#include <list>

#include <SFML/System/Time.hpp>

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumAction::Speed Speed,
    unsigned int Wait = 0,
    unsigned int ForceWait = 140
  >
  class DoorLevelingAction : public DOOM::AbstractTypeAction<DOOM::EnumAction::Type::TypeLeveling>
  {
  private:
    std::list<DOOM::EnumAction::DoorState>	_states;	// Door states to perform
    sf::Time					_elapsed;	// Time elapsed since begining of state

    sf::Time	updateOpen(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed)	// Update an openning door. Return exceding time.
    {
      const float	top = sector.getNeighborLowestCeiling(doom).second - 4.f;

      // Raise door
      sector.ceiling_current += elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds();

      // Compute exceding time
      if (sector.ceiling_current > top) {
	sf::Time	exceding = std::min(sf::seconds((sector.ceiling_current - top) / Speed * DOOM::Doom::Tic.asSeconds()), elapsed);

	sector.ceiling_current = top;
	return exceding;
      }
      else {
	return sf::Time::Zero;
      }
    }

    sf::Time	updateClose(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed)	// Update a closing door. Return exceding time.
    {
      // TODO: add check for things in sector
      if (false) {
	_states.push_front(DOOM::EnumAction::DoorState::DoorStateForceWait);
	_states.push_front(DOOM::EnumAction::DoorState::DoorStateOpen);
	return elapsed;
      }

      // Lower door
      sector.ceiling_current -= elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds();

      // Compute exceding time
      if (sector.ceiling_current < sector.floor_base) {
	sf::Time	exceding = std::min(sf::seconds((sector.floor_base - sector.ceiling_current) / Speed * DOOM::Doom::Tic.asSeconds()), elapsed);

	sector.ceiling_current = sector.floor_base;
	return exceding;
      }
      else {
	return sf::Time::Zero;
      }
    }

    sf::Time	updateForceClose(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed)	// Update a closing door. Don't bounce on player. head Return exceding time.
    {
      // TODO: add check for things in sector
      if (false) {
	return elapsed;
      }

      // Lower door
      sector.ceiling_current -= elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds();

      // Compute exceding time
      if (sector.ceiling_current < sector.floor_base) {
	sf::Time	exceding = std::min(sf::seconds((sector.floor_base - sector.ceiling_current) / Speed * DOOM::Doom::Tic.asSeconds()), elapsed);

	sector.ceiling_current = sector.floor_base;
	return exceding;
      }
      else {
	return sf::Time::Zero;
      }
    }

    sf::Time	updateWait(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed)	// Update a waiting door. Return exceding time.
    {
      _elapsed += elapsed;

      // Return remaining time if any
      return std::max(_elapsed - (DOOM::Doom::Tic * (float)Wait), sf::Time::Zero);
    }

    sf::Time	updateForceWait(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed)	// Update a waiting door. Wait for a fixed time. Return exceding time.
    {
      _elapsed += elapsed;

      // Return remaining time if any
      return std::max(_elapsed - (DOOM::Doom::Tic * (float)ForceWait), sf::Time::Zero);
    }

  public:
    DoorLevelingAction(DOOM::Doom & doom, std::list<DOOM::EnumAction::DoorState> && states) :
      DOOM::AbstractTypeAction<DOOM::EnumAction::Type::TypeLeveling>(doom),
      _states(std::move(states)),
      _elapsed(sf::Time::Zero)
    {}

    ~DoorLevelingAction() override = default;

    void	update(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed) override	// Update door action
    {
      // Update action states
      while (_states.empty() == false) {
	switch (_states.front())
	{
	case DOOM::EnumAction::DoorState::DoorStateOpen:
	  elapsed = updateOpen(doom, sector, elapsed);
	  break;
	case DOOM::EnumAction::DoorState::DoorStateClose:
	  elapsed = updateClose(doom, sector, elapsed);
	  break;
	case DOOM::EnumAction::DoorState::DoorStateForceClose:
	  elapsed = updateForceClose(doom, sector, elapsed);
	  break;
	case DOOM::EnumAction::DoorState::DoorStateWait:
	  elapsed = updateWait(doom, sector, elapsed);
	  break;
	case DOOM::EnumAction::DoorState::DoorStateForceWait:
	  elapsed = updateForceWait(doom, sector, elapsed);
	  break;
	default:	// Handle error (should not happen)
	  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
	}

	// Stop if no more elapsed time
	if (elapsed == sf::Time::Zero)
	  break;

	// Pop ended state
	_states.pop_front();
	_elapsed = sf::Time::Zero;
      }

      // Update ceiling base value when animation ended and remove action from sector
      if (_states.empty() == true) {
	sector.ceiling_base = sector.ceiling_current;
	remove(doom, sector);
      }
    }
  };
};

#endif