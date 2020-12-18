#pragma once

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System.hpp>

#include "Doom/Doom.hpp"
#include "States/AbstractState.hpp"
#include "States/StateMachine.hpp"
  
namespace DOOM
{
  class DoomState : public Game::AbstractState
  {
  private:
    static const sf::Time ForcedExit; // Forced exit time limit

    DOOM::Doom          _doom;    // Main DOOM instance
    Game::StateMachine  _game;    // DOOM state machine
    sf::Texture         _texture; // Image in graphic memory
    sf::Sprite          _sprite;  // Display rendered texture

    sf::Time            _elapsed; // Timer of forced exit
    sf::RectangleShape  _bar;     // Forced exit bar
    
  public:
    DoomState(Game::StateMachine& machine);
    ~DoomState() override;

    bool  update(sf::Time elapsed) override;  // Update state
    void  draw() override;                    // Draw state
  };
}