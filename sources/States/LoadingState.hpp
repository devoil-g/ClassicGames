#pragma once

#include <functional>
#include <future>
#include <thread>
#include <vector>

#include <SFML/Graphics/Text.hpp>

#include "States/AbstractState.hpp"

namespace Game
{
  class LoadingState : public Game::AbstractState
  {
  private:
    sf::Text    _text;    // Loading text
    float       _elapsed; // Time elapsed

  public:
    LoadingState(Game::StateMachine& machine);
    ~LoadingState();

    bool  update(sf::Time) override;  // Update menu state
    void  draw() override;            // Draw menu state
  };
}