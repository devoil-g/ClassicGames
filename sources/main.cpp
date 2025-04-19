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
#include "RolePlayingGame/ClientScene.hpp"
#include "RolePlayingGame/Server.hpp"
#include "Scenes/ExitScene.hpp"

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
    //game.push<Game::SplashScene>();
    game.push<Game::MainMenuScene>();

    // TODO: remove this
    //auto server = std::make_unique<RPG::Server>(Game::Config::ExecutablePath / "assets" / "rpg" / "world.json");
    //server->run();
    //game.push<Game::ExitScene<RPG::ClientScene>>(std::move(server));

    // Run the game !
    while (Game::Window::Instance().isOpen() == true) {
      float elapsed = clock.restart().asSeconds();

      // Stop if update return true
      if (Game::Window::Instance().update(elapsed) == true ||
        Game::Audio::Sound::Instance().update(elapsed) == true ||
        game.update(elapsed) == true)
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