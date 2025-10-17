#pragma once

#include <cstdint>

#include "Doom/Action/AbstractTypeAction.hpp"

namespace DOOM
{
  template<unsigned int Period = 32>
  class OscillateLightingAction : public DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Lighting>
  {
  private:
    float _elapsed;

  public:
    OscillateLightingAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector) :
      DOOM::AbstractTypeAction<DOOM::Doom::Level::Sector::Action::Lighting>(doom, sector),
      _elapsed(0.f)
    {}

    ~OscillateLightingAction() override = default;

    void  update(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, float elapsed) override  // Update light action
    {
      // Update elapsed time
      _elapsed += elapsed;

      std::int16_t  darkest = sector.getNeighborLowestLight(doom);

      // Not glow if all connected sectors have the same or higher brightness
      if (darkest >= sector.light_base)
        sector.light_current = sector.light_base;

      // Return glowing light level
      else
        sector.light_current = (std::int16_t)((std::cos(_elapsed / (DOOM::Doom::Tic * Period / 2) * Math::Pi) + 1.f) / 2.f * (sector.light_base - darkest) + darkest);
    }
  };
}