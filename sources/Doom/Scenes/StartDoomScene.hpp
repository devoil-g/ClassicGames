#pragma once

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System.hpp>

#include "Doom/Doom.hpp"
#include "Scenes/AbstractScene.hpp"
  
namespace DOOM
{
  class StartDoomScene : public Game::AbstractScene
  {
  private:
    DOOM::Doom& _doom;  // DOOM instance

    std::array<int, 4>      _players;       // Players controles (-1 = none, 0 = mouse/keyboard, 1+ = controller id+1)
    sf::Texture             _textureTitle;
    sf::Sprite              _spriteTitle;
    sf::Text                _subtitle;      // Subtitle "Player selection"
    std::array<sf::Text, 4> _controllers;   // Name of the controller of the player or "PRESS START"
    sf::Text                _ready;         // Start button, active if at least one controller is assigned
    sf::Time                _elapsed;       // Total elapsed time

    sf::Texture _textureKeyboard, _textureController;
    sf::Sprite  _spriteKeyboard, _spriteController;

    void  updateRegister(const int id);   // Register ID in player list
    void  updateUnregister(const int id); // Unregister ID in player list
    
  public:
    StartDoomScene(Game::SceneMachine& machine, DOOM::Doom& doom);
    ~StartDoomScene() override = default;

    bool  update(sf::Time elapsed) override;  // Update state
    void  draw() override;                    // Draw state
  };
}