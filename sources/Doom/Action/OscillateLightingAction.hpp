#ifndef _OSCILLATE_LIGHTING_ACTION_HPP_
#define _OSCILLATE_LIGHTING_ACTION_HPP_

#include <SFML/System/Time.hpp>

#include "Doom/Action/AbstractAction.hpp"

namespace DOOM
{
  template<unsigned int Period = 32>
  class OscillateLightingAction : public DOOM::AbstractAction
  {
  private:
    sf::Time	_elapsed;

  public:
    OscillateLightingAction(DOOM::Doom & doom) :
      DOOM::AbstractAction(doom),
      _elapsed(sf::Time::Zero)
    {}

    ~OscillateLightingAction() override = default;

    void	update(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed) override	// Update light action
    {
      // Update elapsed time
      _elapsed += elapsed;

      int16_t	darkest = sector.getNeighborLowestLight(doom);

      // Not glow if all connected sectors have the same or higher brightness
      if (darkest >= sector.light_base)
	sector.light_current = sector.light_base;

      // Return glowing light level
      else
	sector.light_current = (int16_t)((std::cos(_elapsed.asSeconds() / (DOOM::Doom::Tic.asSeconds() * Period / 2) * Math::Pi) + 1.f) / 2.f * (sector.light_base - darkest) + darkest);
    }
  };
};

#endif