#include <chrono>
#include <iostream>
#include <stdexcept>

#include "Scenes/SplashScene.hpp"
#include "Scenes/SceneMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Audio/Sound.hpp"

#ifdef _WIN32
# include <windows.h>
#else
# include <X11/Xlib.h>
#endif

// TODO: remove this
#include "RolePlayingGame/ClientScene.hpp"
#include "RolePlayingGame/Server.hpp"
#include "Scenes/ExitScene.hpp"
#include "Scenes/Menu/MainMenuScene.hpp"

namespace Game
{
  void  initialize(int argc, char ** argv)
  {
    Game::Config::initialize(argc, argv);
  }

  void  help()
  {}

  void  run()
  {
    Game::SceneMachine  game;
    auto                clock = std::chrono::steady_clock::now();

    // Push initial state
    //game.push<Game::SplashScene>();
    game.push<Game::MainMenuScene>();
    //game.push<Game::ExitScene<RPG::ClientScene>>(std::make_unique<RPG::Server>(Game::Config::ExecutablePath / "assets" / "rpg" / "world.json"));

    // Run the game !
    while (Game::Window::Instance().isOpen() == true) {
      auto now = std::chrono::steady_clock::now();
      auto elapsed = std::chrono::duration<float>(now - clock);

      // Update clock
      clock = now;

      // Stop if update return true
      if (Game::Window::Instance().update(elapsed.count()) == true ||
        Game::Audio::Sound::Instance().update(elapsed.count()) == true ||
        game.update(elapsed.count()) == true)
        return;

      // Render game
      Game::Window::Instance().clear();
      game.draw();
      Game::Window::Instance().display();
    }
  }
};

int main(int argc, char ** argv)
{
  try {
    Game::initialize(argc, argv);
    Game::help();
    Game::run();
  }
  catch (const std::exception& e) {
    std::cerr << "[Runtime Error]: " << e.what() << std::endl;

#ifdef _WIN32
    MessageBox(Game::Window::Instance().getHandle(), e.what(), "Runtime error", MB_OK | MB_ICONSTOP);
#endif

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}