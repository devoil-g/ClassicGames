#pragma once

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "Doom/Doom.hpp"
#include "States/AbstractState.hpp"

namespace DOOM
{
  class IntermissionDoomState : public Game::AbstractState
  {
  private:
    DOOM::Doom& _doom;    // DOOM instance
    sf::Image   _image;   // Target for software rendering
    sf::Texture _texture; // Image in graphic memory
    sf::Sprite  _sprite;  // Display rendered texture

    int _killsTotal, _killsCurrent;   // Player kills percentage
    int _itemsTotal, _itemsCurrent;   // Player items percentage
    int _secretTotal, _secretCurrent; // Player secret percentage
    int _timeLevel, _timePar;         // Time of level completion 

  public:
    IntermissionDoomState(Game::StateMachine& machine, DOOM::Doom& doom);
    ~IntermissionDoomState() override = default;

    bool  update(sf::Time elapsed) override;  // Update state
    void  draw() override;                    // Draw state
  };
}