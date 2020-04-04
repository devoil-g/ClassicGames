#pragma once

#include <tuple>
#include <vector>

#include <SFML/Graphics/Text.hpp>

#include "States/Menu/AbstractMenuState.hpp"

namespace Game
{
  class MainMenuState : public Game::AbstractMenuState
  {
  private:
    void  selectDoom(Game::AbstractMenuState::Item&);     // Create a new game
    void  selectOptions(Game::AbstractMenuState::Item&);  // Go to option menu
    void  selectExit(Game::AbstractMenuState::Item&);     // Exit game

  public:
    MainMenuState();
    ~MainMenuState();

    bool  update(sf::Time) override;  // Update state
    void  draw() override;            // Draw state
  };
}