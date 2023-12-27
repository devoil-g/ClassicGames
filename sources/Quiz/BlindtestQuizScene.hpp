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

    QUIZ::Quiz::Blindtest _blindtest; // Question to be played
    sf::Music             _music;     // Music to be played
    sf::Texture           _cover;     // Cover texture
    sf::Texture           _play;      // Play texture
    sf::Texture           _pause;     // Pause texture
    sf::RectangleShape    _bar;       // Timer bar
    State                 _state;     // Current state of blindtest
    int                   _player;    // Index of player answering

    std::unordered_map<int, float>  _cooldown;  // Player cooldown before answer

    void  setPlaying();           // Start playing state
    void  setPending(int player); // Start player answer state
    void  setAnswer();            // Start end state

    void  updatePlaying(float elapsed);
    void  updatePending(float elapsed);
    void  updateAnswer(float elapsed);

    void  drawTimer();
    void  drawTexture(const sf::Texture& texture, float size);

  public:
    BlindtestQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz);
    ~BlindtestQuizScene() = default;

    bool  update(float elapsed) override;  // Update state
    void  draw() override;                    // Draw state
  };
}