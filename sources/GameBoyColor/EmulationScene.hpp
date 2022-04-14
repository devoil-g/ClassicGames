#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "GameBoyColor/GameBoyColor.hpp"
#include "Scenes/AbstractScene.hpp"

namespace GBC
{
  class EmulationScene : public Game::AbstractScene
  {
  private:
    static const sf::Time ForcedExit; // Forced exit time limit

    GBC::GameBoyColor   _gbc;     // Game Boy emulator
    sf::Texture         _texture; // Image in graphic memory
    sf::Sprite          _sprite;  // Display rendered texture
    sf::Time            _fps;     // Timer for FPS control
    sf::Time            _exit;    // Timer of forced exit
    sf::RectangleShape  _bar;     // Forced exit bar

  public:
    EmulationScene(Game::SceneMachine& machine, const std::string& filename);
    ~EmulationScene();

    bool  update(sf::Time elapsed) override;  // TODO: comment
    void  draw() override;                    // TODO: comment
  };
}