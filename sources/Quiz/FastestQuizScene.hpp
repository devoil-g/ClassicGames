#pragma once

#include <vector>
#include <list>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Audio/Music.hpp>

#include "Quiz/Quiz.hpp"
#include "Scenes/AbstractScene.hpp"

namespace QUIZ
{
  class FastestQuizScene : public Game::AbstractScene
  {
  private:
    QUIZ::Quiz& _quiz;  // Quiz instance
    sf::Music   _music; // Background music, also used as timer
    unsigned int  _question;  // Fastest finger question index in quiz

    enum class State {
      Preparation,
      Start,
      Answer,
      Chrono,
      Correct,
      Winner
    } _state;

    float _timer;
    std::vector<std::pair<float, std::list<unsigned int>>>  _answers;

    void  setPreparation();
    void  setStart();
    void  setAnswer();
    void  setChrono();
    void  setCorrect();
    void  setWinner();

    void  updatePreparation(float elapsed);
    void  updateStart(float elapsed);
    void  updateAnswer(float elapsed);
    void  updateChrono(float elapsed);
    void  updateCorrect(float elapsed);
    void  updateWinner(float elapsed);

  public:
    FastestQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz);
    ~FastestQuizScene();

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}