#pragma once

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
    int   _cycle;   // Cycle duration
    int   _flicker; // Flicker duration
    float _elapsed; // Elapsed time

  public:
    FlickerLightingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector) :
      DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Lighting>(doom, sector),
      _cycle(0),
      _flicker(0),
      _elapsed(0.f)
    {
      static_assert(CycleDuration != 0 && FlickerDuration != 0 && CycleDuration >= FlickerDuration, "Invalid FlickerLightingAction parameters.");
    }

    ~FlickerLightingAction() override = default;

    virtual void  update(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float elapsed) override  // Update light sector
    {
      // Update elapsed time
      _elapsed += elapsed;

      // Regenerate random parameter
      while (_elapsed > DOOM::Doom::Tic * (float)_cycle) {
        _elapsed -= DOOM::Doom::Tic * (float)_cycle;
        _cycle = (int)(Math::Random() * CycleDuration) + FlickerDuration + 1;
        _flicker = (int)(Math::Random() * FlickerDuration) + 1;
      }

      // Compute light value from elapsed time
      if (_elapsed < DOOM::Doom::Tic * (float)_flicker)
        sector.light_current = sector.light_base;
      else
        sector.light_current = std::min(sector.light_base + 16, 255);
    }
  };
}