#pragma once

#include <SFML/Graphics/Text.hpp>

#include "Scenes/AbstractScene.hpp"

namespace Game
{
  class StartScene : public Game::AbstractScene
  {
  private:
    float     _elapsed; // Elapsed time
    sf::Text  _text;    // Text to be displayed

  public:
    StartScene(Game::SceneMachine& machine);
    ~StartScene() override = default;

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}