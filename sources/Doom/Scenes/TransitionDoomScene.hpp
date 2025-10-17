#pragma once

#include <vector>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

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
    sf::Image   _transitionImage;   // Transition (animation) image
    sf::Texture _transitionTexture; // Transition (animation) texture
    
    std::vector<float>  _offsets; // Temporal offset of pixel columns

  public:
    TransitionDoomScene(Game::SceneMachine& machine, DOOM::Doom& doom, const sf::Image& start, const sf::Image& end);
    ~TransitionDoomScene() override = default;

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}