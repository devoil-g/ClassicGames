#include "Scenes/LoadingScene.hpp"
#include "Scenes/SceneMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Library/FontLibrary.hpp"

Game::LoadingScene::LoadingScene(Game::SceneMachine& machine) :
  Game::AbstractScene(machine),
  _text("", Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "04b03.ttf"), 1),
  _elapsed(0.f)
{
  // Set taskbar status to flickering
  Game::Window::Instance().taskbar(Game::Window::WindowFlag::Indeterminate);
}

Game::LoadingScene::~LoadingScene()
{
  // Reset taskbar status
  Game::Window::Instance().taskbar(Game::Window::WindowFlag::NoProgress);
}

bool  Game::LoadingScene::update(float elapsed)
{
  // Get elasped time
  _elapsed += elapsed;

  std::string text = ".";

  // Generate loading dots
  for (int n = 0; n < (int)(_elapsed * 2) % 3; n++)
    text += ".";

  // Set loading screen text
  _text.setString(text);

  return false;
}

void  Game::LoadingScene::draw()
{
  // Set text size according to window size
  _text.setCharacterSize(std::min(Game::Window::Instance().window().getSize().x, Game::Window::Instance().window().getSize().y) / 16);
  
  // Position text into screen space
  _text.setPosition(
    Game::Window::Instance().window().getSize().x * 1 / 16 - _text.getLocalBounds().left,
    Game::Window::Instance().window().getSize().y * 15 / 16 - _text.getLocalBounds().top - _text.getLocalBounds().height
  );

  // Draw loading text to window
  Game::Window::Instance().window().clear();
  Game::Window::Instance().window().draw(_text);
}
