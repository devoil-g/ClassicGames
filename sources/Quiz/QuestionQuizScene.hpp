#pragma once

#include <SFML/Graphics/RectangleShape.hpp>

#include "Quiz/Quiz.hpp"
#include "Scenes/AbstractScene.hpp"

namespace QUIZ
{
  class QuestionQuizScene : public Game::AbstractScene
  {
  private:
    QUIZ::Quiz&         _quiz;      // Quiz instance
    bool                _display;   // Display score
    int                 _score;     // Question score
    int                 _current;   // Current question score
    int                 _buzz;      // Player buzzing
    std::vector<float>  _cooldowns; // Players cooldown
    float               _cooldown;  // Buzz cooldown

    void  usage() const;

  public:
    QuestionQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz);
    ~QuestionQuizScene() = default;

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}