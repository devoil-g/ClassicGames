#pragma once

#include <string>

#include <SFML/Graphics/Text.hpp>

#include "States/AbstractState.hpp"

namespace Game
{
  class MessageState : public Game::AbstractState
  {
  private:
    sf::Text  _message;   // Message to display
    sf::Text  _return;    // Return button
    int       _selected;  // Index of selected menu (-1 for no selection)

  public:
    MessageState(Game::StateMachine& machine, const std::string& message);
    ~MessageState() override = default;

    bool  update(sf::Time) override;  // Update state
    void  draw() override;            // Draw state
  };
}