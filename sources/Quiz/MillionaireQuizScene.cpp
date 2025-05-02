#include <iostream>

#include "System/Config.hpp"
#include "System/Window.hpp"
#include "Quiz/MillionaireQuizScene.hpp"

QUIZ::MillionaireQuizScene::MillionaireQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz) :
  Game::AbstractScene(machine),
  _quiz(quiz),
  _music(),
  _effect(),
  _player(0),
  _questions(0),
  _state(State::Select)
{
  setSelect();
}

QUIZ::MillionaireQuizScene::~MillionaireQuizScene()
{}

void  QUIZ::MillionaireQuizScene::setSelect()
{
  // Set state
  _state = State::Select;

  // Start music
  if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "intro.ogg") == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  _music.setLooping(false);
  _music.play();

  // Reset selection
  _player = 0;
  _questions = 0;

  // Reset player outline
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));

    entity.setTargetPosition((index + 1.f) / (_quiz.players.size() + 1.f), 0.5f);
    entity.setTargetScale(0.9f / (_quiz.players.size() + 1.f), 0.75f);
    entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
    entity.setLerp(0.0625f);
    entity.setOutline(0.01f);
  }

  // Set selected player outline
  _quiz.entities.at("player_" + std::to_string(_quiz.players[_player].id)).setOutline(0.01f);

  // Host instructions
  std::cout
    << "--- WHO WANT TO BE A MILLIONAIRE ? ---" << std::endl
    << "Select a main player and a question set." << std::endl
    << std::endl
    << "Commands:" << std::endl
    << "  [N]ext:   start game" << std::endl
    << "  [^]rrow:  select previous player" << std::endl
    << "  [v]rrow:  selection next player" << std::endl
    << "  [<]rrow   select previous question set" << std::endl
    << "  [>]rrow   select next question set" << std::endl
    << std::endl;
}

void  QUIZ::MillionaireQuizScene::updateSelect(float elapsed)
{
  // Select player
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Up) == true ||
    Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Down) == true) {
    _quiz.entities.at("player_" + std::to_string(_quiz.players[_player].id)).setOutline(0.f);

    _player = (_player + _quiz.players.size()
      + (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Up) == true ? -1 : 0)
      + (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Down) == true ? +1 : 0))
      % _quiz.players.size();

    _quiz.entities.at("player_" + std::to_string(_quiz.players[_player].id)).setOutline(0.01f);
  }

  // Select question

}

bool  QUIZ::MillionaireQuizScene::update(float elapsed)
{
  switch (_state) {
  case State::Select:
    updateSelect(elapsed); break;
  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
  return false;
}

void  QUIZ::MillionaireQuizScene::draw()
{}