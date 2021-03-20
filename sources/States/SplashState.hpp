#pragma once

#include <SFML/Graphics/Sprite.hpp>

#include "States/AbstractState.hpp"

namespace Game
{
  class SplashState : public Game::AbstractState
  {
    sf::Time    _elapsed; // Time elasped since state creation
    sf::Sprite  _sprite;  // Sprite containing application logo

  public:
    SplashState(Game::StateMachine& machine);
    ~SplashState() override = default;

    bool  update(sf::Time) override;  // Update state
    void  draw() override;            // Draw state
  };
}