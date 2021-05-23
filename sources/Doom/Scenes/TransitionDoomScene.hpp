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

    sf::Image   _start;      // Start image
    sf::Image   _end;        // End (background) image
    sf::Image   _transition; // Transition (animation) image
    
    std::vector<sf::Time> _offsets; // Temporal offset of pixel column

    void  drawImage(sf::Image& image);

  public:
    TransitionDoomScene(Game::SceneMachine& machine, DOOM::Doom& doom, const sf::Image& start, const sf::Image& end);
    ~TransitionDoomScene() override = default;

    bool  update(sf::Time elapsed) override;  // Update state
    void  draw() override;                    // Draw state
  };
}