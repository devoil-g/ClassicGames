#ifndef _FLOOR_LEVELING_ACTION_HPP_
#define _FLOOR_LEVELING_ACTION_HPP_

#include <list>

#include <SFML/System/Time.hpp>

#include "Doom/Doom.hpp"
#include "Doom/Sector/DoorLevelingAction.hpp"

namespace DOOM
{
  template<DOOM::EnumAction::Direction Direction, DOOM::EnumAction::Speed Speed, DOOM::EnumAction::Crush>
  class FloorLevelingAction : public DOOM::Doom::Level::Sector::AbstractAction
  {
  private:
    const float	_target;	// Floor target height

  public:
    FloorLevelingAction(float target) :
      DOOM::Doom::Level::Sector::AbstractAction(),
      _target(target)
    {}

    ~FloorLevelingAction() override
    {}

    void	update(DOOM::Doom::Level::Sector & sector, sf::Time elapsed) override	// Update door action
    {
      // Update floor height
      sector.floor() = (Direction == DOOM::EnumAction::Direction::DirectionUp) ?
	std::min(sector.floor() + Speed / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds(), _target) :
	std::max(sector.floor() - Speed / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds(), _target);

      // TODO: apply crushing

      // Detect end of action
      if (sector.floor() == _target) {
	sector.baseFloor() = sector.floor();
	remove(sector);
      }
    }
  };
};

#endif