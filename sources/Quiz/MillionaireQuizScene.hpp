#pragma once

#include <vector>
#include <list>

#include <SFML/Audio/Music.hpp>

#include "Quiz/Quiz.hpp"
#include "Scenes/AbstractScene.hpp"

namespace QUIZ
{
  class MillionaireQuizScene : public Game::AbstractScene
  {
  private:
    QUIZ::Quiz&   _quiz;    // Quiz instance
    sf::Music     _music;   // Background music, also used as timer
    sf::Music     _effect;  // Sound effects (buzz)
    
    unsigned int  _player;    // Index of main player
    unsigned int  _questions; // Index of questions set

    enum class State
    {
      Select
    } _state;


  public:
    MillionaireQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz);
    ~MillionaireQuizScene();
    
    void  setSelect();

    void  updateSelect(float elapsed);

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}