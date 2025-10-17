#include "System/Library/FontLibrary.hpp"
#include "Scenes/MessageScene.hpp"
#include "Scenes/SceneMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

Game::MessageScene::MessageScene(Game::SceneMachine& machine, const std::string& message) :
  Game::AbstractScene(machine),
  _message(Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "04b03.ttf"), message),
  _return(Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "04b03.ttf"), "Return"),
  _selected(-1)
{}

bool  Game::MessageScene::update(float elapsed)
{
  // Return to previous menu
  if (Game::Window::Instance().mouse().buttonPressed(Game::Window::MouseButton::Right) ||
    Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Escape) ||
    Game::Window::Instance().joystick().buttonPressed(0, 1))
  {
    _machine.pop();
    return false;
  }

  // Move down in menu or up with keyboard or joystick
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Down) == true || Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Up) == true ||
    Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::S) == true || Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Z) == true ||
    (Game::Window::Instance().joystick().relative(0, Game::Window::JoystickAxis::PovY) < -0.9f && Game::Window::Instance().joystick().position(0, Game::Window::JoystickAxis::PovY) != 0.f) || (Game::Window::Instance().joystick().relative(0, Game::Window::JoystickAxis::PovY) > +0.9f && Game::Window::Instance().joystick().position(0, Game::Window::JoystickAxis::PovY) != 0.f))
    _selected = 0;

  // Select menu if mouse cursor is over when moving
  if ((Game::Window::Instance().mouse().relative().x() != 0 || Game::Window::Instance().mouse().relative().y() != 0))
  {
    _selected = -1;
    if (_return.getGlobalBounds().contains({ (float)Game::Window::Instance().mouse().position().x(), (float)Game::Window::Instance().mouse().position().y() }) == true)
      _selected = 0;
  }

  // Select menu with mouse, keyboard or joystick
  if ((Game::Window::Instance().mouse().buttonPressed(Game::Window::MouseButton::Left) == true ||
    Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Space) == true ||
    Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Enter) == true ||
    Game::Window::Instance().joystick().buttonPressed(0, 0) == true)
    && _selected == 0)
  {
    _machine.pop();
    return false;
  }

  return false;
}

void  Game::MessageScene::draw()
{
  // Set message position to the center of the screen
  float scale = 0.75f * std::min((float)Game::Window::Instance().getSize().x() / (float)_message.getLocalBounds().size.x, (float)Game::Window::Instance().getSize().y() / (float)_message.getLocalBounds().size.y);

  _message.setScale({ scale, scale });
  _message.setPosition(sf::Vector2f((Game::Window::Instance().getSize().x() - _message.getGlobalBounds().size.x) / 2.f, (Game::Window::Instance().getSize().y() - _message.getLocalBounds().size.y) / 2.f));

  // Set return button size
  if (_selected == -1)
    _return.setCharacterSize(Game::Window::Instance().getSize().y() / 16);
  else
    _return.setCharacterSize(Game::Window::Instance().getSize().y() / 12);

  // Set return button position
  _return.setPosition(sf::Vector2f((Game::Window::Instance().getSize().x() - _return.getLocalBounds().size.x) / 2.f, Game::Window::Instance().getSize().y() * (7.f / 8.f) - _return.getLocalBounds().size.y / 2.f));

  // Draw message and return button
  Game::Window::Instance().draw(_message);
  Game::Window::Instance().draw(_return);
}