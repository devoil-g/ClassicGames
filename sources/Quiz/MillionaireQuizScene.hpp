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
    static const unsigned int QuestionNumber = 15;  // Number of questions

    static const std::array<int, QuestionNumber>  ScoreAll;   // Score given to every player for a correct answer
    static const std::array<int, QuestionNumber>  ScoreWin;   // Score payout of each question
    static const std::array<int, QuestionNumber>  ScoreFail;  // Score in case of failure

    struct Sound {
      std::wstring  path;   // Path to file
      float         offset; // Start of loop point
      float         length; // Length of loop
    };

    static const std::array<std::wstring, QuestionNumber> SoundLight;     // Light at the center
    static const std::array<Sound, QuestionNumber>        SoundQuestion;  // Question background music
    static const std::array<Sound, QuestionNumber>        SoundAnswer;    // Suspence music
    static const std::array<std::wstring, QuestionNumber> SoundCorrect;   // Correct horn
    static const std::array<std::wstring, QuestionNumber> SoundWrong;     // Wrong horn

    QUIZ::Quiz&   _quiz;    // Quiz instance
    sf::Music     _music;   // Background music
    
    bool          _lose;      // True when main player has already lost
    int           _score;     // Score of main player
    unsigned int  _player;    // Index of main player
    unsigned int  _questions; // Index of questions set
    unsigned int  _question;  // Index of current question
    unsigned int  _step;      // Current question
    int           _display;   // Display question step

    bool  _joker5050;
    bool  _jokerPhone;
    bool  _jokerAudience;
    bool  _jokerHost;
    bool  _jokerSwitch;

    std::vector<int>  _answers; // Answers of players

    enum class State
    {
      Select,
      Light,
      Question,
      Answer,
      Result,
      Score
    } _state;


  public:
    MillionaireQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz);
    ~MillionaireQuizScene();
    
    void  setSelect();
    void  setLight(unsigned int step = 0);
    void  setQuestion();
    void  setAnswer();
    void  setResult();
    void  setScore();

    void  updateSelect(float elapsed);
    void  updateLight(float elapsed);
    void  updateQuestion(float elapsed);
    void  updateAnswer(float elapsed);
    void  updateResult(float elapsed);
    void  updateScore(float elapsed);

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}