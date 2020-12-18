#include <iostream>

#include "Doom/States/SplashDoomState.hpp"
#include "Doom/States/MenuDoomState.hpp"
#include "System/Window.hpp"

DOOM::SplashDoomState::SplashDoomState(Game::StateMachine& machine, DOOM::Doom& doom) :
  Game::AbstractState(machine),
  _doom(doom)
{
  // TODO: start badass menu music
}

bool	DOOM::SplashDoomState::update(sf::Time elapsed)
{
  // Push to main menu
  if (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Button::Left) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Return) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Space) == true ||
    Game::Window::Instance().joystick().buttonPressed(0, 0) == true ||
    Game::Window::Instance().joystick().buttonPressed(0, 7) == true) {
    _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_swtchn);
    _machine.push<DOOM::MenuDoomState>(_doom);
  }

  return false;
}

void	DOOM::SplashDoomState::draw()
{
  const auto& title = _doom.resources.menus.find(DOOM::str_to_key("TITLEPIC"));

  // Check that the titlecard is loaded
  if (title == _doom.resources.menus.end())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Draw splash screen
  _doom.image = title->second.image(_doom);

  // Does nothing, already drawn in constructor
}