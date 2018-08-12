#ifndef _FLICKER_LIGHTING_ACTION_HPP_
#define _FLICKER_LIGHTING_ACTION_HPP_

#include <SFML/System/Time.hpp>

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<
    unsigned int CycleDuration = 32,
    unsigned int FlickerDuration = 4
  >
  class FlickerLightingAction : public DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Lighting>
  {
  private:
    int16_t	_cycle;		// Cycle duration
    int16_t	_flicker;	// Flicker duration
    sf::Time	_elapsed;	// Elapsed time

  public:
    FlickerLightingAction(DOOM::Doom & doom) :
      DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Lighting>(doom),
      _cycle(0),
      _flicker(0),
      _elapsed(sf::Time::Zero)
    {
      static_assert(CycleDuration != 0 && FlickerDuration != 0 && CycleDuration >= FlickerDuration, "Invalid FlickerLightingAction parameters.");
    }

    ~FlickerLightingAction() override = default;

    virtual void	update(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed) override	// Update light sector
    {
      // Update elapsed time
      _elapsed += elapsed;

      // Regenerate random parameter
      while (_elapsed > DOOM::Doom::Tic * (float)_cycle) {
	_elapsed -= DOOM::Doom::Tic * (float)_cycle;
	_cycle = (int16_t)(Math::Random() * CycleDuration) + FlickerDuration + 1;
	_flicker = (int16_t)(Math::Random() * FlickerDuration) + 1;
      }

      // Compute light value from elapsed time
      if (_elapsed < DOOM::Doom::Tic * (float)_flicker)
	sector.light_current = sector.light_base;
      else
	sector.light_current = std::min(sector.light_base + 16, 255);
    }
  };
};

#endif