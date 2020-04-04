#pragma once

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumAction::Direction Direction,
    DOOM::EnumAction::Speed Speed,
    bool Crush = false
  >
    class CeilingLevelingAction : public DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling>
  {
  private:
    const float _target;  // Ceiling target height

  public:
    CeilingLevelingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float target) :
      DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Leveling>(doom, sector),
      _target(target)
    {}

    ~CeilingLevelingAction() override = default;

    void  update(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed) override  // Update door action
    {
      // Update ceiling height
      sector.ceiling_current = (Direction == DOOM::EnumAction::Direction::DirectionUp) ?
        std::min(sector.ceiling_current + Speed / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds(), _target) :
        std::max(sector.ceiling_current - Speed / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds(), _target);

      // TODO: apply crushing

      // Detect end of action
      if (sector.ceiling_current == _target) {
        sector.ceiling_base = sector.ceiling_current;
        remove(doom, sector);
      }
    }
  };
}