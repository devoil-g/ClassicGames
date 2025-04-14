#pragma once

#include "Doom/Doom.hpp"
#include "Scenes/AbstractScene.hpp"

namespace DOOM
{
  class GameDoomScene : public Game::AbstractScene
  {
  private:
    DOOM::Doom& _doom;    // DOOM instance

    void  addPlayer(int controller);  // Add player to the game
    void  end();                      // End level

  public:
    GameDoomScene(Game::SceneMachine& machine, DOOM::Doom& doom);
    ~GameDoomScene() override = default;

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}