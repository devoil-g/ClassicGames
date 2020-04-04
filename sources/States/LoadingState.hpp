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
    std::future<Game::AbstractState*> _loader;  // Load task returing new state when completed
    sf::Text                          _text;    // Loading text
    float                             _elapsed; // Time elapsed

  public:
    LoadingState(std::future<Game::AbstractState*>&&);
    ~LoadingState();

    bool  update(sf::Time) override;  // Update menu state
    void  draw() override;            // Draw menu state
  };
}