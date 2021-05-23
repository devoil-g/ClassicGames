#pragma once

#include <SFML/Graphics/Sprite.hpp>

#include "Scenes/AbstractScene.hpp"

namespace Game
{
  class SplashScene : public Game::AbstractScene
  {
    sf::Time    _elapsed; // Time elasped since state creation
    sf::Sprite  _sprite;  // Sprite containing application logo

  public:
    SplashScene(Game::SceneMachine& machine);
    ~SplashScene() override = default;

    bool  update(sf::Time) override;  // Update state
    void  draw() override;            // Draw state
  };
}