#pragma once

#include <string>

#include <SFML/Graphics/Text.hpp>

#include "Scenes/AbstractScene.hpp"

namespace Game
{
  class MessageScene : public Game::AbstractScene
  {
  private:
    sf::Text  _message;   // Message to display
    sf::Text  _return;    // Return button
    int       _selected;  // Index of selected menu (-1 for no selection)

  public:
    MessageScene(Game::SceneMachine& machine, const std::string& message);
    ~MessageScene() override = default;

    bool  update(sf::Time) override;  // Update state
    void  draw() override;            // Draw state
  };
}