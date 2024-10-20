#include "System/Library/FontLibrary.hpp"
#include "Scenes/MessageScene.hpp"
#include "Scenes/SceneMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

Game::MessageScene::MessageScene(Game::SceneMachine& machine, const std::string& message) :
  Game::AbstractScene(machine),
  _message(message, Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "04b03.ttf")),
  _return("Return", Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "04b03.ttf")),
  _selected(-1)
{}

bool  Game::MessageScene::update(float elapsed)
{
  // Return to previous menu
  if (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Button::Right) ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Escape) ||
    Game::Window::Instance().joystick().buttonPressed(0, 1))
  {
    _machine.pop();
    return false;
  }

  // Move down in menu or up with keyboard or joystick
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Down) == true || Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Up) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::S) == true || Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Z) == true ||
    (Game::Window::Instance().joystick().relative(0, sf::Joystick::Axis::PovY) < -0.9f && Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovY) != 0.f) || (Game::Window::Instance().joystick().relative(0, sf::Joystick::Axis::PovY) > +0.9f && Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovY) != 0.f))
    _selected = 0;

  // Select menu if mouse cursor is over when moving
  if ((Game::Window::Instance().mouse().relative().x != 0 || Game::Window::Instance().mouse().relative().y != 0))
  {
    _selected = -1;
    if (Game::Window::Instance().mouse().position().x > _return.getGlobalBounds().left &&
      Game::Window::Instance().mouse().position().x < _return.getGlobalBounds().left + _return.getGlobalBounds().width &&
      Game::Window::Instance().mouse().position().y > _return.getGlobalBounds().top &&
      Game::Window::Instance().mouse().position().y < _return.getGlobalBounds().top + _return.getGlobalBounds().height)
      _selected = 0;
  }

  // Select menu with mouse, keyboard or joystick
  if ((Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Button::Left) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Space) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Return) == true ||
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
  float scale = 0.75f * std::min((float)Game::Window::Instance().window().getSize().x / (float)_message.getLocalBounds().width, (float)Game::Window::Instance().window().getSize().y / (float)_message.getLocalBounds().height);

  _message.setScale(scale, scale);
  _message.setPosition(sf::Vector2f((Game::Window::Instance().window().getSize().x - _message.getGlobalBounds().width) / 2.f, (Game::Window::Instance().window().getSize().y - _message.getLocalBounds().height) / 2.f));

  // Set return button size
  if (_selected == -1)
    _return.setCharacterSize(Game::Window::Instance().window().getSize().y / 16);
  else
    _return.setCharacterSize(Game::Window::Instance().window().getSize().y / 12);

  // Set return button position
  _return.setPosition(sf::Vector2f((Game::Window::Instance().window().getSize().x - _return.getLocalBounds().width) / 2.f, Game::Window::Instance().window().getSize().y * (7.f / 8.f) - _return.getLocalBounds().height / 2.f));

  // Draw message and return button
  Game::Window::Instance().window().draw(_message);
  Game::Window::Instance().window().draw(_return);
}