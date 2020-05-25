#pragma once

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System.hpp>

#include "Doom/Camera.hpp"
#include "Doom/Doom.hpp"
#include "States/AbstractState.hpp"
#include "States/LoadingState.hpp"
#include "Math/Vector.hpp"

namespace Game
{
  class GameDoomState : public Game::AbstractState
  {
  public:
    static unsigned int RenderScale;  // Scaling factor of resolution

  private:
    DOOM::Doom  _doom;    // Instance of current level
    sf::Image   _image;   // Target for software rendering
    sf::Texture _texture; // Image in graphic memory
    sf::Sprite  _sprite;  // Display rendered texture

  public:
    GameDoomState(Game::StateMachine& machine, const std::vector<int>& players);
    ~GameDoomState() override = default;

    bool  update(sf::Time elapsed) override;  // Update state
    void  draw() override;                    // Draw state

    void  addPlayer(int controller);  // Add player to the game
  };
}