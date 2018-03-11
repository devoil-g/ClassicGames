#include "States/Menu/AbstractMenuState.hpp"
#include "States/StateMachine.hpp"
#include "System/Window.hpp"

Game::AbstractMenuState::AbstractMenuState()
  : _menu(), _selected(-1)
{}

Game::AbstractMenuState::~AbstractMenuState()
{}

bool	Game::AbstractMenuState::update(sf::Time elapsed)
{
  // Return to previous menu
  if (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Button::Right) ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Escape) ||
    Game::Window::Instance().joystick().buttonPressed(0, 1))
  {
    Game::StateMachine::Instance().pop();
    return false;
  }

  // Move down in menu with keyboard or joystick
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Down) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::S) == true ||
    (Game::Window::Instance().joystick().relative(0, sf::Joystick::Axis::PovY) < -0.9f && Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovY) != 0.f))
    if (_selected == -1)
      _selected = 0;
    else
      _selected = (_selected + 1) % _menu.size();

  // Move up in menu with keyboard or joystick
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Up) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Z) == true ||
    (Game::Window::Instance().joystick().relative(0, sf::Joystick::Axis::PovY) > +0.9f && Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovY) != 0.f))
    if (_selected == -1)
      _selected = 0;
    else
      _selected = (_selected - 1 + (int)_menu.size()) % (int)_menu.size();

  // Select menu if mouse cursor is over when moving
  if ((Game::Window::Instance().mouse().relative().x != 0 || Game::Window::Instance().mouse().relative().y != 0))
  {
    _selected = -1;

    for (unsigned int i = 0; i < _menu.size(); i++)
    {
      sf::FloatRect	bound = _menu[i].getGlobalBounds();

      if (Game::Window::Instance().mouse().position().x > bound.left &&
	Game::Window::Instance().mouse().position().x < bound.left + bound.width &&
	Game::Window::Instance().mouse().position().y > bound.top &&
	Game::Window::Instance().mouse().position().y < bound.top + bound.height)
	_selected = i;
    }
  }

  // Select menu with mouse, keyboard or joystick
  if ((Game::Window::Instance().mouse().buttonReleased(sf::Mouse::Button::Left) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Space) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Return) == true ||
    Game::Window::Instance().joystick().buttonPressed(0, 0) == true)
    && _selected != -1)
    _menu[_selected].select();

  return false;
}

void	Game::AbstractMenuState::draw()
{
  // Set font size of menu
  for (unsigned int i = 0; i < _menu.size(); i++)
    if (i == _selected)
      _menu[i].setCharacterSize(Game::Window::Instance().window().getSize().y / 12);
    else
      _menu[i].setCharacterSize(Game::Window::Instance().window().getSize().y / 16);

  // Set position and draw menu items
  for (unsigned int i = 0; i < _menu.size(); i++)
  {
    _menu[i].setPosition(
      (Game::Window::Instance().window().getSize().x - _menu[i].getLocalBounds().width) / 2.f,
      Game::Window::Instance().window().getSize().y / 4.f
      + Game::Window::Instance().window().getSize().y / (2.f * _menu.size()) * i
      + Game::Window::Instance().window().getSize().y / (4.f * _menu.size())
      - _menu[i].getLocalBounds().height / 2.f
    );

    Game::Window::Instance().window().draw(_menu[i]);
  }
}
