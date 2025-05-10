#include <iostream>

#include "Doom/Scenes/DoomScene.hpp"
#include "GameBoyColor/SelectionScene.hpp"
#include "Quiz/QuizScene.hpp"
#include "RolePlayingGame/ClientScene.hpp"
#include "RolePlayingGame/Server.hpp"
#include "Scenes/Menu/MainMenuScene.hpp"
#include "Scenes/Menu/OptionsMenuScene.hpp"
#include "Scenes/ExitScene.hpp"
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
  add("DOOM", [this](auto& item) { selectDoom(item, Game::Config::ExecutablePath / "assets" / "levels" / "doom.wad", DOOM::Enum::Mode::ModeRetail); });
  add("DOOM II", [this](auto& item) { selectDoom(item, Game::Config::ExecutablePath / "assets" / "levels" / "doom2.wad", DOOM::Enum::Mode::ModeCommercial); });
  add("Game Boy", [this](auto& item) { selectGameBoy(item); });
  add("Quiz", [this](auto& item) { selectQuiz(item); });
  add("RPG Host", [this](auto& item) { selectGameHost(item); });
  add("RPG Join", [this](auto& item) { selectGameJoin(item); });
  add("Options", [this](auto& item) { selectOptions(item); });
  footer("Exit", [this](auto& item) { selectExit(item); });
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
      machine.swap<Game::ExitScene<DOOM::DoomScene>>(wad, mode);
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
    machine.swap<Game::ExitScene<QUIZ::QuizScene>>();
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
      auto server = std::make_unique<RPG::Server>(Game::Config::ExecutablePath / "assets" / "rpg" / "level_01.json");

      server->run();
      machine.swap<Game::ExitScene<RPG::ClientScene>>(std::move(server));
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
