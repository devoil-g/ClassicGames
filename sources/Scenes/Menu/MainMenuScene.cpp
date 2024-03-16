#include <iostream>

#include "Doom/Scenes/DoomScene.hpp"
#include "GameBoyColor/SelectionScene.hpp"
#include "Quiz/QuizScene.hpp"
#include "RolePlayingGame/GameScene.hpp"
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
  // Set menu items/handlers
  title("ClassicGames");
  add("DOOM", std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&Game::MainMenuScene::selectDoom, this, std::placeholders::_1, Game::Config::ExecutablePath / "assets" / "levels" / "doom.wad", DOOM::Enum::Mode::ModeRetail)));
  add("DOOM II", std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&Game::MainMenuScene::selectDoom, this, std::placeholders::_1, Game::Config::ExecutablePath / "assets" / "levels" / "doom2.wad", DOOM::Enum::Mode::ModeCommercial)));
  add("Game Boy", std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&Game::MainMenuScene::selectGameBoy, this, std::placeholders::_1)));
  add("Quiz", std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&Game::MainMenuScene::selectQuiz, this, std::placeholders::_1)));
  add("RPG Host", std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&Game::MainMenuScene::selectGameHost, this, std::placeholders::_1)));
  add("RPG Join", std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&Game::MainMenuScene::selectGameJoin, this, std::placeholders::_1)));
  add("Options", std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&Game::MainMenuScene::selectOptions, this, std::placeholders::_1)));
  footer("Exit", std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&Game::MainMenuScene::selectExit, this, std::placeholders::_1)));
}

bool  Game::MainMenuScene::update(float elapsed)
{
  // Update menu
  return Game::AbstractMenuScene::update(elapsed);
}

void  Game::MainMenuScene::selectDoom(Game::AbstractMenuScene::Item&, const std::filesystem::path& wad, DOOM::Enum::Mode mode)
{
  Game::SceneMachine& machine = _machine;

  // Push loading screen
  _machine.push<Game::LoadingScene>();

  // Start to load DOOM game
  std::thread([&machine, wad, mode]() {
    try {
      machine.swap<DOOM::DoomScene>(wad, mode);
    }
    catch (const std::exception&) {
      machine.swap<Game::MessageScene>("Error: failed to run DOOM.");
    }
    }).detach();
}

void  Game::MainMenuScene::selectGameBoy(Game::AbstractMenuScene::Item&)
{
  // Go to Game Boy emulator game selection
  try {
    _machine.push<GBC::SelectionScene>();
  }

  // Error message when no game found
  catch (const std::exception&) {
    _machine.push<Game::MessageScene>("Error: no games found in /assets/gbc/.");
  }
}

void  Game::MainMenuScene::selectQuiz(Game::AbstractMenuScene::Item&)
{
  Game::SceneMachine& machine = _machine;

  // Push loading screen
  _machine.push<Game::LoadingScene>();

  std::thread([&machine]() {
    // Go to quiz scene
    try {
    machine.swap<QUIZ::QuizScene>();
  }

  // Error message when quiz loading failed
  catch (const std::exception&) {
    machine.swap<Game::MessageScene>("Error: failed to load quiz.");
  }
    }).detach();
}

void  Game::MainMenuScene::selectGameHost(Game::AbstractMenuScene::Item&)
{
  Game::SceneMachine& machine = _machine;

  // Push loading screen
  _machine.push<Game::LoadingScene>();

  // Start a RPG server then a client
  std::thread([&machine]() {
    try {
      auto server = std::make_unique<RPG::GameServer>(Game::Config::ExecutablePath / "assets" / "rpg" / "level_01.json");
      
      server->run();
      machine.push<RPG::GameScene>(std::move(server));
    }
    catch (const std::exception& e) {
      //machine.swap<Game::MessageScene>("Error: failed to start RPG.");
      machine.swap<Game::MessageScene>(e.what());
    }
    }).detach();
}

void  Game::MainMenuScene::selectGameJoin(Game::AbstractMenuScene::Item&)
{
  _machine.push<Game::MessageScene>("Not implemented.");
}

void  Game::MainMenuScene::selectOptions(Game::AbstractMenuScene::Item&)
{
  // Go to option menu
  _machine.push<Game::OptionsMenuScene>();
}

void  Game::MainMenuScene::selectExit(Game::AbstractMenuScene::Item &)
{
  // Exit application
  _machine.clear();
}
