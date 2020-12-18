#include <iostream>

#include "Doom/States/DoomState.hpp"
#include "Doom/States/StartDoomState.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Sound.hpp"

const sf::Time  DOOM::DoomState::ForcedExit = sf::seconds(1.f);

DOOM::DoomState::DoomState(Game::StateMachine& machine) :
  Game::AbstractState(machine),
  _doom(),
  _game(),
  _elapsed(sf::Time::Zero),
  _bar(sf::Vector2f(1.f, 1.f))
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
  _bar.setSize(sf::Vector2f(1.f, 1.f));
  _bar.setFillColor(sf::Color::White);
}

DOOM::DoomState::~DoomState()
{
  // Interrupt playing DOOM sounds to avoid reading deleted buffers
  Game::Sound::Instance().clear();
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
  // Draw game in DOOM rendering target
  _game.draw();

  // Update texture on VRam
  if (_texture.getSize() != _doom.image.getSize() && _texture.create(_doom.image.getSize().x, _doom.image.getSize().y) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  _texture.update(_doom.image);
  
  // Making sure the texture is not filtered
  _texture.setSmooth(false);

  // Update sprite texture
  _sprite.setTexture(_texture, true);
  
  // Compute sprite scale and position
  float	scale = std::min((float)Game::Window::Instance().window().getSize().x / (float)_doom.image.getSize().x, (float)Game::Window::Instance().window().getSize().y / ((float)_doom.image.getSize().y * DOOM::Doom::RenderStretching));
  float	pos_x = (((float)Game::Window::Instance().window().getSize().x - ((float)_doom.image.getSize().x * scale)) / 2.f);
  float	pos_y = (((float)Game::Window::Instance().window().getSize().y - ((float)_doom.image.getSize().y * scale * DOOM::Doom::RenderStretching)) / 2.f);

  // Position sprite in window
  _sprite.setScale(sf::Vector2f(scale, scale * DOOM::Doom::RenderStretching));
  _sprite.setPosition(sf::Vector2f(pos_x, pos_y));

  // Draw DOOM rendering target
  Game::Window::Instance().window().draw(_sprite);

  // Draw forced exit bar
  _bar.setScale(Game::Window::Instance().window().getSize().x * _elapsed.asSeconds() / DOOM::DoomState::ForcedExit.asSeconds(), 4.f);
  Game::Window::Instance().window().draw(_bar);


}