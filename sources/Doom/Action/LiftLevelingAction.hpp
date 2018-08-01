#ifndef _LIFT_LEVELING_ACTION_HPP_
#define _LIFT_LEVELING_ACTION_HPP_

#include <list>

#include "Doom/Action/AbstractStoppableAction.hpp"

#include <iostream>

namespace DOOM
{
  template<
    DOOM::EnumAction::Speed Speed,
    DOOM::EnumAction::Repeat Repeat = DOOM::EnumAction::Repeat::RepeatFalse,
    unsigned int Wait = 105
  >
  class LiftLevelingAction : public DOOM::AbstractStoppableAction<DOOM::EnumAction::Type::TypeLeveling>
  {
  private:
    const float			_low, _high;	// Low and high heigh of lift
    DOOM::EnumAction::LiftState	_state;		// Lift current state
    sf::Time			_elapsed;	// Elapsed time since beginning of state

    sf::Time	updateRaise(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed)
    {
      // Raise door
      sector.floor_current += elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds();

      // TODO: cancel if collision
      if (false) {
	_state = DOOM::EnumAction::LiftState::LiftStateLower;
	return elapsed;
      }

      // Compute exceding time
      if (sector.floor_current > _high) {
	sf::Time	exceding = std::min(sf::seconds((sector.floor_current - _high) / Speed * DOOM::Doom::Tic.asSeconds()), elapsed);

	sector.floor_current = _high;
	_state = (Repeat == DOOM::EnumAction::Repeat::RepeatTrue && _stop == false ? DOOM::EnumAction::LiftState::LiftStateWait : DOOM::EnumAction::LiftState::LiftStateStop);
	return exceding;
      }
      else {
	return sf::Time::Zero;
      }
    }

    sf::Time	updateLower(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed)
    {
      // Lower door
      sector.floor_current -= elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds();

      // Compute exceding time
      if (sector.floor_current < _low) {
	sf::Time	exceding = std::min(sf::seconds((_low - sector.floor_current) / Speed * DOOM::Doom::Tic.asSeconds()), elapsed);

	sector.floor_current = _low;
	_state = DOOM::EnumAction::LiftState::LiftStateWait;
	return exceding;
      }
      else {
	return sf::Time::Zero;
      }
    }

    sf::Time	updateWait(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed)
    {
      // Add elapsed time to counter
      _elapsed += elapsed;

      // Compute remaining time if any
      sf::Time	exceding = std::max(_elapsed - (DOOM::Doom::Tic * (float)Wait), sf::Time::Zero);

      // Switch to next state if wait finished
      if (exceding != sf::Time::Zero) {
	_state = (sector.floor_current == _low ?
	  DOOM::EnumAction::LiftState::LiftStateRaise :
	  DOOM::EnumAction::LiftState::LiftStateLower);
      }

      return exceding;
    }

    sf::Time	updateStop(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed)
    {
      // Set new floor base value
      sector.floor_base = sector.floor_current;

      // Remove sector action
      remove(sector);

      return sf::Time::Zero;
    }

  public:
    LiftLevelingAction(DOOM::Doom & doom, float low, float high) :
      DOOM::AbstractStoppableAction<DOOM::EnumAction::Type::TypeLeveling>(doom),
      _low(low),
      _high(high),
      _state(DOOM::EnumAction::LiftState::LiftStateLower),
      _elapsed(sf::Time::Zero)
    {}

    ~LiftLevelingAction() override = default;

    void	update(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed) override	// Update door action
    {
      // Update action states
      while (elapsed != sf::Time::Zero) {
	switch (_state)
	{
	case DOOM::EnumAction::LiftState::LiftStateRaise:
	  elapsed = updateRaise(doom, sector, elapsed);
	  break;
	case DOOM::EnumAction::LiftState::LiftStateLower:
	  elapsed = updateLower(doom, sector, elapsed);
	  break;
	case DOOM::EnumAction::LiftState::LiftStateWait:
	  elapsed = updateWait(doom, sector, elapsed);
	  break;
	case DOOM::EnumAction::LiftState::LiftStateStop:
	  elapsed = updateStop(doom, sector, elapsed);
	  break;

	default:	// Handle error (should not happen)
	  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
	}

	// Reset internal elapsed time between states
	if (elapsed != sf::Time::Zero)
	  _elapsed = sf::Time::Zero;
      }
    }
  };
};

#endif