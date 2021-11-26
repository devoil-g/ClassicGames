#include "System/Library/FontLibrary.hpp"
#include "Scenes/Menu/OptionsMenuScene.hpp"
#include "Scenes/SceneMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

Game::OptionsMenuScene::OptionsMenuScene(Game::SceneMachine& machine) :
  Game::AbstractMenuScene(machine)
{
  // Get menu font
  const auto& font = Game::FontLibrary::Instance().get(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf");

  // Options menu text
  std::string fullscreenText = std::string("Fullscreen [") + std::string(Game::Window::Instance().window().getSize().x == sf::VideoMode::getDesktopMode().width && Game::Window::Instance().window().getSize().y == sf::VideoMode::getDesktopMode().height ? "X" : "   ") + std::string("]");
  std::string antialiasingText = "Antialiasing [";
  std::string verticalText = std::string("Vertical Sync. [") + std::string((Game::Window::Instance().getVerticalSync() == true) ? "X" : "   ") + std::string("]");
  std::string returnText = "Return";

  for (unsigned int i = 2; i <= 8; i *= 2)
    if (Game::Window::Instance().window().getSettings().antialiasingLevel >= i)
      antialiasingText += "|";
    else
      antialiasingText += ".";

  antialiasingText += "]";

  // Set menu items/handlers
  menu() = {
    Game::AbstractMenuScene::Item(fullscreenText, font, std::function<void(Game::AbstractMenuScene::Item &)>(std::bind(&Game::OptionsMenuScene::selectFullscreen, this, std::placeholders::_1))),
    Game::AbstractMenuScene::Item(antialiasingText, font, std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&Game::OptionsMenuScene::selectAntialiasing, this, std::placeholders::_1))),
    Game::AbstractMenuScene::Item(verticalText, font, std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&Game::OptionsMenuScene::selectVerticalSync, this, std::placeholders::_1))),
    Game::AbstractMenuScene::Item(returnText, font, std::function<void(Game::AbstractMenuScene::Item &)>(std::bind(&Game::OptionsMenuScene::selectReturn, this, std::placeholders::_1)))
  };
}

bool  Game::OptionsMenuScene::update(sf::Time elapsed)
{
  // Update menu
  return Game::AbstractMenuScene::update(elapsed);
}

void  Game::OptionsMenuScene::draw()
{
  // Draw menu
  Game::AbstractMenuScene::draw();
}

void  Game::OptionsMenuScene::selectFullscreen(Game::AbstractMenuScene::Item& item)
{
  // Save context settings
  sf::ContextSettings settings = Game::Window::Instance().window().getSettings();

  // Toogle fullscreen mode
  if (Game::Window::Instance().window().getSize().x != sf::VideoMode::getDesktopMode().width ||
    Game::Window::Instance().window().getSize().y != sf::VideoMode::getDesktopMode().height)
  {
    Game::Window::Instance().create(sf::VideoMode::getDesktopMode(), sf::Style::Fullscreen, settings, Game::Window::Instance().getVerticalSync());
    item.setString("Fullscreen [X]");
  }
  else
  {
    Game::Window::Instance().create(sf::VideoMode(Game::Window::DefaultWidth, Game::Window::DefaultHeight), sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close, settings, Game::Window::Instance().getVerticalSync());
    item.setString("Fullscreen [   ]");
  }
}

void  Game::OptionsMenuScene::selectAntialiasing(Game::AbstractMenuScene::Item& item)
{
  sf::VideoMode       video;
  unsigned int        style;
  sf::ContextSettings settings = Game::Window::Instance().window().getSettings();

  // Get actual video mode and window style
  if (Game::Window::Instance().window().getSize().x == sf::VideoMode::getDesktopMode().width &&
    Game::Window::Instance().window().getSize().y == sf::VideoMode::getDesktopMode().height)
  {
    video = sf::VideoMode::getDesktopMode();
    style = sf::Style::Fullscreen;
  }
  else
  {
    video = sf::VideoMode(Game::Window::Instance().window().getSize().x, Game::Window::Instance().window().getSize().y);
    style = sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close;
  }

  // Set new antialiasing level
  if (settings.antialiasingLevel <= 0)
    settings.antialiasingLevel = 8;
  else
    settings.antialiasingLevel = ((settings.antialiasingLevel == 2) ? 0 : (settings.antialiasingLevel / 2));

  // Create window with new parameters
  Game::Window::Instance().create(video, style, settings, Game::Window::Instance().getVerticalSync());

  // Generate new menu item text
  std::string text = "Antialiasing [";

  for (unsigned int i = 2; i <= 8; i *= 2)
    if (Game::Window::Instance().window().getSettings().antialiasingLevel >= i)
      text += "|";
    else
      text += ".";

  text += "]";

  // Apply text to menu item
  item.setString(text);
}

void  Game::OptionsMenuScene::selectVerticalSync(Game::AbstractMenuScene::Item& item)
{
  sf::VideoMode       video;
  unsigned int        style;
  sf::ContextSettings settings = Game::Window::Instance().window().getSettings();

  // Get actual video mode and window style
  if (Game::Window::Instance().window().getSize().x == sf::VideoMode::getDesktopMode().width &&
    Game::Window::Instance().window().getSize().y == sf::VideoMode::getDesktopMode().height)
  {
    video = sf::VideoMode::getDesktopMode();
    style = sf::Style::Fullscreen;
  }
  else
  {
    video = sf::VideoMode(Game::Window::Instance().window().getSize().x, Game::Window::Instance().window().getSize().y);
    style = sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close;
  }

  // Create window with new parameters, toggle vertical sync
  Game::Window::Instance().create(video, style, settings, !Game::Window::Instance().getVerticalSync());

  // Apply new text to menu item
  item.setString(std::string("Vertical Sync. [") + std::string((Game::Window::Instance().getVerticalSync() == true) ? "X" : "   ") + std::string("]"));
}

void  Game::OptionsMenuScene::selectReturn(Game::AbstractMenuScene::Item &)
{
  // Return to main menu
  _machine.pop();
}