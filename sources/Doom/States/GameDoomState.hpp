#pragma once

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "Doom/Doom.hpp"
#include "States/AbstractState.hpp"

namespace DOOM
{
  class GameDoomState : public Game::AbstractState
  {
  private:
    DOOM::Doom& _doom;    // DOOM instance

    void  addPlayer(int controller);  // Add player to the game
    void  end();                      // End level

  public:
    GameDoomState(Game::StateMachine& machine, DOOM::Doom& doom);
    ~GameDoomState() override = default;

    bool  update(sf::Time elapsed) override;  // Update state
    void  draw() override;                    // Draw state
  };
}