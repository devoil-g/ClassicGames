#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Audio/Music.hpp>

#include "Quiz/Quiz.hpp"
#include "Scenes/AbstractScene.hpp"

namespace QUIZ
{
  class ControllerQuizScene : public Game::AbstractScene
  {
  private:
    static const float  TimerStart; // Begining of player selection
    static const float  TimerLimit; // Maximum time of player selection
    static const float  TimerOver;  // Duration of animation after selection

    QUIZ::Quiz&         _quiz;  // Quiz instance
    sf::Music           _music; // Background music, also used as timer
    sf::RectangleShape  _bar;   // Timer bar
    unsigned int        _ping;  // Register ping sound index

    void  updateRegister();   // Register new player
    void  updateUnregister(); // Remove a player
    void  updateAvatar();     // Change avatar of players
    void  updateHost();       // Handle host commands

    void  drawPlayers();  // Draw players
    void  drawTimer();    // Draw timer bar

  public:
    ControllerQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz);
    ~ControllerQuizScene() = default;

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}