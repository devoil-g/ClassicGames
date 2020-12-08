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
  // Draw game
  _game.draw();

  // Draw forced exit bar
  _bar.setScale(Game::Window::Instance().window().getSize().x * _elapsed.asSeconds() / DOOM::DoomState::ForcedExit.asSeconds(), 4.f);
  Game::Window::Instance().window().draw(_bar);
}