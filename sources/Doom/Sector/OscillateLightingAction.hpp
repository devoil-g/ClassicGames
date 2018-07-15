#ifndef _OSCILLATE_LIGHTING_ACTION_HPP_
#define _OSCILLATE_LIGHTING_ACTION_HPP_

#include "Doom/Doom.hpp"

namespace DOOM
{
  template<unsigned int Period = 32>
  class OscillateLightingAction : public DOOM::Doom::Level::Sector::AbstractAction
  {
  private:
    sf::Time	_elapsed;

  public:
    OscillateLightingAction() :
      DOOM::Doom::Level::Sector::AbstractAction(),
      _elapsed(sf::Time::Zero)
    {}

    ~OscillateLightingAction() override
    {}

    void	update(DOOM::Doom::Level::Sector & sector, sf::Time elapsed) override	// Update light action
    {
      // Update elapsed time
      _elapsed += elapsed;

      int16_t	darkest = sector.getNeighborLowestLight();

      // Not glow if all connected sectors have the same or higher brightness
      if (darkest >= sector.baseLight())
	sector.light() = sector.baseLight();

      // Return glowing light level
      else
	sector.light() = (int16_t)((std::cos(_elapsed.asSeconds() / (DOOM::Doom::Tic.asSeconds() * Period / 2) * Math::Pi) + 1.f) / 2.f * (sector.baseLight() - darkest) + darkest);
    }
  };
};

#endif