#include "States/LoadingState.hpp"
#include "States/StateMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Library/FontLibrary.hpp"

Game::LoadingState::LoadingState(std::future<Game::AbstractState *> && loader)
  : _loader(std::move(loader)), _text("Loading", Game::FontLibrary::Instance().get(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf"), 1), _elapsed(0.f)
{
  // Set taskbar status to flickering
  Game::Window::Instance().taskbar(Game::Window::WindowFlag::Indeterminate);
}

Game::LoadingState::~LoadingState()
{}

bool	Game::LoadingState::update(sf::Time elapsed)
{
  // Get elasped time
  _elapsed += elapsed.asSeconds();

  // Check if loading task has completed
  if (_loader.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
  {
    Game::AbstractState *	state = _loader.get();

    // Swap current state with loaded one if valid
    if (state != nullptr)
      Game::StateMachine::Instance().swap(state);
    else
      Game::StateMachine::Instance().pop();

    // Reset taskbar status
    Game::Window::Instance().taskbar(Game::Window::WindowFlag::NoProgress);

    return false;
  }

  std::string	text("Loading");

  // Generate loading dots
  for (int n = 0; n < (int)(_elapsed * 2) % 4; n++)
    text += ".";

  // Set loading screen text
  _text.setString(text);

  return false;
}

void	Game::LoadingState::draw()
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
