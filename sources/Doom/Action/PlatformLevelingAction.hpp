#ifndef _PLATFORM_LEVELING_ACTION_HPP_
#define _PLATFORM_LEVELING_ACTION_HPP_

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumAction::Speed Speed,
    DOOM::EnumAction::Change::Type ChangeType = DOOM::EnumAction::Change::Type::ChangeTypeNone
  >
  class PlatformLevelingAction : public DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType>
  {
  private:
    enum State
    {
      Raise,	// Raise the platform
      Lower,	// Lower the platform
      Stop	// Stop platform
    };

    float	_target;	// Floor target height
    State	_state;		// Platform current state

    sf::Time	updateRaise(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed)
    {
      // Raise door
      sector.floor_current += elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds();

      // TODO: cancel if collision
      if (false) {
	// Change target height to original
	_target = sector.floor_base;
	_state = State::Lower;
	return elapsed;
      }

      // Compute exceding time
      if (sector.floor_current > _target) {
	sf::Time	exceding = std::min(sf::seconds((sector.floor_current - _target) / Speed * DOOM::Doom::Tic.asSeconds()), elapsed);

	sector.floor_current = _target;
	_state = State::Stop;
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
      if (sector.floor_current < _target) {
	sf::Time	exceding = std::min(sf::seconds((_target - sector.floor_current) / Speed * DOOM::Doom::Tic.asSeconds()), elapsed);

	sector.floor_current = _target;
	_state = State::Stop;
	return exceding;
      }
      else {
	return sf::Time::Zero;
      }
    }

    sf::Time	updateStop(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed)
    {
      // Set new floor base value
      sector.floor_base = sector.floor_current;

      // Remove sector action
      remove(doom, sector);

      return sf::Time::Zero;
    }

  public:
    PlatformLevelingAction(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, float target, int16_t model = -1) :
      DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType>(doom, sector, model),
      _target(target),
      _state(State::Raise)
    {}

    ~PlatformLevelingAction() override = default;

    void	update(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed) override	// Update door action
    {
      // Update action states
      while (elapsed != sf::Time::Zero) {
	switch (_state)
	{
	case State::Raise:
	  elapsed = updateRaise(doom, sector, elapsed);
	  break;
	case State::Lower:
	  elapsed = updateLower(doom, sector, elapsed);
	  break;
	case State::Stop:
	  elapsed = updateStop(doom, sector, elapsed);
	  break;

	default:	// Handle error (should not happen)
	  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
	}
      }
    }
  };
};

#endif