#pragma once

#include <string>

#include "Scenes/Menu/AbstractMenuScene.hpp"

namespace GBC
{
  class SelectionScene : public Game::AbstractMenuScene
  {
  private:
    void  selectGame(Game::AbstractMenuScene::Item&, const std::string& file);  // Start emulation with given game
    void  selectBrowse(Game::AbstractMenuScene::Item&);                         // Select file to start game
    
    std::string _selected;

  public:
    SelectionScene(Game::SceneMachine& machine);
    ~SelectionScene() override = default;

    bool  update(sf::Time elapsed) override;  // Update state
    void  draw() override;                    // Draw state
  };
}