#pragma once

#include <SFML/Graphics/RectangleShape.hpp>

#include "Quiz/Quiz.hpp"
#include "Scenes/AbstractScene.hpp"

namespace QUIZ
{
  class QuestionQuizScene : public Game::AbstractScene
  {
  private:
    QUIZ::Quiz&             _quiz;      // Quiz instance
    //QUIZ::Quiz::Question&   _question;  // Current question

  public:
    QuestionQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz/*, QUIZ::Quiz::Question& question*/);
    ~QuestionQuizScene() = default;

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}