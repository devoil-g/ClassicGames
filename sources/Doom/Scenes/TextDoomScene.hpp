#pragma once

#include <cstdint>
#include <queue>
#include <string>

#include <SFML/Graphics/Image.hpp>

#include "Doom/Doom.hpp"
#include "Scenes/AbstractScene.hpp"

namespace DOOM
{
  class TextDoomScene : public Game::AbstractScene
  {
  private:
    static const unsigned int TextSpeed;  // Number of tics for each character

    DOOM::Doom&       _doom;    // DOOM instance
    float             _elapsed; // Time elapsed since beginning of state
    sf::Image         _image;   // Rendering target
    std::queue<char>  _text;    // Character to be displayed
    int               _x, _y;   // Text coordinates

    bool  updateSkip(); // Return true if skip button has been pressed

  public:
    TextDoomScene(Game::SceneMachine& machine, DOOM::Doom& doom, const std::string& text, std::uint64_t background);
    ~TextDoomScene() override = default;

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}