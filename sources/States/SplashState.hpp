#ifndef _SPLASH_STATE_HPP_
#define _SPLASH_STATE_HPP_

#include <SFML/Graphics/Sprite.hpp>

#include "States/AbstractState.hpp"

namespace Game
{
  class SplashState : public Game::AbstractState
  {
    float	_elapsed;	// Time elasped since state creation
    sf::Sprite	_sprite;	// Sprite containing application logo

  public:
    SplashState();
    ~SplashState() override;

    bool	update(sf::Time) override;	// Update state
    void	draw() override;		// Draw state
  };
};

#endif