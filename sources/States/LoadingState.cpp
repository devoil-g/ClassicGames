#include "States/LoadingState.hpp"
#include "States/StateMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Library/FontLibrary.hpp"

Game::LoadingState::LoadingState(Game::StateMachine& machine) :
  Game::AbstractState(machine),
  _text("Loading", Game::FontLibrary::Instance().get(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf"), 1),
  _elapsed(sf::Time::Zero)
{
  // Set taskbar status to flickering
  Game::Window::Instance().taskbar(Game::Window::WindowFlag::Indeterminate);
}

Game::LoadingState::~LoadingState()
{
  // Reset taskbar status
  Game::Window::Instance().taskbar(Game::Window::WindowFlag::NoProgress);
}

bool  Game::LoadingState::update(sf::Time elapsed)
{
  // Get elasped time
  _elapsed += elapsed;

  std::string text("Loading");

  // Generate loading dots
  for (int n = 0; n < (int)(_elapsed.asSeconds() * 2) % 4; n++)
    text += ".";

  // Set loading screen text
  _text.setString(text);

  return false;
}

void  Game::LoadingState::draw()
{
  // Set text size according to window size
  _text.setCharacterSize(std::min(Game::Window::Instance().window().getSize().x, Game::Window::Instance().window().getSize().y) / 16);
  
  // Position text into screen space
  _text.setPosition(
    Game::Window::Instance().window().getSize().x * 1 / 16 - _text.getLocalBounds().left,
    Game::Window::Instance().window().getSize().y * 15 / 16 + _text.getLocalBounds().top - _text.getLocalBounds().height
  );

  // Draw loading text to window
  Game::Window::Instance().window().clear();
  Game::Window::Instance().window().draw(_text);
}
