#include <algorithm>
#include <stdexcept>

#include <SFML/Graphics/RectangleShape.hpp>

#include "System/Library/TextureLibrary.hpp"
#include "States/StartState.hpp"
#include "States/SplashState.hpp"
#include "States/StateMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

Game::SplashState::SplashState(Game::StateMachine& machine) :
  Game::AbstractState(machine),
  _elapsed(sf::seconds(-1.f))
{
  // Load texture as sprite
  _sprite = sf::Sprite(Game::TextureLibrary::Instance().get(Game::Config::ExecutablePath + "assets/textures/splashscreen.png"));
}

bool  Game::SplashState::update(sf::Time elapsed)
{
  // Add elapsed time to counter
  _elapsed += elapsed;

  // Skip splash screen
  if (_elapsed.asSeconds() > 0.5f && _elapsed.asSeconds() < 4.5f &&
    (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Button::Left) == true ||
      Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Return) == true ||
      Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Space) == true ||
      Game::Window::Instance().joystick().buttonPressed(0, 0) == true ||
      Game::Window::Instance().joystick().buttonPressed(0, 7) == true))
    _elapsed = sf::seconds(4.5f);

  // End splash screen
  if (_elapsed.asSeconds() >= 5.f)
    _machine.swap<Game::StartState>();

  return false;
}

void  Game::SplashState::draw()
{
  sf::RectangleShape  rect;
  sf::Uint8           alpha;

  // Get alpha for fade in/out
  if (_elapsed.asSeconds() < 0.f)
    alpha = 255;
  else if (_elapsed.asSeconds() < 0.5f)
    alpha = sf::Uint8((0.5f - _elapsed.asSeconds()) * 2 * 255);
  else if (_elapsed.asSeconds() > 4.5f)
    alpha = sf::Uint8((_elapsed.asSeconds() - 4.5f) * 2 * 255);
  else
    alpha = 0;

  // Set fade in/out rectange parameters
  rect.setSize(sf::Vector2f((float)Game::Window::Instance().window().getSize().x, (float)Game::Window::Instance().window().getSize().y));
  rect.setFillColor(sf::Color(0, 0, 0, alpha));

  // Position sprite in window
  const auto& texture = Game::TextureLibrary::Instance().get(Game::Config::ExecutablePath + "assets/textures/splashscreen.png");
  float       scale = std::min(1.f, std::min((float)Game::Window::Instance().window().getSize().x / (float)texture.getSize().x, (float)Game::Window::Instance().window().getSize().y / (float)texture.getSize().y));
  float       pos_x = (((float)Game::Window::Instance().window().getSize().x - ((float)texture.getSize().x * scale)) / 2.f);
  float       pos_y = (((float)Game::Window::Instance().window().getSize().y - ((float)texture.getSize().y * scale)) / 2.f);

  _sprite.setScale(sf::Vector2f(scale, scale));
  _sprite.setPosition(sf::Vector2f(pos_x, pos_y));

  // Draw splash screen
  Game::Window::Instance().window().draw(_sprite);
  Game::Window::Instance().window().draw(rect);
}