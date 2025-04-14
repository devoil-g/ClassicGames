#include <algorithm>
#include <stdexcept>

#include <SFML/Graphics/RectangleShape.hpp>

#include "System/Library/TextureLibrary.hpp"
#include "Scenes/SplashScene.hpp"
#include "Scenes/Menu/MainMenuScene.hpp"
#include "Scenes/SceneMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

Game::SplashScene::SplashScene(Game::SceneMachine& machine) :
  Game::AbstractScene(machine),
  _elapsed(-1.f),
  _sprite(Game::TextureLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "textures" / "splashscreen.png"))
{
  // Center sprite
  _sprite.setOrigin({ _sprite.getTexture().getSize().x / 2.f, _sprite.getTexture().getSize().y / 2.f });
}

bool  Game::SplashScene::update(float elapsed)
{
  // Add elapsed time to counter
  _elapsed += elapsed;

  // Skip splash screen
  if (_elapsed > 0.5f && _elapsed < 4.5f &&
    (Game::Window::Instance().mouse().buttonPressed(Game::Window::MouseButton::Left) == true ||
      Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Enter) == true ||
      Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Space) == true ||
      Game::Window::Instance().joystick().buttonPressed(0, 0) == true ||
      Game::Window::Instance().joystick().buttonPressed(0, 7) == true))
    _elapsed = 4.5f;

  // End splash screen
  if (_elapsed >= 5.f)
    _machine.swap<Game::MainMenuScene>();

  return false;
}

void  Game::SplashScene::draw()
{
  sf::RectangleShape  rect;
  std::uint8_t        alpha;

  // Get alpha for fade in/out
  if (_elapsed < 0.f)
    alpha = 255;
  else if (_elapsed < 0.5f)
    alpha = (std::uint8_t)((0.5f - _elapsed) * 2 * 255);
  else if (_elapsed > 4.5f)
    alpha = (std::uint8_t)((_elapsed - 4.5f) * 2 * 255);
  else
    alpha = 0;

  // Set fade in/out rectange parameters
  rect.setSize(sf::Vector2f((float)Game::Window::Instance().getSize().x(), (float)Game::Window::Instance().getSize().y()));
  rect.setFillColor(sf::Color(0, 0, 0, alpha));

  // Position sprite in window
  const auto& texture = _sprite.getTexture();
  float       scale = std::min(1.f, std::min((float)Game::Window::Instance().getSize().x() / (float)texture.getSize().x, (float)Game::Window::Instance().getSize().y() / (float)texture.getSize().y));

  _sprite.setScale({ scale, scale });
  _sprite.setPosition({ Game::Window::Instance().getSize().x() / 2.f, Game::Window::Instance().getSize().y() / 2.f });

  // Draw splash screen
  Game::Window::Instance().draw(_sprite);
  Game::Window::Instance().draw(rect);
}