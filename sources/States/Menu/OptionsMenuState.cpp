#include "System/Library/FontLibrary.hpp"
#include "States/Menu/OptionsMenuState.hpp"
#include "States/StateMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

Game::OptionsMenuState::OptionsMenuState(Game::StateMachine& machine) :
  Game::AbstractMenuState(machine)
{
  // Get menu font
  const auto& font = Game::FontLibrary::Instance().get(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf");

  // Options menu text
  std::string fullscreenText = std::string("Fullscreen [") + std::string(Game::Window::Instance().window().getSize().x == sf::VideoMode::getDesktopMode().width && Game::Window::Instance().window().getSize().y == sf::VideoMode::getDesktopMode().height ? "X" : "   ") + std::string("]");
  std::string antialiasingText = "Antialiasing [";
  std::string returnText = "Return";

  for (unsigned int i = 2; i <= 8; i *= 2)
    if (Game::Window::Instance().window().getSettings().antialiasingLevel >= i)
      antialiasingText += "|";
    else
      antialiasingText += ".";

  antialiasingText += "]";

  // Set menu items/handlers
  menu() = {
    Game::AbstractMenuState::Item(fullscreenText, font, std::function<void(Game::AbstractMenuState::Item &)>(std::bind(&Game::OptionsMenuState::selectFullscreen, this, std::placeholders::_1))),
    Game::AbstractMenuState::Item(antialiasingText, font, std::function<void(Game::AbstractMenuState::Item &)>(std::bind(&Game::OptionsMenuState::selectAntialiasing, this, std::placeholders::_1))),
    Game::AbstractMenuState::Item(returnText, font, std::function<void(Game::AbstractMenuState::Item &)>(std::bind(&Game::OptionsMenuState::selectReturn, this, std::placeholders::_1)))
  };
}

bool  Game::OptionsMenuState::update(sf::Time elapsed)
{
  // Update menu
  return Game::AbstractMenuState::update(elapsed);
}

void  Game::OptionsMenuState::draw()
{
  // Draw menu
  Game::AbstractMenuState::draw();
}

void  Game::OptionsMenuState::selectFullscreen(Game::AbstractMenuState::Item& item)
{
  // Save context settings
  sf::ContextSettings settings = Game::Window::Instance().window().getSettings();

  // Toogle fullscreen mode
  if (Game::Window::Instance().window().getSize().x != sf::VideoMode::getDesktopMode().width ||
    Game::Window::Instance().window().getSize().y != sf::VideoMode::getDesktopMode().height)
  {
    Game::Window::Instance().create(sf::VideoMode::getDesktopMode(), sf::Style::Fullscreen, settings);
    item.setString("Fullscreen [X]");
  }
  else
  {
    Game::Window::Instance().create(sf::VideoMode(Game::Window::DefaultWidth, Game::Window::DefaultHeight), sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close, settings);
    item.setString("Fullscreen [   ]");
  }
}

void  Game::OptionsMenuState::selectAntialiasing(Game::AbstractMenuState::Item& item)
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
  Game::Window::Instance().create(video, style, settings);

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

void  Game::OptionsMenuState::selectReturn(Game::AbstractMenuState::Item &)
{
  // Return to main menu
  _machine.pop();
}
