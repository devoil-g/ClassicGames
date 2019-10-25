#ifndef _FLOOR_LEVELING_ACTION_HPP_
#define _FLOOR_LEVELING_ACTION_HPP_

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumAction::Direction Direction,
    DOOM::EnumAction::Speed Speed,
    DOOM::EnumAction::Change::Type ChangeType = DOOM::EnumAction::Change::Type::None,
    DOOM::EnumAction::Change::Time ChangeTime = DOOM::EnumAction::Change::Time::Before,
    bool Crush = false
  >
  class FloorLevelingAction : public DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>
  {
  private:
    const float	_target;	// Floor target height

  public:
    FloorLevelingAction(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, float target, int16_t model = -1) :
      DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling, ChangeType, ChangeTime>(doom, sector, model),
      _target(target)
    {}

    ~FloorLevelingAction() override = default;

    void	update(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed) override	// Update door action
    {
      // TODO: handle monster on sector when lowering floor height (gravity wiggle)
      // Reduce speed by 2 if an obtacle is encountered

      // Update floor height
      sector.floor_current = (Direction == DOOM::EnumAction::Direction::DirectionUp) ?
	std::min(sector.floor_current + Speed / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds(), _target) :
	std::max(sector.floor_current - Speed / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds(), _target);

      if (Crush == true) {
	// TODO: apply crushing
      }

      // Detect end of action
      if (sector.floor_current == _target) {
	sector.floor_base = sector.floor_current;
	remove(doom, sector);
      }
    }
  };
};

#endif