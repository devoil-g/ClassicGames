#pragma once

#include <filesystem>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System.hpp>

#include "Quiz/Quiz.hpp"
#include "Scenes/AbstractScene.hpp"
#include "Scenes/SceneMachine.hpp"
  
namespace QUIZ
{
  class QuizScene : public Game::AbstractScene
  {
  private:
    QUIZ::Quiz          _quiz;    // Main quiz instance
    Game::SceneMachine  _game;    // Quiz state machine
    bool                _scores;  // Score display
    
  public:
    QuizScene(Game::SceneMachine& machine);
    ~QuizScene() override;

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}