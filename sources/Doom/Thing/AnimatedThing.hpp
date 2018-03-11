#ifndef _ANIMATED_THING_HPP_
#define _ANIMATED_THING_HPP_

#include <utility>

#include <SFML/System/Time.hpp>

#include "Doom/Thing/AbstractThing.hpp"
#include "Doom/Doom.hpp"

namespace Game
{
  class AnimatedThing : public virtual AbstractThing
  {
    static const int	FrameDuration = 8;	// Tics between two frames of animation

  private:
    sf::Time									_elapsed;	// Total elapsed time
    std::vector<std::pair<Game::Doom::Resources::Texture const *, bool>[8]>	_sprites;	// Vector of sprites according to time/orientation
    
  public:
    AnimatedThing(const Game::Doom &, const std::string &, const std::string &, const Game::Wad::RawLevel::Thing &, int16_t, int16_t);
    virtual ~AnimatedThing();

    virtual bool								update(sf::Time) override;		// Update animation sequence
    virtual const std::pair<Game::Doom::Resources::Texture const *, bool> &	sprite(float) const override;		// Return sprite to be displayed, bool to true if mirrored
  };
};

#endif