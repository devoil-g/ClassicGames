#pragma once

#include <queue>

#include "Doom/Doom.hpp"
#include "States/AbstractState.hpp"
  
namespace DOOM
{
  class TextDoomState : public Game::AbstractState
  {
  private:
    static const unsigned int TextSpeed;  // Number of tics for each character

    DOOM::Doom&       _doom;    // DOOM instance
    sf::Time          _elapsed; // Time elapsed since beginning of state
    sf::Image         _image;   // Rendering target
    std::queue<char>  _text;    // Character to be displayed
    int               _x, _y;   // Text coordinates

    bool  updateSkip(); // Return true if skip button has been pressed

  public:
    TextDoomState(Game::StateMachine& machine, DOOM::Doom& doom, const std::string& text, uint64_t background);
    ~TextDoomState() override = default;

    bool  update(sf::Time elapsed) override;  // Update state
    void  draw() override;                    // Draw state
  };
}