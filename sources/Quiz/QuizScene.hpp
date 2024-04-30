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
    static const float  ForcedExit; // Forced exit time limit

    QUIZ::Quiz          _quiz;    // Main quiz instance
    Game::SceneMachine  _game;    // Quiz state machine
    bool                _scores;  // Score display
    
    float               _elapsed; // Timer of forced exit
    sf::RectangleShape  _bar;     // Forced exit bar
    
  public:
    QuizScene(Game::SceneMachine& machine);
    ~QuizScene() override;

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}