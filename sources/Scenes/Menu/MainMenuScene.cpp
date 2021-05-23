#include <iostream>

#include "System/Library/FontLibrary.hpp"
#include "Doom/Scenes/DoomScene.hpp"
#include "Scenes/Menu/MainMenuScene.hpp"
#include "Scenes/Menu/OptionsMenuScene.hpp"
#include "Scenes/LoadingScene.hpp"
#include "Scenes/MessageScene.hpp"
#include "Scenes/SceneMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

Game::MainMenuScene::MainMenuScene(Game::SceneMachine& machine) :
  Game::AbstractMenuScene(machine)
{
  // Get menu font
  const auto& font = Game::FontLibrary::Instance().get(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf");

  // Set menu items/handlers
  menu() = {
    Game::AbstractMenuScene::Item("DOOM", font, std::function<void(Game::AbstractMenuScene::Item &)>(std::bind(&Game::MainMenuScene::selectDoom, this, std::placeholders::_1))),
    Game::AbstractMenuScene::Item("Options", font, std::function<void(Game::AbstractMenuScene::Item &)>(std::bind(&Game::MainMenuScene::selectOptions, this, std::placeholders::_1))),
    Game::AbstractMenuScene::Item("Exit", font, std::function<void(Game::AbstractMenuScene::Item &)>(std::bind(&Game::MainMenuScene::selectExit, this, std::placeholders::_1)))
  };
}

bool  Game::MainMenuScene::update(sf::Time elapsed)
{
  // Update menu
  return Game::AbstractMenuScene::update(elapsed);
}

void  Game::MainMenuScene::draw()
{
  // Draw menu
  Game::AbstractMenuScene::draw();
}

void  Game::MainMenuScene::selectDoom(Game::AbstractMenuScene::Item&)
{
  Game::SceneMachine& machine = _machine;

  // Push loading screen
  _machine.push<Game::LoadingScene>();

  // Start to load DOOM game
  std::thread([&machine]() {
    try {
      machine.swap<DOOM::DoomScene>();
    }
    catch (std::exception exception) {
      machine.swap<Game::MessageScene>("Error: failed to run DOOM.\n");
    }
    }).detach();
}

void  Game::MainMenuScene::selectOptions(Game::AbstractMenuScene::Item &)
{
  // Go to option menu
  _machine.push<Game::OptionsMenuScene>();
}

void  Game::MainMenuScene::selectExit(Game::AbstractMenuScene::Item &)
{
  // Exit application
  _machine.clear();
}
