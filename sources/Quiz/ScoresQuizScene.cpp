#include <iostream>

#include "Quiz/ScoresQuizScene.hpp"
#include "Quiz/BlindtestQuizScene.hpp"
#include "Quiz/ControllerQuizScene.hpp"
#include "Quiz/FastestQuizScene.hpp"
#include "Quiz/QuestionQuizScene.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Audio/Sound.hpp"
#include "System/Library/FontLibrary.hpp"

QUIZ::ScoresQuizScene::ScoresQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz) :
  Game::AbstractScene(machine),
  _quiz(quiz)
{
  // Set player visible
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));

    entity.setTargetPosition((index + 1.f) / (_quiz.players.size() + 1.f), 0.5f);
    entity.setTargetScale(0.9f / (_quiz.players.size() + 1.f), 0.75f);
    entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
    entity.setLerp(0.0625f);
  }

  static sf::Music music;
  
  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "2000_question.ogg");
  music.setLoopPoints({ .offset = sf::seconds(4.000f), .length = sf::seconds(32.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "2000_answer.ogg");
  music.setLoopPoints({ .offset = sf::seconds(8.310f), .length = sf::seconds(22.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "4000_question.ogg");
  music.setLoopPoints({ .offset = sf::seconds(4.000f), .length = sf::seconds(32.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "4000_answer.ogg");
  music.setLoopPoints({ .offset = sf::seconds(8.310f), .length = sf::seconds(22.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "8000_question.ogg");
  music.setLoopPoints({ .offset = sf::seconds(4.000f), .length = sf::seconds(32.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "8000_answer.ogg");
  music.setLoopPoints({ .offset = sf::seconds(8.310f), .length = sf::seconds(22.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "16000_question.ogg");
  music.setLoopPoints({ .offset = sf::seconds(4.000f), .length = sf::seconds(32.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "16000_answer.ogg");
  music.setLoopPoints({ .offset = sf::seconds(8.310f), .length = sf::seconds(22.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "32000_question.ogg");
  music.setLoopPoints({ .offset = sf::seconds(4.000f), .length = sf::seconds(32.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "32000_answer.ogg");
  music.setLoopPoints({ .offset = sf::seconds(8.310f), .length = sf::seconds(22.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "64000_question.ogg");
  music.setLoopPoints({ .offset = sf::seconds(4.000f), .length = sf::seconds(32.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "64000_answer.ogg");
  music.setLoopPoints({ .offset = sf::seconds(8.310f), .length = sf::seconds(22.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "125000_question.ogg");
  music.setLoopPoints({ .offset = sf::seconds(4.000f), .length = sf::seconds(32.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "125000_answer.ogg");
  music.setLoopPoints({ .offset = sf::seconds(4.000f), .length = sf::seconds(32.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "250000_question.ogg");
  music.setLoopPoints({ .offset = sf::seconds(4.000f), .length = sf::seconds(32.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "250000_answer.ogg");
  music.setLoopPoints({ .offset = sf::seconds(6.000f), .length = sf::seconds(30.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "500000_question.ogg");
  music.setLoopPoints({ .offset = sf::seconds(4.000f), .length = sf::seconds(32.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "500000_answer.ogg");
  music.setLoopPoints({ .offset = sf::seconds(6.000f), .length = sf::seconds(30.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "1000000_question.ogg");
  music.setLoopPoints({ .offset = sf::seconds(4.000f), .length = sf::seconds(32.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "1000000_answer.ogg");
  music.setLoopPoints({ .offset = sf::seconds(6.000f), .length = sf::seconds(30.000f) });

  music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "1000_question.ogg");
  music.setLoopPoints({ .offset = sf::seconds(13.919f), .length = sf::seconds(13.919f) });

  music.setLooping(true);
  //music.play();

  // Host instructions
  std::cout
    << "--- SCOREBOARD ---" << std::endl
    << "Instruction for players: nothing" << std::endl
    << std::endl
    << "Commands:" << std::endl
    << "  [C]ontroller: controller selection" << std::endl
    << "  [F]astest:    fastest finger" << std::endl
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
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::C) == true) {
    _machine.swap<QUIZ::ControllerQuizScene>(_quiz);
    return false;
  }

  // Fastest finger
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::F) == true) {
    _machine.swap<QUIZ::FastestQuizScene>(_quiz);
    return false;
  }

  // Free question
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Q) == true) {
    _machine.swap<QUIZ::QuestionQuizScene>(_quiz);
    return false;
  }

  // Start blindtest
  if (_quiz.blindtests.empty() == false && Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::B) == true) {
    _machine.swap<QUIZ::BlindtestQuizScene>(_quiz);
    return false;
  }

  return false;
}

void  QUIZ::ScoresQuizScene::draw()
{}