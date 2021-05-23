#pragma once

#include <tuple>
#include <vector>

#include <SFML/Graphics/Text.hpp>

#include "Scenes/Menu/AbstractMenuScene.hpp"

namespace Game
{
  class MainMenuScene : public Game::AbstractMenuScene
  {
  private:
    void  selectDoom(Game::AbstractMenuScene::Item&);     // Create a new game
    void  selectOptions(Game::AbstractMenuScene::Item&);  // Go to option menu
    void  selectExit(Game::AbstractMenuScene::Item&);     // Exit game

  public:
    MainMenuScene(Game::SceneMachine& machine);
    ~MainMenuScene() override = default;

    bool  update(sf::Time elapsed) override;  // Update state
    void  draw() override;                    // Draw state
  };
}