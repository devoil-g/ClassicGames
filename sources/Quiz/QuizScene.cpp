#include <iostream>

#include "Quiz/QuizScene.hpp"
#include "Quiz/ControllerQuizScene.hpp"
#include "Quiz/ScoresQuizScene.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Audio/Sound.hpp"

const float QUIZ::QuizScene::ForcedExit = 1.f;

QUIZ::QuizScene::QuizScene(Game::SceneMachine& machine) :
  Game::AbstractScene(machine),
  _quiz(),
  _game(),
  _elapsed(0.f),
  _bar(sf::Vector2f(1.f, 1.f))
{
  // Push initial states
  _game.push<QUIZ::ScoresQuizScene>(_quiz);
  _game.push<QUIZ::ControllerQuizScene>(_quiz);

  // Initialize force exit bar
  _bar.setSize(sf::Vector2f(1.f, 1.f));
  _bar.setFillColor(sf::Color::White);
}

QUIZ::QuizScene::~QuizScene()
{
  // Interrupt playing sounds to avoid reading deleted buffers
  Game::Audio::Sound::Instance().clear();
}

bool  QUIZ::QuizScene::update(float elapsed)
{
  // Update exit timer
  _elapsed += elapsed;

  // Reset timer when ESC is not pressed
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Escape) == false)
    _elapsed = 0.f;

  // Exit if limit reached
  if (_elapsed > QUIZ::QuizScene::ForcedExit) {
    _machine.pop();
    return false;
  }

  // Update game
  if (_game.update(elapsed) == true) {
    _game.push<QUIZ::ScoresQuizScene>(_quiz);
    return false;
  }

  // TODO: show scores using TAB

  return false;
}

void  QUIZ::QuizScene::draw()
{
  // Draw quiz
  _game.draw();
  
  // Draw forced exit bar
  _bar.setScale(Game::Window::Instance().window().getSize().x * _elapsed / QUIZ::QuizScene::ForcedExit, 4.f);
  Game::Window::Instance().window().draw(_bar);
}