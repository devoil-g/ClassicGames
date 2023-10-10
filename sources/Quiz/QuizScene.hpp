#pragma once

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
    static const sf::Time ForcedExit; // Forced exit time limit

    QUIZ::Quiz          _quiz;    // Main quiz instance
    Game::SceneMachine  _game;    // Quiz state machine
    
    sf::Time            _elapsed; // Timer of forced exit
    sf::RectangleShape  _bar;     // Forced exit bar
    
  public:
    QuizScene(Game::SceneMachine& machine, const std::string& config);
    ~QuizScene() override;

    bool  update(sf::Time elapsed) override;  // Update state
    void  draw() override;                    // Draw state
  };
}