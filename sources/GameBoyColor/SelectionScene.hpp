#pragma once

#include <filesystem>
#include <string>

#include "Scenes/Menu/AbstractMenuScene.hpp"

namespace GBC
{
  class SelectionScene : public Game::AbstractMenuScene
  {
  private:
    void  selectGame(Game::AbstractMenuScene::Item&, const std::filesystem::path& file);  // Start emulation with given game
    void  selectBrowse(Game::AbstractMenuScene::Item&);                                   // Select file to start game
    
    std::filesystem::path _selected;

    std::filesystem::path browse() const;

  public:
    SelectionScene(Game::SceneMachine& machine);
    ~SelectionScene() override = default;

    bool  update(float elapsed) override;  // Update state
  };
}