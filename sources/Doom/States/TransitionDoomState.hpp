#pragma once

#include <vector>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "Doom/Doom.hpp"
#include "States/AbstractState.hpp"
  
namespace DOOM
{
  class TransitionDoomState : public Game::AbstractState
  {
  private:
    sf::Image   _start;   // Start image
    sf::Image   _end;     // End image
    sf::Image   _image;   // Rendering target
    sf::Texture _texture; // Target on VRAM
    sf::Sprite  _sprite;  // Target drawable

    std::vector<sf::Time> _offsets; // Temporal offset of pixel column

  public:
    TransitionDoomState(Game::StateMachine& machine, const sf::Image& start, const sf::Image& end);
    ~TransitionDoomState() override = default;

    bool  update(sf::Time elapsed) override;  // Update state
    void  draw() override;                    // Draw state
  };
}