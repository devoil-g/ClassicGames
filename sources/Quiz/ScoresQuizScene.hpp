#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Audio/Music.hpp>

#include "Quiz/Quiz.hpp"
#include "Scenes/AbstractScene.hpp"

namespace QUIZ
{
  class ScoresQuizScene : public Game::AbstractScene
  {
  private:
    QUIZ::Quiz&         _quiz;    // Quiz instance
    sf::Music           _music;   // Background music, also used as timer
    bool                _display; // Display score

  public:
    ScoresQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz);
    ~ScoresQuizScene() = default;

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}