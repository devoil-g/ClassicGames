#include "System/Library/FontLibrary.hpp"
#include "Scenes/Menu/OptionsMenuScene.hpp"
#include "Scenes/SceneMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

Game::OptionsMenuScene::OptionsMenuScene(Game::SceneMachine& machine) :
  Game::AbstractMenuScene(machine)
{
  // Options menu text
  std::string fullscreenText = std::string("Fullscreen [") + std::string(Game::Window::Instance().getFullscreen() == true ? "X" : "   ") + std::string("]");
  std::string antialiasingText = "Antialiasing [";
  std::string verticalText = std::string("Vertical Sync. [") + std::string((Game::Window::Instance().getVerticalSync() == true) ? "X" : "   ") + std::string("]");
  std::string returnText = "Return";

  for (unsigned int i = 1; i <= 3; i++)
    if (Game::Window::Instance().getAntialiasing() >= i)
      antialiasingText += "|";
    else
      antialiasingText += ".";

  antialiasingText += "]";

  // Set menu items/handlers
  title("Options");
  add(fullscreenText, std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&Game::OptionsMenuScene::selectFullscreen, this, std::placeholders::_1)));
  add(antialiasingText, std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&Game::OptionsMenuScene::selectAntialiasing, this, std::placeholders::_1)));
  add(verticalText, std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&Game::OptionsMenuScene::selectVerticalSync, this, std::placeholders::_1)));
  footer(returnText, std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&Game::OptionsMenuScene::selectReturn, this, std::placeholders::_1)));
}

bool  Game::OptionsMenuScene::update(float elapsed)
{
  // Update menu
  return Game::AbstractMenuScene::update(elapsed);
}

void  Game::OptionsMenuScene::selectFullscreen(Game::AbstractMenuScene::Item& item)
{
  // Toogle fullscreen mode
  Game::Window::Instance().setFullscreen(!Game::Window::Instance().getFullscreen());
  item.setString(std::string("Fullscreen [") + ((Game::Window::Instance().getFullscreen() == true) ? "X" : "  ") + "]");
}

void  Game::OptionsMenuScene::selectAntialiasing(Game::AbstractMenuScene::Item& item)
{
  Game::Window::Instance().setAntialiasing((Game::Window::Instance().getAntialiasing() + 1) % 4);

  // Generate new menu item text
  std::string text = "Antialiasing [";

  for (unsigned int i = 1; i <= 3; i++)
    if (Game::Window::Instance().getAntialiasing() >= i)
      text += "|";
    else
      text += ".";

  text += "]";

  // Apply text to menu item
  item.setString(text);
}

void  Game::OptionsMenuScene::selectVerticalSync(Game::AbstractMenuScene::Item& item)
{
  // Toggle vertical sync
  Game::Window::Instance().setVerticalSync(!Game::Window::Instance().getVerticalSync());
  item.setString(std::string("Vertical Sync. [") + std::string((Game::Window::Instance().getVerticalSync() == true) ? "X" : "   ") + std::string("]"));
}

void  Game::OptionsMenuScene::selectReturn(Game::AbstractMenuScene::Item &)
{
  // Return to main menu
  _machine.pop();
}