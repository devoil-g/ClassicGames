#ifndef _PLATFORM_LEVELING_ACTION_HPP_
#define _PLATFORM_LEVELING_ACTION_HPP_

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumAction::Speed Speed,
    DOOM::EnumAction::Change::Type ChangeType = DOOM::EnumAction::Change::Type::None
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
      float	obstacle = std::numeric_limits<float>::max();

      // Get lowest obstacle
      for (const std::reference_wrapper<DOOM::AbstractThing> & thing : doom.level.getThings(sector, DOOM::AbstractThing::Monster))
	for (int16_t sector_index : doom.level.getSector(thing.get()))
	  obstacle = std::min(obstacle, doom.level.sectors[sector_index].ceiling_current - thing.get().height);

      // Handle collision with obstacle, changing target height to original
      if (sector.floor_current + elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds() > obstacle) {
	if (sector.floor_current > obstacle) {
	  _target = sector.floor_base;
	  _state = State::Lower;
	  return elapsed;
	}
	else {
	  sf::Time	exceding = std::min(sf::seconds((obstacle - sector.floor_current) / Speed * DOOM::Doom::Tic.asSeconds()), elapsed);

	  sector.floor_current = obstacle;
	  _target = sector.floor_base;
	  _state = State::Lower;
	  return exceding;
	}
	
      }

      // Raise door
      sector.floor_current += elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds();

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
      // TODO: lower things that stand on the ground of the sector

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