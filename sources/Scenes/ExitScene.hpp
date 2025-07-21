#pragma once

#include <SFML/Graphics/RectangleShape.hpp>

#include "Scenes/AbstractScene.hpp"
#include "Scenes/SceneMachine.hpp"
#include "System/Window.hpp"

namespace Game
{
  template<typename Scene, unsigned int ExitTimeout = 1>
  class ExitScene : public Game::AbstractScene
  {
  private:
    float               _elapsed;     // Time elapsed
    Game::SceneMachine  _submachine;  // Sub-scenes

  public:
    template <typename ...Args>
    ExitScene(Game::SceneMachine& machine, Args&& ...args) :
      AbstractScene(machine),
      _elapsed(0.f),
      _submachine()
    {
      _submachine.push<Scene>(args...);
    }

    ~ExitScene() = default;

    bool  update(float elapsed) override  // Update exit state
    {
      // Increase exit timer
      if (Game::Window::Instance().keyboard().keyDown(Game::Window::Key::Escape) == true) {
        _elapsed += elapsed;

        // Exit scene
        if (_elapsed >= (float)ExitTimeout) {
          _machine.pop();
          return false;
        }
      }

      // Reset exit timer
      else
        _elapsed = 0.f;

      // Update sub-scene
      if (_submachine.update(elapsed) == true) {
        _machine.pop();
        return false;
      }

      return false;
    }

    void  draw() override // Draw menu state
    {
      // Draw sub-scene
      _submachine.draw();

      auto& window = Game::Window::Instance();
      sf::RectangleShape  bar;

      // Draw exit bar
      bar.setSize(sf::Vector2f(1.f, 1.f));
      bar.setFillColor(sf::Color::White);
      bar.setScale({ window.getSize().x() * _elapsed / (float)ExitTimeout, 4.f });
      window.draw(bar);
    }
  };
}