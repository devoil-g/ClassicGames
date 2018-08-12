#ifndef _RANDOM_LIGHTING_ACTION_HPP_
#define _RANDOM_LIGHTING_ACTION_HPP_

#include <SFML/System/Time.hpp>

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<
    unsigned int CycleDuration = 24,
    unsigned int FlashDuration = 4
  >
  class RandomLightingAction : public DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Lighting>
  {
  private:
    int16_t	_cycle;		// Cycle duration
    int16_t	_flash;		// Flicker duration
    sf::Time	_elapsed;	// Elapsed time

  public:
    RandomLightingAction(DOOM::Doom & doom) :
      DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Lighting>(doom),
      _cycle(0),
      _flash(0),
      _elapsed(sf::Time::Zero)
    {
      static_assert(CycleDuration != 0 && FlashDuration != 0 && CycleDuration >= FlashDuration, "Invalid RandomLightingAction parameters.");
    }

    ~RandomLightingAction() override = default;

    virtual void	update(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed) override	// Update light sector
    {
      // Update elapsed time
      _elapsed += elapsed;

      // Regenerate random parameter
      while (_elapsed > DOOM::Doom::Tic * (float)_cycle) {
	_elapsed -= DOOM::Doom::Tic * (float)_cycle;
	_cycle = (int16_t)(Math::Random() * CycleDuration) + (FlashDuration * 4) + 1;
	_flash = (int16_t)(Math::Random() * FlashDuration) + 1;
      }

      // Compute light value from elapsed time
      if (_elapsed < DOOM::Doom::Tic * (float)_flash)
	sector.light_current = sector.getNeighborLowestLight(doom);
      else
	sector.light_current = sector.light_base;
    }
  };
};

#endif