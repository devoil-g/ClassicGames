#include <iostream>

#include "System/Library/FontLibrary.hpp"
#include "States/Doom/StartDoomState.hpp"
#include "States/Menu/MainMenuState.hpp"
#include "States/Menu/OptionsMenuState.hpp"
#include "States/MessageState.hpp"
#include "States/StateMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

Game::MainMenuState::MainMenuState()
  : Game::AbstractMenuState()
{
  // Get menu font
  sf::Font const &	font = Game::FontLibrary::Instance().get(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf");

  // Set menu items/handlers
  menu() =
  {
    Game::AbstractMenuState::Item("DOOM", font, std::function<void(Game::AbstractMenuState::Item &)>(std::bind(&Game::MainMenuState::selectDoom, this, std::placeholders::_1))),
    Game::AbstractMenuState::Item("Options", font, std::function<void(Game::AbstractMenuState::Item &)>(std::bind(&Game::MainMenuState::selectOptions, this, std::placeholders::_1))),
    Game::AbstractMenuState::Item("Exit", font, std::function<void(Game::AbstractMenuState::Item &)>(std::bind(&Game::MainMenuState::selectExit, this, std::placeholders::_1)))
  };
}

Game::MainMenuState::~MainMenuState()
{}

bool	Game::MainMenuState::update(sf::Time elapsed)
{
  // Update menu
  return Game::AbstractMenuState::update(elapsed);
}

void	Game::MainMenuState::draw()
{
  // Draw menu
  Game::AbstractMenuState::draw();
}

void	Game::MainMenuState::selectDoom(Game::AbstractMenuState::Item &)
{
  // Go to DOOM menu
  Game::StateMachine::Instance().push(new Game::StartDoomState());
}

void	Game::MainMenuState::selectOptions(Game::AbstractMenuState::Item &)
{
  // Go to option menu
  Game::StateMachine::Instance().push(new Game::OptionsMenuState());
}

void	Game::MainMenuState::selectExit(Game::AbstractMenuState::Item &)
{
  // Exit application
  Game::StateMachine::Instance().pop();
  Game::StateMachine::Instance().pop();
}
