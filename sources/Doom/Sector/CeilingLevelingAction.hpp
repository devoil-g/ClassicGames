#ifndef _CEILING_LEVELING_ACTION_HPP_
#define _CEILING_LEVELING_ACTION_HPP_

#include <list>

#include <SFML/System/Time.hpp>

#include "Doom/Doom.hpp"

namespace DOOM
{
  template<DOOM::EnumAction::Direction Direction, DOOM::EnumAction::Speed Speed, DOOM::EnumAction::Crush>
  class CeilingLevelingAction : public DOOM::Doom::Level::Sector::AbstractAction
  {
  private:
    const float	_target;	// Ceiling target height

  public:
    CeilingLevelingAction(float target) :
      DOOM::Doom::Level::Sector::AbstractAction(),
      _target(target)
    {}

    ~CeilingLevelingAction() override
    {}

    void	update(DOOM::Doom::Level::Sector & sector, sf::Time elapsed) override	// Update door action
    {
      // Update ceiling height
      sector.ceiling() = (Direction == DOOM::EnumAction::Direction::DirectionUp) ?
	std::min(sector.ceiling() + Speed / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds(), _target) :
	std::max(sector.ceiling() - Speed / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds(), _target);

      // TODO: apply crushing

      // Detect end of action
      if (sector.ceiling() == _target) {
	sector.baseCeiling() = sector.ceiling();
	remove(sector);
      }
    }
  };
};

#endif