#ifndef _DOOR_LEVELING_ACTION_HPP_
#define _DOOR_LEVELING_ACTION_HPP_

#include <list>

#include <SFML/System/Time.hpp>

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  namespace EnumAction
  {
    enum Door
    {
      DoorWaitOpen,		// Open the door
      DoorWaitClose,		// Close the door
      DoorOpenWaitClose,	// Open, wait then close the door
      DoorCloseWaitOpen		// Close, wait then re-open to door
    };
  };

  template<
    DOOM::EnumAction::Door Door,
    DOOM::EnumAction::Speed Speed = DOOM::EnumAction::Speed::SpeedSlow,
    unsigned int TickWait = 140,
    unsigned int TickForceWait = 140
  >
  class DoorLevelingAction : public DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling>
  {
  private:
    enum State
    {
      Noop,		// Does nothing (dummy state)
      Open,		// Open the door
      Close,		// Close the door
      ForceClose,	// Close the door without bouncing on thing
      Wait,		// Wait for the specified time
      ForceWait		// Wait for a fixed time
    };

    std::list<State>	_states;	// Door states to perform
    sf::Time		_elapsed;	// Time elapsed since begining of state

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
	switch (Door) {
	case DOOM::EnumAction::Door::DoorOpenWaitClose:
	  _states = { State::Noop, State::Open, State::ForceWait, State::Close };
	  break;
	case DOOM::EnumAction::Door::DoorCloseWaitOpen:
	  _states = { State::Noop, State::Open, State::ForceWait, State::Close, State::Wait, State::Open };
	  break;
	default:
	  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
	}
	
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
      return std::max(_elapsed - (DOOM::Doom::Tic * (float)TickWait), sf::Time::Zero);
    }

    sf::Time	updateForceWait(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed)	// Update a waiting door. Wait for a fixed time. Return exceding time.
    {
      _elapsed += elapsed;

      // Return remaining time if any
      return std::max(_elapsed - (DOOM::Doom::Tic * (float)TickForceWait), sf::Time::Zero);
    }

  public:
    DoorLevelingAction(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector) :
      DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling>(doom, sector),
      _states(),
      _elapsed(sf::Time::Zero)
    {
      // Map of states according to door type
      static const std::unordered_map<DOOM::EnumAction::Door, std::list<State>>	states = {
	{ DOOM::EnumAction::Door::DoorOpenWaitClose, { State::Open, State::Wait, State::Close } },
	{ DOOM::EnumAction::Door::DoorCloseWaitOpen, { State::Close, State::Wait, State::Open } },
	{ DOOM::EnumAction::Door::DoorWaitOpen, { State::Wait, State::Open } },
	{ DOOM::EnumAction::Door::DoorWaitClose, { State::Wait, State::ForceClose } }
      };

      std::unordered_map<DOOM::EnumAction::Door, std::list<State>>::const_iterator	iterator = states.find(Door);

      // Check for errors
      if (iterator != states.end())
	_states = iterator->second;
      else
	throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }

    ~DoorLevelingAction() override = default;

    void	update(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed) override	// Update door action
    {
      // Update action states
      while (_states.empty() == false) {
	switch (_states.front())
	{
	case State::Open:
	  elapsed = updateOpen(doom, sector, elapsed);
	  break;
	case State::Close:
	  elapsed = updateClose(doom, sector, elapsed);
	  break;
	case State::ForceClose:
	  elapsed = updateForceClose(doom, sector, elapsed);
	  break;
	case State::Wait:
	  elapsed = updateWait(doom, sector, elapsed);
	  break;
	case State::ForceWait:
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

    void	start(DOOM::Doom & doom, DOOM::AbstractThing & thing) override	// Request door to re-trigger
    {
      switch (Door) {
      case DOOM::EnumAction::Door::DoorOpenWaitClose:
	switch (_states.front()) {
	case State::Open:
	  _states = { State::Close };
	  break;
	case State::Close: case State::ForceClose:
	  _states = { State::Open, State::Wait, State::Close };
	  break;
	case State::Wait: case State::ForceWait:
	  _states.pop_front();
	  break;
	}
	break;
      case DOOM::EnumAction::Door::DoorCloseWaitOpen:
	switch (_states.front()) {
	case State::Open:
	  _states = { State::Close, State::Wait, State::Open };
	  break;
	case State::Close: case State::ForceClose:
	  _states = { State::Open };
	  break;
	case State::Wait: case State::ForceWait:
	  _states.pop_front();
	  break;
	}
	break;
      case DOOM::EnumAction::Door::DoorWaitOpen:
	break;
      case DOOM::EnumAction::Door::DoorWaitClose:
	break;
      default:
	throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      }
    }
  };
};

#endif