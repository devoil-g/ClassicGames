#ifndef _DOOR_LEVELING_ACTION_HPP_
#define _DOOR_LEVELING_ACTION_HPP_

#include <list>

#include <SFML/System/Time.hpp>

#include "Doom/Doom.hpp"

namespace DOOM
{
  template<DOOM::EnumAction::Speed Speed, unsigned int Wait = 0, unsigned int ForceWait = 140>
  class DoorLevelingAction : public DOOM::Doom::Level::Sector::AbstractAction
  {
  private:
    std::list<DOOM::EnumAction::State>	_states;	// Door states to perform
    sf::Time				_elapsed;	// Time elapsed since begining of state

    sf::Time	updateOpen(DOOM::Doom::Level::Sector & sector, sf::Time elapsed)	// Update an openning door. Return exceding time.
    {
      const float	top = sector.getNeighborLowestCeiling() - 4.f;

      // Instant open if above top
      if (sector.ceiling() > top) {
	sector.ceiling() = top;
	return elapsed;
      }

      // Raise door
      sector.ceiling() += elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds();

      // Compute exceding time
      if (sector.ceiling() > top) {
	sf::Time	exceding = sf::seconds((sector.ceiling() - top) / Speed * DOOM::Doom::Tic.asSeconds());

	sector.ceiling() = top;
	return exceding;
      }
      else {
	return sf::Time::Zero;
      }
    }

    sf::Time	updateClose(DOOM::Doom::Level::Sector & sector, sf::Time elapsed)	// Update a closing door. Return exceding time.
    {
      // Instant close if under floor
      if (sector.ceiling() < sector.floor()) {
	sector.ceiling() = sector.floor();
	return elapsed;
      }

      // TODO: add check for things in sector
      if (false) {
	_states.push_front(DOOM::EnumAction::State::StateForceWait);
	_states.push_front(DOOM::EnumAction::State::StateOpen);
	return elapsed;
      }

      // Lower door
      sector.ceiling() -= elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds();

      // Compute exceding time
      if (sector.ceiling() < sector.floor()) {
	sf::Time	exceding = sf::seconds((sector.floor() - sector.ceiling()) / Speed * DOOM::Doom::Tic.asSeconds());

	sector.ceiling() = sector.floor();
	return exceding;
      }
      else {
	return sf::Time::Zero;
      }
    }

    sf::Time	updateForceClose(DOOM::Doom::Level::Sector & sector, sf::Time elapsed)	// Update a closing door. Don't bounce on player. head Return exceding time.
    {
      // Instant close if under floor
      if (sector.ceiling() < sector.floor()) {
	sector.ceiling() = sector.floor();
	return elapsed;
      }

      // TODO: add check for things in sector
      if (false) {
	return elapsed;
      }

      // Lower door
      sector.ceiling() -= elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds();

      // Compute exceding time
      if (sector.ceiling() < sector.floor()) {
	sf::Time	exceding = sf::seconds((sector.floor() - sector.ceiling()) / Speed * DOOM::Doom::Tic.asSeconds());

	sector.ceiling() = sector.floor();
	return exceding;
      }
      else {
	return sf::Time::Zero;
      }
    }

    sf::Time	updateWait(DOOM::Doom::Level::Sector & sector, sf::Time elapsed)	// Update a waiting door. Return exceding time.
    {
      _elapsed += elapsed;

      // Return remaining time if any
      return std::max(_elapsed - (DOOM::Doom::Tic * (float)Wait), sf::Time::Zero);
    }

    sf::Time	updateForceWait(DOOM::Doom::Level::Sector & sector, sf::Time elapsed)	// Update a waiting door. Wait for a fixed time. Return exceding time.
    {
      _elapsed += elapsed;

      // Return remaining time if any
      return std::max(_elapsed - (DOOM::Doom::Tic * (float)ForceWait), sf::Time::Zero);
    }

  public:
    DoorLevelingAction(std::list<DOOM::EnumAction::State> && states) :
      DOOM::Doom::Level::Sector::AbstractAction(),
      _states(std::move(states)),
      _elapsed(sf::Time::Zero)
    {}

    ~DoorLevelingAction() override
    {}

    void	update(DOOM::Doom::Level::Sector & sector, sf::Time elapsed) override	// Update door action
    {
      // Update action states
      while (_states.empty() == false) {
	switch (_states.front())
	{
	case DOOM::EnumAction::State::StateOpen:
	  elapsed = updateOpen(sector, elapsed);
	  break;
	case DOOM::EnumAction::State::StateClose:
	  elapsed = updateClose(sector, elapsed);
	  break;
	case DOOM::EnumAction::State::StateForceClose:
	  elapsed = updateForceClose(sector, elapsed);
	  break;
	case DOOM::EnumAction::State::StateWait:
	  elapsed = updateWait(sector, elapsed);
	  break;
	case DOOM::EnumAction::State::StateForceWait:
	  elapsed = updateForceWait(sector, elapsed);
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
	sector.baseCeiling() = sector.ceiling();
	remove(sector);
      }
    }
  };
};

#endif