#ifndef _BLINK_LIGHTING_ACTION_HPP_
#define _BLINK_LIGHTING_ACTION_HPP_

#include "Doom/Doom.hpp"

namespace DOOM
{
  template<unsigned int CycleDuration = 35, unsigned int FlashDuration = 5, bool Sync = false>
  class BlinkLightingAction : public DOOM::Doom::Level::Sector::AbstractAction
  {
  private:
    sf::Time	_elapsed;	// Elapsed time

  public:
    BlinkLightingAction() :
      DOOM::Doom::Level::Sector::AbstractAction(),
      _elapsed(Sync == true ? sf::Time::Zero : DOOM::Doom::Tic * (9.f * Math::Random()))
    {}

    ~BlinkLightingAction() override
    {}

    virtual void	update(DOOM::Doom::Level::Sector & sector, sf::Time elapsed) override	// Update sector
    {
      // Update time
      _elapsed += elapsed;

      if (_elapsed.asMicroseconds() / DOOM::Doom::Tic.asMicroseconds() % CycleDuration < FlashDuration)
	sector.light() = sector.baseLight();
      else
	sector.light() = sector.getNeighborLowestLight();
    }
  };
};

#endif