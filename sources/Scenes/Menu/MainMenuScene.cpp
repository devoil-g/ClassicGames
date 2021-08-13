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
    Game::AbstractMenuScene::Item("DOOM", font, std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&Game::MainMenuScene::selectDoom, this, std::placeholders::_1, Game::Config::ExecutablePath + "assets/levels/doom.wad", DOOM::Enum::Mode::ModeRetail))),
    Game::AbstractMenuScene::Item("DOOM II", font, std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&Game::MainMenuScene::selectDoom, this, std::placeholders::_1, Game::Config::ExecutablePath + "assets/levels/doom2.wad", DOOM::Enum::Mode::ModeCommercial))),
    Game::AbstractMenuScene::Item("DOOM II (test)", font, std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&Game::MainMenuScene::selectDoom, this, std::placeholders::_1, Game::Config::ExecutablePath + "assets/levels/doom2_test.wad", DOOM::Enum::Mode::ModeCommercial))),
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

void  Game::MainMenuScene::selectDoom(Game::AbstractMenuScene::Item&, const std::string& wad, DOOM::Enum::Mode mode)
{
  Game::SceneMachine& machine = _machine;

  // Push loading screen
  _machine.push<Game::LoadingScene>();

  // Start to load DOOM game
  std::thread([&machine, wad, mode]() {
    try {
      machine.swap<DOOM::DoomScene>(wad, mode);
    }
    catch (const std::exception& exception) {
      machine.swap<Game::MessageScene>("Error: failed to run DOOM.\n");
      machine.swap<Game::MessageScene>(exception.what());
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
