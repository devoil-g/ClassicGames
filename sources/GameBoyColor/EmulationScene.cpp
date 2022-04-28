#include <fstream>
#include <filesystem>

#include "GameBoyColor/EmulationScene.hpp"
#include "Math/Math.hpp"
#include "System/Window.hpp"

const sf::Time  GBC::EmulationScene::ForcedExit = sf::seconds(1.f);

GBC::EmulationScene::EmulationScene(Game::SceneMachine& machine, const std::string& filename) :
  Game::AbstractScene(machine),
  _gbc(filename),
  _fps(sf::seconds(-1.f)),
  _exit(sf::Time::Zero),
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
  // Update timers
  _exit += elapsed;
  _fps += elapsed;

  // Reset exit timer when ESC is not pressed
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Escape) == false)
    _exit = sf::Time::Zero;

  // Exit if limit reached
  if (_exit > GBC::EmulationScene::ForcedExit) {
    _machine.pop();
    return false;
  }

  // Simulate frames at 59.72 fps
  if (_fps.asSeconds() >= 70224.f / Math::Pow<22>(2)) {
    _gbc.simulate();
    _fps -= sf::seconds(70224.f / Math::Pow<22>(2));
  }

  return false;
}

void  GBC::EmulationScene::draw()
{
  const sf::Image&  image = _gbc.screen();

  // Only draw if rendering target is visible
  if (image.getSize().x != 0 && image.getSize().y != 0)
  {
    // Update texture on VRam
    if (_texture.getSize() != image.getSize() && _texture.create(image.getSize().x, image.getSize().y) == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    _texture.update(image);

    // Making sure the texture is not filtered
    _texture.setSmooth(false);

    // Update sprite texture
    _sprite.setTexture(_texture, true);

    // Compute sprite scale and position
    float scale = std::min((float)Game::Window::Instance().window().getSize().x / (float)image.getSize().x, (float)Game::Window::Instance().window().getSize().y / (float)image.getSize().y);
    float pos_x = (((float)Game::Window::Instance().window().getSize().x - ((float)image.getSize().x * scale)) / 2.f);
    float pos_y = (((float)Game::Window::Instance().window().getSize().y - ((float)image.getSize().y * scale)) / 2.f);

    // Position sprite in window
    _sprite.setScale(sf::Vector2f(scale, scale));
    _sprite.setPosition(sf::Vector2f(pos_x, pos_y));

    // Draw DOOM rendering target
    Game::Window::Instance().window().draw(_sprite);
  }

  // Draw forced exit bar
  _bar.setScale(Game::Window::Instance().window().getSize().x * _exit.asSeconds() / GBC::EmulationScene::ForcedExit.asSeconds(), 4.f);
  Game::Window::Instance().window().draw(_bar);
}