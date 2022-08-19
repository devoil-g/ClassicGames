#pragma once

#include <vector>

#include "Doom/Doom.hpp"
#include "Scenes/AbstractScene.hpp"
  
namespace DOOM
{
  class TransitionDoomScene : public Game::AbstractScene
  {
  private:
    DOOM::Doom& _doom;  // DOOM instance

    sf::Image   _startImage;        // Start image
    sf::Texture _endTexture;        // End (background) texture
    sf::Sprite  _endSprite;         // End (background) sprite
    sf::Image   _transitionImage;   // Transition (animation) image
    sf::Texture _transitionTexture; // Transition (animation) texture
    sf::Sprite  _transitionSprite;  // Transition (animation) sprite
    
    std::vector<sf::Time> _offsets; // Temporal offset of pixel columns

  public:
    TransitionDoomScene(Game::SceneMachine& machine, DOOM::Doom& doom, const sf::Image& start, const sf::Image& end);
    ~TransitionDoomScene() override = default;

    bool  update(sf::Time elapsed) override;  // Update state
    void  draw() override;                    // Draw state
  };
}