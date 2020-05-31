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
    DOOM::Doom          _doom;  // Main DOOM instance
    Game::StateMachine  _game;  // DOOM state machine
    
  public:
    DoomState(Game::StateMachine& machine);
    ~DoomState() override = default;

    bool  update(sf::Time elapsed) override;  // Update state
    void  draw() override;                    // Draw state
  };
}