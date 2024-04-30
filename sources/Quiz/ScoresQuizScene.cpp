#include <SFML/Graphics/Text.hpp>

#include "Quiz/ScoresQuizScene.hpp"
#include "Quiz/BlindtestQuizScene.hpp"
#include "Quiz/ControllerQuizScene.hpp"
#include "Quiz/QuestionQuizScene.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Audio/Sound.hpp"
#include "System/Library/FontLibrary.hpp"

#include <iostream>

QUIZ::ScoresQuizScene::ScoresQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz) :
  Game::AbstractScene(machine),
  _quiz(quiz),
  _music()
{
  // Load music
  if (_music.openFromFile((Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "scores.ogg").string()) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Set music parameters
  _music.setLoop(true);
  _music.setVolume(25.f);
  _music.play();

  // Set player visible
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));

    entity.setTargetPosition((index + 1.f) / (_quiz.players.size() + 1.f), 0.5f);
    entity.setTargetScale(0.9f / (_quiz.players.size() + 1.f), 0.75f);
    entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
    entity.setLerp(0.0625f);
  }

  // Host instructions
  std::cout
    << "--- SCOREBOARD ---" << std::endl
    << "Instruction for players: nothing" << std::endl
    << std::endl
    << "Commands:" << std::endl
    << "  [C]ontroller: controller selection" << std::endl
    << "  [Q]uestion:   free questions" << std::endl;
  if (_quiz.blindtests.empty() == false)
    std::cout
    << "  [B]lintest:   play blindtest (" << std::count_if(_quiz.blindtests.begin(), _quiz.blindtests.end(), [](const auto& entry) { return entry.done == false; }) << " remaining)" << std::endl;
  std::cout
    << std::endl;
}

bool  QUIZ::ScoresQuizScene::update(float elapsed)
{
  // Controller selection
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::C) == true) {
    _machine.swap<QUIZ::ControllerQuizScene>(_quiz);
    return false;
  }

  // Free question
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Q) == true) {
    _machine.swap<QUIZ::QuestionQuizScene>(_quiz);
    return false;
  }

  // Start blindtest
  if (_quiz.blindtests.empty() == false && Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::B) == true) {
    _machine.swap<QUIZ::BlindtestQuizScene>(_quiz);
    return false;
  }

  return false;
}

void  QUIZ::ScoresQuizScene::draw()
{}