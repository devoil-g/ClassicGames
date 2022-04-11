#include <fstream>
#include <filesystem>

#include "GameBoyColor/EmulationScene.hpp"
#include "Math/Math.hpp"
#include "System/Window.hpp"

// TODO: remove this
#include <iostream>

const sf::Time  GBC::EmulationScene::ForcedExit = sf::seconds(1.f);

GBC::EmulationScene::EmulationScene(Game::SceneMachine& machine, const std::string& filename) :
  Game::AbstractScene(machine),
  _gbc(filename),
  _elapsed(sf::Time::Zero),
  _bar(sf::Vector2f(1.f, 1.f))
{
  // Initialize force exit bar
  _bar.setSize(sf::Vector2f(1.f, 1.f));
  _bar.setFillColor(sf::Color::White);
}

GBC::EmulationScene::~EmulationScene()
{}

bool  GBC::EmulationScene::update(sf::Time elapsed)
{
  // Update exit timer
  _elapsed += elapsed;

  // Reset timer when ESC is not pressed
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Escape) == false)
    _elapsed = sf::Time::Zero;

  // Exit if limit reached
  if (_elapsed > GBC::EmulationScene::ForcedExit) {
    _machine.pop();
    return false;
  }

  // Simulate a frame
  _gbc.simulate();

  return false;
}

void  GBC::EmulationScene::draw()
{
  // Only draw if rendering target is visible
  if (_gbc.screen().getSize().x != 0 && _gbc.screen().getSize().y != 0)
  {
    // Update texture on VRam
    if (_texture.getSize() != _gbc.screen().getSize() && _texture.create(_gbc.screen().getSize().x, _gbc.screen().getSize().y) == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    _texture.update(_gbc.screen());

    // Making sure the texture is not filtered
    _texture.setSmooth(false);

    // Update sprite texture
    _sprite.setTexture(_texture, true);

    // Compute sprite scale and position
    float scale = std::min((float)Game::Window::Instance().window().getSize().x / (float)_gbc.screen().getSize().x, (float)Game::Window::Instance().window().getSize().y / (float)_gbc.screen().getSize().y);
    float pos_x = (((float)Game::Window::Instance().window().getSize().x - ((float)_gbc.screen().getSize().x * scale)) / 2.f);
    float pos_y = (((float)Game::Window::Instance().window().getSize().y - ((float)_gbc.screen().getSize().y * scale)) / 2.f);

    // Position sprite in window
    _sprite.setScale(sf::Vector2f(scale, scale));
    _sprite.setPosition(sf::Vector2f(pos_x, pos_y));

    // Draw DOOM rendering target
    Game::Window::Instance().window().draw(_sprite);
  }

  // Draw forced exit bar
  _bar.setScale(Game::Window::Instance().window().getSize().x * _elapsed.asSeconds() / GBC::EmulationScene::ForcedExit.asSeconds(), 4.f);
  Game::Window::Instance().window().draw(_bar);
}