#pragma once

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System.hpp>

#include "Doom/Doom.hpp"
#include "States/AbstractState.hpp"
  
namespace DOOM
{
  class SplashDoomState : public Game::AbstractState
  {
  private:
    DOOM::Doom& _doom;  // DOOM instance

    sf::Texture _texture; // Image in graphic memory
    sf::Sprite  _sprite;  // Display rendered texture

  public:
    SplashDoomState(Game::StateMachine& machine, DOOM::Doom& doom);
    ~SplashDoomState() override = default;

    bool  update(sf::Time elapsed) override;  // Update state
    void  draw() override;                    // Draw state
  };
}