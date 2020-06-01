#include <iostream>

#include "Doom/States/DoomState.hpp"
#include "Doom/States/StartDoomState.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

const sf::Time  DOOM::DoomState::ForcedExit = sf::seconds(1.f);

DOOM::DoomState::DoomState(Game::StateMachine& machine) :
  Game::AbstractState(machine),
  _doom(),
  _game(),
  _elapsed(sf::Time::Zero),
  _texture(),
  _sprite()
{
  // Load WAD
  _doom.load(Game::Config::ExecutablePath + "assets/levels/doom.wad");

  // Check that at least one level is available
  if (_doom.getLevels().empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Push initial state
  _game.push<DOOM::StartDoomState>(_doom);

  sf::Image image;

  // Initialize force exit bar
  image.create(1, 1, sf::Color::White);
  _texture.create(1, 1);
  _texture.setSmooth(false);
  _texture.update(image);
  _sprite.setTexture(_texture);
}

bool	DOOM::DoomState::update(sf::Time elapsed)
{
  // Update exit timer
  _elapsed += elapsed;

  // Reset timer when ESC is not pressed
  if (Game::Window::Instance().keyboard().key(sf::Keyboard::Escape) == false)
    _elapsed = sf::Time::Zero;

  // Exit if limit reached
  if (_elapsed > DOOM::DoomState::ForcedExit) {
    _machine.pop();
    return false;
  }

  // Update game
  if (_game.update(elapsed) == true) {
    _machine.pop();
    return false;
  }

  return false;
}

void	DOOM::DoomState::draw()
{
  // Draw game
  _game.draw();

  // Draw forced exit bar
  _sprite.setScale(Game::Window::Instance().window().getSize().x * _elapsed.asSeconds() / DOOM::DoomState::ForcedExit.asSeconds(), 3.f);
  Game::Window::Instance().window().draw(_sprite);
}