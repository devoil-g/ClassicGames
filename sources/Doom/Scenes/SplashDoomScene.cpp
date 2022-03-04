#include <iostream>

#include "Doom/Scenes/SplashDoomScene.hpp"
#include "Doom/Scenes/MenuDoomScene.hpp"
#include "System/Window.hpp"

DOOM::SplashDoomScene::SplashDoomScene(Game::SceneMachine& machine, DOOM::Doom& doom) :
  Game::AbstractScene(machine),
  _doom(doom)
{
  // TODO: start badass menu music
}

bool  DOOM::SplashDoomScene::update(sf::Time elapsed)
{
  // Push to main menu
  if (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Button::Left) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Return) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Space) == true ||
    Game::Window::Instance().joystick().buttonPressed(0, 0) == true ||
    Game::Window::Instance().joystick().buttonPressed(0, 7) == true) {
    _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_swtchn);
    _machine.push<DOOM::MenuDoomScene>(_doom, true);
  }

  return false;
}

void  DOOM::SplashDoomScene::draw()
{
  // Draw splash screen
  _doom.image = _doom.resources.getMenu(Game::Utilities::str_to_key<uint64_t>("TITLEPIC")).image(_doom);
}