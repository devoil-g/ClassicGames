#pragma once

#include "Quiz/Quiz.hpp"
#include "Scenes/AbstractScene.hpp"

namespace QUIZ
{
  class QuestionQuizScene : public Game::AbstractScene
  {
  private:
    QUIZ::Quiz&         _quiz;      // Quiz instance
    std::vector<float>  _cooldowns; // Players cooldowns
    float               _cooldown;  // Waiting time before next answer
    int                 _buzz;      // Buzzing player
    
  public:
    QuestionQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz);
    ~QuestionQuizScene() = default;

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}