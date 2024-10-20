#pragma once

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System.hpp>

#include "Doom/Doom.hpp"
#include "Scenes/AbstractScene.hpp"
#include "Scenes/SceneMachine.hpp"
  
namespace DOOM
{
  class DoomScene : public Game::AbstractScene
  {
  private:
    static const float  ForcedExit; // Forced exit time limit

    DOOM::Doom          _doom;    // Main DOOM instance
    Game::SceneMachine  _game;    // DOOM state machine
    sf::Texture         _texture; // Image in graphic memory
    sf::Sprite          _sprite;  // Display rendered texture

    float               _elapsed; // Timer of forced exit
    sf::RectangleShape  _bar;     // Forced exit bar
    
  public:
    DoomScene(Game::SceneMachine& machine, const std::filesystem::path& wad, DOOM::Enum::Mode mode);
    ~DoomScene() override;

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}