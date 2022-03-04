#include <iostream>

#include "Doom/Scenes/TextDoomScene.hpp"
#include "Doom/Scenes/TransitionDoomScene.hpp"
#include "Doom/Thing/PlayerThing.hpp"
#include "System/Window.hpp"

const unsigned int  DOOM::TextDoomScene::TextSpeed = 3;

DOOM::TextDoomScene::TextDoomScene(Game::SceneMachine& machine, DOOM::Doom& doom, const std::string& text, uint64_t background) :
  Game::AbstractScene(machine),
  _doom(doom),
  _elapsed(sf::Time::Zero),
  _text(),
  _image(),
  _x(10), _y(10)
{
  const auto& flat = doom.resources.getFlat(background).flat();

  // Resize image
  _image.create(DOOM::Doom::RenderWidth, DOOM::Doom::RenderHeight);

  // Draw background
  for (unsigned int y = 0; y < _image.getSize().y; y++)
    for (unsigned int x = 0; x < _image.getSize().x; x++)
      _image.setPixel(x, y, _doom.resources.palettes[0][_doom.resources.colormaps[0][flat[(y % 64) * 64 + x % 64]]]);

  // Send text character in queue
  for (char c : text)
    _text.push((c >= 'a' && c <= 'z') ? (c - 'a' + 'A') : (c));

  // TODO: start badass menu music
}

bool  DOOM::TextDoomScene::update(sf::Time elapsed)
{
  // Add elapsed time to timer
  _elapsed += elapsed;

  bool done = _text.empty();

  // Draw characters
  for (; (updateSkip() == true || _elapsed >= DOOM::Doom::Tic * (float)DOOM::TextDoomScene::TextSpeed) && _text.empty() == false; _elapsed -= DOOM::Doom::Tic * (float)DOOM::TextDoomScene::TextSpeed, _text.pop()) {
    // New line
    if (_text.front() == '\n') {
      _x = 10;
      _y += 11;
      continue;
    }

    // Draw character
    try {
      const auto& character = _doom.resources.getMenu(Game::Utilities::str_to_key<uint64_t>(std::string("STCFN") + std::to_string((int)_text.front() / 100 % 10) + std::to_string((int)_text.front() / 10 % 10) + std::to_string((int)_text.front() / 1 % 10)));

      character.draw(_doom, _image, sf::Vector2i(_x, _y), sf::Vector2i(1, 1));
      _x += character.width;
    }
    catch (const std::exception&) {
      _x += 4;
    }
  }

  // Skip button has been pressed
  if (done == true && updateSkip() == true)
  {
    // Copy current screen
    sf::Image start = _doom.image;

    // Save references as 'this' is gonna be deleted
    auto& machine = _machine;
    auto& doom = _doom;

    // Pop text screen
    machine.pop();

    // Simulate first frame of next screen
    machine.draw();

    // Push transition
    machine.push<DOOM::TransitionDoomScene>(doom, start, doom.image);
    return false;
  }
  
  return false;
}

bool  DOOM::TextDoomScene::updateSkip()
{
  bool  skip = false;

  // Check skip key
  for (const auto& player : _doom.level.players) {
    // Keyboard/mouse
    if (player.get().controller == 0) {
      skip |= Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Space);
      skip |= Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Left);
    }

    else {
      skip |= Game::Window::Instance().joystick().buttonPressed(player.get().controller - 1, 0);
      skip |= Game::Window::Instance().joystick().buttonPressed(player.get().controller - 1, 1);
    }
  }

  return skip;
}

void  DOOM::TextDoomScene::draw()
{
  // Copy buffer
  _doom.image = _image;
}
