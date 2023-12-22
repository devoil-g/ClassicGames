#pragma once

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System.hpp>

#include "Doom/Doom.hpp"
#include "Scenes/AbstractScene.hpp"
  
namespace DOOM
{
  class SplashDoomScene : public Game::AbstractScene
  {
  private:
    DOOM::Doom& _doom;  // DOOM instance

  public:
    SplashDoomScene(Game::SceneMachine& machine, DOOM::Doom& doom);
    ~SplashDoomScene() override = default;

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}