#ifndef _FLICKER_LIGHTING_ACTION_HPP_
#define _FLICKER_LIGHTING_ACTION_HPP_

#include "Doom/Doom.hpp"

namespace DOOM
{
  template<unsigned int CycleDuration = 32, unsigned int FlickerDuration = 4>
  class FlickerLightingAction : public DOOM::Doom::Level::Sector::AbstractAction
  {
  private:
    int16_t	_cycle;		// Cycle duration
    int16_t	_flicker;	// Flicker duration
    sf::Time	_elapsed;	// Elapsed time

  public:
    FlickerLightingAction() :
      DOOM::Doom::Level::Sector::AbstractAction(),
      _cycle(0),
      _flicker(0),
      _elapsed(sf::Time::Zero)
    {
      static_assert(CycleDuration != 0 && FlickerDuration != 0 && CycleDuration >= FlickerDuration, "Invalid FlickerLightingAction parameters.");
    }

    ~FlickerLightingAction() override
    {}

    virtual void	update(DOOM::Doom::Level::Sector & sector, sf::Time elapsed) override	// Update light sector
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
	sector.light() = sector.baseLight();
      else
	sector.light() = std::min(sector.baseLight() + 16, 255);
    }
  };
};

#endif