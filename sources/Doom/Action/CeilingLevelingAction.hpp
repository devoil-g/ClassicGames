#ifndef _CEILING_LEVELING_ACTION_HPP_
#define _CEILING_LEVELING_ACTION_HPP_

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumAction::Direction Direction,
    DOOM::EnumAction::Speed Speed,
    DOOM::EnumAction::Crush = DOOM::EnumAction::Crush::CrushFalse,
    DOOM::EnumAction::Change Change = DOOM::EnumAction::Change::ChangeNone
  >
  class CeilingLevelingAction : public DOOM::AbstractTypeAction<DOOM::EnumAction::Type::TypeLeveling>
  {
  private:
    const float	_target;	// Ceiling target height

  public:
    CeilingLevelingAction(DOOM::Doom & doom, float target) :
      DOOM::AbstractTypeAction<DOOM::EnumAction::Type::TypeLeveling>(doom),
      _target(target)
    {}

    ~CeilingLevelingAction() override = default;

    void	update(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed) override	// Update door action
    {
      // Update ceiling height
      sector.ceiling_current = (Direction == DOOM::EnumAction::Direction::DirectionUp) ?
	std::min(sector.ceiling_current + Speed / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds(), _target) :
	std::max(sector.ceiling_current - Speed / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds(), _target);

      // TODO: apply crushing

      // Detect end of action
      if (sector.ceiling_current == _target) {
	sector.ceiling_base = sector.ceiling_current;
	sector.action<DOOM::EnumAction::Type::TypeLeveling>().reset();
      }
    }
  };
};

#endif