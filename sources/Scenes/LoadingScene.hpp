#pragma once

#include <functional>
#include <future>
#include <thread>
#include <vector>

#include <SFML/Graphics/Text.hpp>

#include "Scenes/AbstractScene.hpp"

namespace Game
{
  class LoadingScene : public Game::AbstractScene
  {
  private:
    sf::Text  _text;    // Loading text
    sf::Time  _elapsed; // Time elapsed

  public:
    LoadingScene(Game::SceneMachine& machine);
    ~LoadingScene();

    bool  update(sf::Time) override;  // Update menu state
    void  draw() override;            // Draw menu state
  };
}