#include <iostream>

#include "Doom/States/SplashDoomState.hpp"
#include "Doom/States/MenuDoomState.hpp"
#include "System/Window.hpp"

DOOM::SplashDoomState::SplashDoomState(Game::StateMachine& machine, DOOM::Doom& doom) :
  Game::AbstractState(machine),
  _doom(doom),
  _texture(),
  _sprite()
{
  const auto& title = _doom.resources.menus.find(DOOM::str_to_key("TITLEPIC"));

  // Check that the titlecard is loaded
  if (title == _doom.resources.menus.end())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  sf::Image image = title->second.image(_doom);

  // Send image to GC
  _texture.create(image.getSize().x, image.getSize().y);
  _texture.setSmooth(false);
  _texture.update(image);
  _sprite.setTexture(_texture, true);

  // TODO: start badass menu music
}

bool	DOOM::SplashDoomState::update(sf::Time elapsed)
{
  // Push to main menu
  if (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Button::Left) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Return) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Space) == true ||
    Game::Window::Instance().joystick().buttonPressed(0, 0) == true ||
    Game::Window::Instance().joystick().buttonPressed(0, 7) == true)
    _machine.push<DOOM::MenuDoomState>(_doom);

  return false;
}

void	DOOM::SplashDoomState::draw()
{
  // Compute sprite scale and position
  float	scale = std::min((float)Game::Window::Instance().window().getSize().x / (float)_texture.getSize().x, (float)Game::Window::Instance().window().getSize().y / ((float)_texture.getSize().y * DOOM::Doom::RenderStretching));
  float	pos_x = (((float)Game::Window::Instance().window().getSize().x - ((float)_texture.getSize().x * scale)) / 2.f);
  float	pos_y = (((float)Game::Window::Instance().window().getSize().y - ((float)_texture.getSize().y * scale * DOOM::Doom::RenderStretching)) / 2.f);

  // Position sprite in window
  _sprite.setScale(sf::Vector2f(scale, scale * DOOM::Doom::RenderStretching));
  _sprite.setPosition(sf::Vector2f(pos_x, pos_y));

  // Draw sprite
  Game::Window::Instance().window().draw(_sprite);
}