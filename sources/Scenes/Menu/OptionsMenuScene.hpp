#pragma once

#include <tuple>
#include <vector>

#include <SFML/Graphics/Text.hpp>

#include "Scenes/Menu/AbstractMenuScene.hpp"

namespace Game
{
  class OptionsMenuScene : public Game::AbstractMenuScene
  {
  private:
    void  selectFullscreen(Game::AbstractMenuScene::Item&);   // Toggle fullscreen mode
    void  selectAntialiasing(Game::AbstractMenuScene::Item&); // Change antialiasing level
    void  selectReturn(Game::AbstractMenuScene::Item&);       // Return to main menu

  public:
    OptionsMenuScene(Game::SceneMachine& machine);
    ~OptionsMenuScene() override = default;

    bool  update(sf::Time elapsed) override;  // Update state
    void  draw() override;                    // Draw state
  };
}