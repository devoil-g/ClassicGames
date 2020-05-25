#pragma once

#include <SFML/Graphics/Text.hpp>

#include "States/AbstractState.hpp"

namespace Game
{
  class StartState : public Game::AbstractState
  {
  private:
    float     _elapsed;
    sf::Text  _text;

  public:
    StartState(Game::StateMachine& machine);
    ~StartState() override = default;

    bool  update(sf::Time) override;  // Update state
    void  draw() override;            // Draw state
  };
}