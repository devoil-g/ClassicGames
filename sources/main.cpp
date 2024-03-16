#include <stdexcept>
#include <iostream>

#include "Scenes/SplashScene.hpp"
#include "Scenes/Menu/MainMenuScene.hpp"
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
#include "RolePlayingGame/GameServer.hpp"
#include "RolePlayingGame/GameScene.hpp"

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
    sf::Clock           clock;

    // Push initial state
    game.push<Game::SplashScene>();

    // TODO: remove this
    auto server = std::make_unique<RPG::GameServer>(Game::Config::ExecutablePath / "assets" / "rpg" / "level_01.json");
    server->run();
    game.push<RPG::GameScene>(std::move(server));

    // Run the game !
    while (Game::Window::Instance().window().isOpen()) {
      float elapsed = clock.restart().asSeconds();

      // Stop if update return true
      if (Game::Window::Instance().update(elapsed) == true ||
        Game::Audio::Sound::Instance().update(elapsed) == true ||
        game.update(elapsed) == true)
        return;

      // Draw image
      Game::Window::Instance().window().clear();
      game.draw();

      // Display image
      Game::Window::Instance().window().display();
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
    MessageBox(Game::Window::Instance().window().getSystemHandle(), e.what(), "Runtime error", MB_OK | MB_ICONSTOP);
#endif

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}