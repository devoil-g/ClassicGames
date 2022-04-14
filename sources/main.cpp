#include <stdexcept>
#include <iostream>

#include "Scenes/SplashScene.hpp"
#include "Scenes/Menu/MainMenuScene.hpp"
#include "Scenes/SceneMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Audio/Sound.hpp"

#include "Scenes/MidiScene.hpp"
#include "GameBoyColor/EmulationScene.hpp"

#ifdef _WIN32
# include <windows.h>
#else
# include <X11/Xlib.h>
#endif

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
    game.push<GBC::EmulationScene>(Game::Config::ExecutablePath + "/assets/gbc/tetris.gb");

    // Run the game !
    while (Game::Window::Instance().window().isOpen()) {
      sf::Time  elapsed = clock.restart();

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
#ifdef __linux__
    // Initializes Xlib support for concurrent threads
    //if (XInitThreads() == 0)
    //throw std::runtime_error("XInitThreads() failed.");
#endif

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