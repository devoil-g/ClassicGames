#pragma once

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
    int16_t _cycle;   // Cycle duration
    int16_t _flash;   // Flicker duration
    float   _elapsed; // Elapsed time

  public:
    RandomLightingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector) :
      DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Lighting>(doom, sector),
      _cycle(0),
      _flash(0),
      _elapsed(0.f)
    {
      static_assert(CycleDuration != 0 && FlashDuration != 0 && CycleDuration >= FlashDuration, "Invalid RandomLightingAction parameters.");
    }

    ~RandomLightingAction() override = default;

    virtual void  update(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float elapsed) override  // Update light sector
    {
      // Update elapsed time
      _elapsed += elapsed;

      // Regenerate random parameter
      while (_elapsed > DOOM::Doom::Tic* (float)_cycle) {
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
}