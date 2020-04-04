#pragma once

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<
    unsigned int CycleDuration = 35,
    unsigned int FlashDuration = 5,
    bool Sync = false
  >
    class BlinkLightingAction : public DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Lighting>
  {
  private:
    sf::Time  _elapsed; // Elapsed time

  public:
    BlinkLightingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector) :
      DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Lighting>(doom, sector),
      _elapsed(Sync == true ? sf::Time::Zero : DOOM::Doom::Tic * (9.f * Math::Random()))
    {}

    ~BlinkLightingAction() override = default;

    virtual void  update(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, sf::Time elapsed) override  // Update sector
    {
      // Update time
      _elapsed += elapsed;

      if (_elapsed.asMicroseconds() / DOOM::Doom::Tic.asMicroseconds() % CycleDuration < FlashDuration)
        sector.light_current = sector.light_base;
      else
        sector.light_current = sector.getNeighborLowestLight(doom);
    }
  };
}