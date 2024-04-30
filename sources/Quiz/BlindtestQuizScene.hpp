#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Audio/Music.hpp>
#include <unordered_map>

#include "Quiz/Quiz.hpp"
#include "Scenes/AbstractScene.hpp"

namespace QUIZ
{
  class BlindtestQuizScene : public Game::AbstractScene
  {
  private:
    enum State
    {
      StatePlaying, // Play music
      StatePending, // Player is answering
      StateAnswer   // Show answer
    };

    QUIZ::Quiz& _quiz;    // Quiz instance

    std::vector<QUIZ::Quiz::Blindtest>::iterator _blindtest; // Question to be played

    sf::Music             _music;     // Music to be played
    sf::RectangleShape    _bar;       // Timer bar
    State                 _state;     // Current state of blindtest
    int                   _buzz;      // Index of player answering

    std::vector<float>  _cooldowns; // Player cooldown before answer
    float               _cooldown;  // Cooldown given to players

    void  start();
    void  next();
    void  previous();

    void  updatePlaying(float elapsed);
    void  updatePending(float elapsed);
    void  updateAnswer(float elapsed);

    void  setPlaying();
    void  setPending();
    void  setAnswer();

  public:
    BlindtestQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz);
    ~BlindtestQuizScene();

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}