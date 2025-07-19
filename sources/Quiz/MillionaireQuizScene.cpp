#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>

#include "System/Config.hpp"
#include "System/Utilities.hpp"
#include "System/Window.hpp"
#include "System/Audio/Sound.hpp"
#include "Quiz/MillionaireQuizScene.hpp"

const std::array<int, QUIZ::MillionaireQuizScene::QuestionNumber> QUIZ::MillionaireQuizScene::ScoreAll =
{ 100, 100, 100, 100, 200, 1000, 1000, 1000, 1000, 2000, 32000, 32000, 32000, 32000, 64000 };
const std::array<int, QUIZ::MillionaireQuizScene::QuestionNumber> QUIZ::MillionaireQuizScene::ScoreWin =
{ 100, 200, 300, 500, 1000, 2000, 4000, 8000, 16000, 32000, 64000, 125000, 250000, 500000, 1000000 };
const std::array<int, QUIZ::MillionaireQuizScene::QuestionNumber> QUIZ::MillionaireQuizScene::ScoreFail =
{ 0, 0, 0, 0, 0, 1000, 1000, 1000, 1000, 1000, 32000, 32000, 32000, 32000, 32000 };

const std::array<std::wstring, QUIZ::MillionaireQuizScene::QuestionNumber>  QUIZ::MillionaireQuizScene::SoundLight = {
  L"1000_lights.ogg",
  L"",
  L"",
  L"",
  L"",
  L"2000_lights.ogg",
  L"4000_lights.ogg",
  L"8000_lights.ogg",
  L"16000_lights.ogg",
  L"32000_lights.ogg",
  L"64000_lights.ogg",
  L"125000_lights.ogg",
  L"250000_lights.ogg",
  L"500000_lights.ogg",
  L"1000000_lights.ogg"
};

const std::array<QUIZ::MillionaireQuizScene::Sound, QUIZ::MillionaireQuizScene::QuestionNumber> QUIZ::MillionaireQuizScene::SoundQuestion = {
  Sound{ .path = L"1000_question.ogg", .offset = 13.919f, .length = 13.919f },
  Sound{ .path = L"1000_question.ogg", .offset = 13.919f, .length = 13.919f },
  Sound{ .path = L"1000_question.ogg", .offset = 13.919f, .length = 13.919f },
  Sound{ .path = L"1000_question.ogg", .offset = 13.919f, .length = 13.919f },
  Sound{ .path = L"1000_question.ogg", .offset = 13.919f, .length = 13.919f },
  Sound{ .path = L"2000_question.ogg", .offset = 4.f, .length = 32.f },
  Sound{ .path = L"4000_question.ogg", .offset = 4.f, .length = 32.f },
  Sound{ .path = L"8000_question.ogg", .offset = 4.f, .length = 32.f },
  Sound{ .path = L"16000_question.ogg", .offset = 4.f, .length = 32.f },
  Sound{ .path = L"32000_question.ogg", .offset = 4.f, .length = 32.f },
  Sound{ .path = L"64000_question.ogg", .offset = 4.f, .length = 32.f },
  Sound{ .path = L"125000_question.ogg", .offset = 4.f, .length = 32.f },
  Sound{ .path = L"250000_question.ogg", .offset = 4.f, .length = 32.f },
  Sound{ .path = L"500000_question.ogg", .offset = 4.f, .length = 32.f },
  Sound{ .path = L"1000000_question.ogg", .offset = 4.f, .length = 32.f },
};

const std::array<QUIZ::MillionaireQuizScene::Sound, QUIZ::MillionaireQuizScene::QuestionNumber> QUIZ::MillionaireQuizScene::SoundAnswer = {
  Sound{ .path = L"", .offset = 0.f, .length = 0.f },
  Sound{ .path = L"", .offset = 0.f, .length = 0.f },
  Sound{ .path = L"", .offset = 0.f, .length = 0.f },
  Sound{ .path = L"", .offset = 0.f, .length = 0.f },
  Sound{ .path = L"", .offset = 0.f, .length = 0.f },
  Sound{ .path = L"2000_answer.ogg", .offset = 8.31f, .length = 15.f },
  Sound{ .path = L"4000_answer.ogg", .offset = 8.31f, .length = 15.f },
  Sound{ .path = L"8000_answer.ogg", .offset = 8.31f, .length = 15.f },
  Sound{ .path = L"16000_answer.ogg", .offset = 8.31f, .length = 15.f },
  Sound{ .path = L"32000_answer.ogg", .offset = 8.31f, .length = 15.f },
  Sound{ .path = L"64000_answer.ogg", .offset = 8.31f, .length = 15.f },
  Sound{ .path = L"125000_answer.ogg", .offset = 8.31f, .length = 15.f },
  Sound{ .path = L"250000_answer.ogg", .offset = 8.31f, .length = 15.f },
  Sound{ .path = L"500000_answer.ogg", .offset = 8.31f, .length = 15.f },
  Sound{ .path = L"1000000_answer.ogg", .offset = 8.31f, .length = 15.f },
};

const std::array<std::wstring, QUIZ::MillionaireQuizScene::QuestionNumber>  QUIZ::MillionaireQuizScene::SoundCorrect = {
  L"fastest_correct.ogg",
  L"fastest_correct.ogg",
  L"fastest_correct.ogg",
  L"fastest_correct.ogg",
  L"1000_correct.ogg",
  L"2000_correct.ogg",
  L"4000_correct.ogg",
  L"8000_correct.ogg",
  L"16000_correct.ogg",
  L"32000_correct.ogg",
  L"64000_correct.ogg",
  L"125000_correct.ogg",
  L"250000_correct.ogg",
  L"500000_correct.ogg",
  L"1000000_correct.ogg"
};

const std::array<std::wstring, QUIZ::MillionaireQuizScene::QuestionNumber>  QUIZ::MillionaireQuizScene::SoundWrong = {
  L"1000_wrong.ogg",
  L"1000_wrong.ogg",
  L"1000_wrong.ogg",
  L"1000_wrong.ogg",
  L"1000_wrong.ogg",
  L"2000_wrong.ogg",
  L"4000_wrong.ogg",
  L"8000_wrong.ogg",
  L"16000_wrong.ogg",
  L"32000_wrong.ogg",
  L"64000_wrong.ogg",
  L"125000_wrong.ogg",
  L"250000_wrong.ogg",
  L"500000_wrong.ogg",
  L"1000000_wrong.ogg"
};

QUIZ::MillionaireQuizScene::MillionaireQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz) :
  Game::AbstractScene(machine),
  _quiz(quiz),
  _music(),
  _player(0),
  _questions(0),
  _state(State::Select)
{
  // No player
  if (_quiz.players.size() < 0  || _quiz.millionaires.size() == 0)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  
  // Set player and question set
  setSelect();
}

QUIZ::MillionaireQuizScene::~MillionaireQuizScene()
{
  // Remove jokers
  _quiz.entities["joker_5050"].setDead(true);
  _quiz.entities["joker_5050"].setTargetColor(1.f, 1.f, 1.f, 0.f);
  _quiz.entities["joker_phone"].setDead(true);
  _quiz.entities["joker_phone"].setTargetColor(1.f, 1.f, 1.f, 0.f);
  _quiz.entities["joker_audience"].setDead(true);
  _quiz.entities["joker_audience"].setTargetColor(1.f, 1.f, 1.f, 0.f);
  _quiz.entities["joker_host"].setDead(true);
  _quiz.entities["joker_host"].setTargetColor(1.f, 1.f, 1.f, 0.f);
  _quiz.entities["joker_switch"].setDead(true);
  _quiz.entities["joker_switch"].setTargetColor(1.f, 1.f, 1.f, 0.f);

  _quiz.entities["audience_0"].setDead(true);
  _quiz.entities["audience_0"].setTargetColor(1.f, 1.f, 1.f, 0.f);
  _quiz.entities["audience_1"].setDead(true);
  _quiz.entities["audience_1"].setTargetColor(1.f, 1.f, 1.f, 0.f);
  _quiz.entities["audience_2"].setDead(true);
  _quiz.entities["audience_2"].setTargetColor(1.f, 1.f, 1.f, 0.f);
  _quiz.entities["audience_3"].setDead(true);
  _quiz.entities["audience_3"].setTargetColor(1.f, 1.f, 1.f, 0.f);

  // Remove questions
  _quiz.entities["question"].setDead(true);
  _quiz.entities["question"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.entities["answer_0"].setDead(true);
  _quiz.entities["answer_0"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.entities["answer_1"].setDead(true);
  _quiz.entities["answer_1"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.entities["answer_2"].setDead(true);
  _quiz.entities["answer_2"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.entities["answer_3"].setDead(true);
  _quiz.entities["answer_3"].setTargetColor(0.f, 0.f, 0.f, 0.f);

  // Scores
  _quiz.entities["score"].setDead(true);
  _quiz.entities["score"].setTargetColor(1.f, 1.f, 1.f, 0.f);
  _quiz.entities["final"].setDead(true);
  _quiz.entities["final"].setTargetColor(1.f, 1.f, 1.f, 0.f);
}

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
  _lose = false;

  // Reset player color
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));

    entity.setTargetPosition((index + 1.f) / (_quiz.players.size() + 1.f), 0.5f);
    entity.setTargetScale(0.9f / (_quiz.players.size() + 1.f), 0.75f);
    entity.setTargetColor(1.f, 1.f, 1.f, 0.5f);
    entity.setLerp(0.125f);
  }

  // Set up jokers
  _joker5050 = true;
  _jokerAudience = true;
  _jokerPhone = true;
  _jokerHost = true;
  _jokerSwitch = true;
  auto& joker_5050 = _quiz.entities["joker_5050"];
  joker_5050.reset();
  joker_5050.setTexture(Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "joker_5050.png");
  joker_5050.setColor(1.f, 1.f, 1.f, 0.f);
  joker_5050.setPosition(0.9f, 0.1f);
  joker_5050.setScale(0.18f, 0.18f);
  joker_5050.setLerp(0.125f);
  auto& joker_audience = _quiz.entities["joker_audience"];
  joker_audience.reset();
  joker_audience.setTexture(Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "joker_audience.png");
  joker_audience.setColor(1.f, 1.f, 1.f, 0.f);
  joker_audience.setPosition(0.9f, 0.3f);
  joker_audience.setScale(0.18f, 0.18f);
  joker_audience.setLerp(0.125f);
  auto& joker_phone = _quiz.entities["joker_phone"];
  joker_phone.reset();
  joker_phone.setTexture(Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "joker_phone.png");
  joker_phone.setColor(1.f, 1.f, 1.f, 0.f);
  joker_phone.setPosition(0.9f, 0.5f);
  joker_phone.setScale(0.18f, 0.18f);
  joker_phone.setLerp(0.125f);
  auto& joker_host = _quiz.entities["joker_host"];
  joker_host.reset();
  joker_host.setTexture(Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "joker_host.png");
  joker_host.setColor(1.f, 1.f, 1.f, 0.f);
  joker_host.setPosition(0.9f, 0.7f);
  joker_host.setScale(0.18f, 0.18f);
  joker_host.setLerp(0.125f);
  auto& joker_switch = _quiz.entities["joker_switch"];
  joker_switch.reset();
  joker_switch.setTexture(Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "joker_switch.png");
  joker_switch.setColor(1.f, 1.f, 1.f, 0.f);
  joker_switch.setPosition(0.9f, 0.9f);
  joker_switch.setScale(0.18f, 0.18f);
  joker_switch.setLerp(0.125f);

  auto& audience_0 = _quiz.entities["audience_0"];
  audience_0.reset();
  audience_0.setColor(1.f, 1.f, 1.f, 0.f);
  audience_0.setPosition(0.3f, 0.34375f);
  audience_0.setScale(0.05f, 0.15f);
  audience_0.setLerp(0.125f);
  auto& audience_1 = _quiz.entities["audience_1"];
  audience_1.reset();
  audience_1.setColor(1.f, 1.f, 1.f, 0.f);
  audience_1.setPosition(0.3f, 0.53125f);
  audience_1.setScale(0.05f, 0.15f);
  audience_1.setLerp(0.125f);
  auto& audience_2 = _quiz.entities["audience_2"];
  audience_2.reset();
  audience_2.setColor(1.f, 1.f, 1.f, 0.f);
  audience_2.setPosition(0.3f, 0.71875f);
  audience_2.setScale(0.05f, 0.15f);
  audience_2.setLerp(0.125f);
  auto& audience_3 = _quiz.entities["audience_3"];
  audience_3.reset();
  audience_3.setColor(1.f, 1.f, 1.f, 0.f);
  audience_3.setPosition(0.3f, 0.90625f);
  audience_3.setScale(0.05f, 0.15f);
  audience_3.setLerp(0.125f);

  // Setup question box
  auto& question = _quiz.entities["question"];
  question.reset();
  question.setScale(0.58f, 0.20f);
  question.setPosition(0.5f, 0.125f - 0.5f);
  question.setOutline(0.01f);
  question.setColor(1.f, 1.f, 1.f, 1.f);
  question.setLerp(0.125f);
  auto& answer_0 = _quiz.entities["answer_0"];
  answer_0.reset();
  answer_0.setScale(0.5f, 0.15f);
  answer_0.setPosition(0.5f, 0.34375f + 1.f);
  answer_0.setOutline(0.01f);
  answer_0.setColor(0x51 / 255.f, 0x8a / 255.f, 0xc1 / 255.f, 1.f);
  answer_0.setLerp(0.125f);
  auto& answer_1 = _quiz.entities["answer_1"];
  answer_1.reset();
  answer_1.setScale(0.5f, 0.15f);
  answer_1.setPosition(0.5f, 0.53125f + 1.f);
  answer_1.setOutline(0.01f);
  answer_1.setColor(0xf1 / 255.f, 0x49 / 255.f, 0x02 / 255.f, 1.f);
  answer_1.setLerp(0.125f);
  auto& answer_2 = _quiz.entities["answer_2"];
  answer_2.reset();
  answer_2.setScale(0.5f, 0.15f);
  answer_2.setPosition(0.5f, 0.71875f + 1.f);
  answer_2.setOutline(0.01f);
  answer_2.setColor(0x3d / 255.f, 0x9f / 255.f, 0x05 / 255.f, 1.f);
  answer_2.setLerp(0.125f);
  auto& answer_3 = _quiz.entities["answer_3"];
  answer_3.reset();
  answer_3.setScale(0.5f, 0.15f);
  answer_3.setPosition(0.5f, 0.90625f + 1.f);
  answer_3.setOutline(0.01f);
  answer_3.setColor(0xeb / 255.f, 0xde / 255.f, 0x0a / 255.f, 1.f);
  answer_3.setLerp(0.125f);

  auto& score = _quiz.entities["score"];
  score.reset();
  score.setScale(0.18f, 0.1f);
  score.setPosition(0.1f, 0.075f);
  score.setOutline(0.01f);
  score.setColor(1.f, 1.f, 1.f, 0.f);
  score.setLerp(0.125f);

  auto& final = _quiz.entities["final"];
  final.reset();
  final.setScale(0.2f, 0.2f);
  final.setPosition(0.5f, 0.625f);
  final.setOutline(0.01f);
  final.setColor(1.f, 1.f, 1.f, 0.f);
  final.setLerp(0.125f);

  // Host instructions
  Game::Utilities::Clear();
  std::cout << std::endl
    << "--- WHO WANT TO BE A MILLIONAIRE ? ---" << std::endl
    << "Select a main player and a question set." << std::endl
    << std::endl
    << "Commands:" << std::endl
    << "  [N]ext:   start game" << std::endl
    << "  [<]rrow:  select previous player" << std::endl
    << "  [>]rrow:  selection next player" << std::endl
    << "  [^]rrow   select previous question set" << std::endl
    << "  [v]rrow   select next question set" << std::endl
    << std::endl;

  std::wcout << "Question set " << _questions << ": '" << _quiz.millionaires.at(_questions).name << "'" << (_quiz.millionaires.at(_questions).done ? " (done)" : "") << ".                \r" << std::flush;
}

void  QUIZ::MillionaireQuizScene::updateSelect(float elapsed)
{
  // Select player
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Left) == true ||
    Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Right) == true) {
    _player = (_player + _quiz.players.size()
      + (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Left) == true ? -1 : 0)
      + (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Right) == true ? +1 : 0))
      % _quiz.players.size();
  }

  // Set player color
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));

    entity.setTargetColor(1.f, 1.f, 1.f, index == _player ? 1.f : 0.5f);
  }

  // Select question set
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Up) == true ||
    Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Down) == true) {
    _questions = (_questions + _quiz.millionaires.size()
      + (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Up) == true ? -1 : 0)
      + (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Down) == true ? +1 : 0))
      % _quiz.millionaires.size();

    std::wcout << "Question set " << _questions << ": '" << _quiz.millionaires.at(_questions).name << "'" << (_quiz.millionaires.at(_questions).done ? " (done)" : "") << ".                \r" << std::flush;
  }

  // Start quiz
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::N) == true)
    setLight();
}

void  QUIZ::MillionaireQuizScene::setLight(unsigned int step)
{
  // State state
  _state = State::Light;

  // Set new step
  _step = step;

  int position = 0;

  // Set player visible
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));

    // Main player
    if (index == _player) {
      entity.setTargetPosition(0.1f, _quiz.players.size() == 1 ? 0.575f : 0.475f);
      entity.setTargetScale(0.18f, _quiz.players.size() == 1 ? 0.8f : 0.6f);
      entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
      entity.setOutline(0.f);
    }

    // Other players
    else {
      entity.setTargetPosition(0.2f * (position + 0.5f) / (_quiz.players.size() - 1), 0.9f);
      entity.setTargetScale(0.2f / (_quiz.players.size() - 1.f) * 0.95f, 0.18f);
      entity.setTargetColor(1.f, 1.f, 1.f, 0.5f);
      entity.setOutline(0.f);
      position++;
    }
  }

  // Show jokers
  _quiz.entities["joker_5050"].setTargetColor(1.f, 1.f, 1.f, _joker5050 == true ? 1.f : 0.5f);
  _quiz.entities["joker_phone"].setTargetColor(1.f, 1.f, 1.f, _jokerPhone == true ? 1.f : 0.5f);
  _quiz.entities["joker_audience"].setTargetColor(1.f, 1.f, 1.f, _jokerAudience == true ? 1.f : 0.5f);
  _quiz.entities["joker_host"].setTargetColor(1.f, 1.f, 1.f, _jokerHost == true ? 1.f : 0.5f);
  _quiz.entities["joker_switch"].setTargetColor(1.f, 1.f, 1.f, _jokerSwitch == true ? 1.f : 0.5f);

  // Hide question boxes
  _quiz.entities["question"].setTargetPosition(0.5f, 0.125f - 0.5f);
  _quiz.entities["question"].setTargetColor(1.f, 1.f, 1.f, 1.f);
  _quiz.entities["answer_0"].setTargetPosition(0.5f, 0.34375f + 1.f);
  _quiz.entities["answer_0"].setTargetColor(0x51 / 255.f, 0x8a / 255.f, 0xc1 / 255.f, 1.f);
  _quiz.entities["answer_1"].setTargetPosition(0.5f, 0.53125f + 1.f);
  _quiz.entities["answer_1"].setTargetColor(0xf1 / 255.f, 0x49 / 255.f, 0x02 / 255.f, 1.f);
  _quiz.entities["answer_2"].setTargetPosition(0.5f, 0.71875f + 1.f);
  _quiz.entities["answer_2"].setTargetColor(0x3d / 255.f, 0x9f / 255.f, 0x05 / 255.f, 1.f);
  _quiz.entities["answer_3"].setTargetPosition(0.5f, 0.90625f + 1.f);
  _quiz.entities["answer_3"].setTargetColor(0xeb / 255.f, 0xde / 255.f, 0x0a / 255.f, 1.f);

  _quiz.entities["audience_0"].setTargetColor(1.f, 1.f, 1.f, 0.f);
  _quiz.entities["audience_1"].setTargetColor(1.f, 1.f, 1.f, 0.f);
  _quiz.entities["audience_2"].setTargetColor(1.f, 1.f, 1.f, 0.f);
  _quiz.entities["audience_3"].setTargetColor(1.f, 1.f, 1.f, 0.f);

  // Show score
  auto& score = _quiz.entities["score"];
  score.setTargetScale(0.18f, 0.1f);
  score.setTargetPosition(0.1f, 0.075f);
  score.setText(std::to_wstring(_step + 1) + L" / " + std::to_wstring(ScoreWin.size()) + L" : " + std::to_wstring(ScoreWin.at(_step)));
  if (_lose == false)
    score.setTargetColor(1.f, 1.f, 1.f, 1.f);
  else
    score.setTargetColor(1.f, 1.f, 1.f, 0.5f);

  // Hide final score
  auto& final = _quiz.entities["final"];
  final.setTargetScale(0.2f, 0.2f);
  final.setTargetColor(1.f, 1.f, 1.f, 0.f);

  // Play light sound
  if (SoundLight.at(_step).empty() == false) {
    auto sound = Game::Audio::Sound::Instance().get();

    if (sound.buffer.loadFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / SoundLight.at(_step)) == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    sound.sound.play();
  }

  // Skip to question
  else {
    setQuestion();
    return;
  }

  // Host instructions
  Game::Utilities::Clear();
  std::cout << std::endl
    << "--- WHO WANT TO BE A MILLIONAIRE ? ---" << std::endl
    << "Lights at the center, ready to start next question." << std::endl
    << std::endl
    << "Commands:" << std::endl
    << "  [N]ext:   start question" << std::endl;
}

void  QUIZ::MillionaireQuizScene::updateLight(float elapsed)
{
  // Start question
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::N) == true)
    setQuestion();
}

void  QUIZ::MillionaireQuizScene::setQuestion()
{
  // State state
  _state = State::Question;

  // Reset display counter
  _display = 0;

  // Reset players answers
  _answers = std::vector<int>(_quiz.players.size(), -1);

  std::vector<unsigned int> available;

  // Select random question
  for (unsigned int index = 0; index < _quiz.millionaires.at(_questions).questions.size(); index++)
  {
    auto& question = _quiz.millionaires.at(_questions).questions.at(index);

    if (question.set.contains(_step) == true && question.done == false)
      available.push_back(index);
  }
  if (available.empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  _question = available.at(std::rand() % available.size());

  // Start question music
  if (_step == 0 || _step > 4 || _music.getStatus() != sf::SoundSource::Status::Playing) {
    if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / SoundQuestion.at(_step).path) == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    _music.setLoopPoints({ .offset = sf::seconds(SoundQuestion.at(_step).offset), .length = sf::seconds(SoundQuestion.at(_step).length) });
    _music.setLooping(true);
    _music.play();
  }

  // Host instructions
  Game::Utilities::Clear();
  std::cout << std::endl
    << "--- WHO WANT TO BE A MILLIONAIRE ? ---" << std::endl
    << "Display question and answers, then players select their answer using their buzzer." << std::endl
    << std::endl;
  
  // Display question
  auto& question = _quiz.millionaires.at(_questions).questions.at(_question);
  if (_lose == false)
    std::cout << "Points: " << ScoreWin.at(_step) << " at win, " << ScoreFail.at(_step) << " at lose, " << ScoreAll.at(_step) << " for correct answers." << std::endl;
  else
    std::cout << "Points: " << ScoreAll.at(_step) << " for correct answers." << std::endl;
  std::cout << std::endl;
  std::wcout << "Question " << (_step + 1) << " / " << QuestionNumber << ": "<< question.question << std::endl
    << "  A: " << question.answers.at(0) << std::endl
    << "  B: " << question.answers.at(1) << std::endl
    << "  C: " << question.answers.at(2) << std::endl
    << "  D: " << question.answers.at(3) << std::endl
    << std::endl;

  // Commands
  std::cout
    << "Commands:" << std::endl
    << "  [N]ext:   next display or lock answer" << std::endl
    << "  [1]:      joker 50/50" << std::endl
    << "  [2]:      joker audience" << std::endl
    << "  [3]:      joker phone" << std::endl
    << "  [4]:      joker host" << std::endl
    << "  [5]:      joker switch" << std::endl
    << "  [Shift]:  force reuse of a joker" << std::endl
    << std::endl;

  std::cout << "Q:[ ] A:[ ] B:[ ] C:[ ] D:[ ]\r" << std::flush;

  // Mark question as done
  _quiz.millionaires.at(_questions).done = true;
  _quiz.millionaires.at(_questions).questions.at(_question).done = true;

}

void  QUIZ::MillionaireQuizScene::updateQuestion(float elapsed)
{
  // Next display
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::N) == true) {
    switch (_display) {
    case 0: // Display question
      _display++;
      std::cout << "Q:[X] A:[ ] B:[ ] C:[ ] D:[ ]\r" << std::flush;
      _quiz.millionaires.at(_questions).questions.at(_question).done = true;
      _quiz.millionaires.at(_questions).done = true;
      _quiz.entities["question"].setText(_quiz.millionaires.at(_questions).questions.at(_question).question);
      _quiz.entities["question"].setTargetPosition(0.5f, 0.125f);
      break;
    case 1: // Display answer A
      _display++;
      std::cout << "Q:[X] A:[X] B:[ ] C:[ ] D:[ ]\r" << std::flush;
      _quiz.entities["answer_0"].setText(_quiz.millionaires.at(_questions).questions.at(_question).answers.at(0));
      _quiz.entities["answer_0"].setTargetPosition(0.5f, 0.34375f);
      break;
    case 2: // Display answer B
      _display++;
      std::cout << "Q:[X] A:[X] B:[X] C:[ ] D:[ ]\r" << std::flush;
      _quiz.entities["answer_1"].setText(_quiz.millionaires.at(_questions).questions.at(_question).answers.at(1));
      _quiz.entities["answer_1"].setTargetPosition(0.5f, 0.53125f);
      break;
    case 3: // Display answer C
      _display++;
      std::cout << "Q:[X] A:[X] B:[X] C:[X] D:[ ]\r" << std::flush;
      _quiz.entities["answer_2"].setText(_quiz.millionaires.at(_questions).questions.at(_question).answers.at(2));
      _quiz.entities["answer_2"].setTargetPosition(0.5f, 0.71875f);
      break;
    case 4: // Display answer D
      _display++;
      std::cout << "Q:[X] A:[X] B:[X] C:[X] D:[X]" << std::endl;
      _quiz.entities["answer_3"].setText(_quiz.millionaires.at(_questions).questions.at(_question).answers.at(3));
      _quiz.entities["answer_3"].setTargetPosition(0.5f, 0.90625f);
      break;

    case 5: // Lock player answer
    {
      bool done = true;
      for (auto answer : _answers)
        if (answer == -1) {
          done = false;
          break;
        }
      if (done == true) {
        setAnswer();
        return;
      }
      else
        std::cout << "Waiting for players answers." << std::endl;
      break;
    }

    default: // Player must select an answer first
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }
  }

  auto& window = Game::Window::Instance();

  // Jokers
  if (_display == 5)
  {
    // 50 50
    if (window.keyboard().keyPressed(Game::Window::Key::Num1) == true &&
      (window.keyboard().keyDown(Game::Window::Key::LShift) == true || _joker5050 == true)) {
      _joker5050 = false;

      std::vector<unsigned int> answers = { 0, 1, 2, 3 };

      // Remove correct answer from list
      answers.erase(
        std::next(answers.begin(), _quiz.millionaires.at(_questions).questions.at(_question).correct),
        std::next(answers.begin(), _quiz.millionaires.at(_questions).questions.at(_question).correct + 1)
      );
      
      std::default_random_engine  randomEngine((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());

      // Shuffle list
      std::shuffle(answers.begin(), answers.end(), randomEngine);

      // Remove 2 answers
      _quiz.entities["answer_" + std::to_string(answers[0])].setText(L"");
      _quiz.entities["answer_" + std::to_string(answers[1])].setText(L"");

      // Grey out joker
      _quiz.entities["joker_5050"].setTargetColor(1.f, 1.f, 1.f, 0.5f);

      auto sound = Game::Audio::Sound::Instance().get();
      
      // Play sound
      if (sound.buffer.loadFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "joker_5050.ogg") == false)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      sound.sound.play();
      return;
    }

    // Audience
    if (window.keyboard().keyPressed(Game::Window::Key::Num2) == true &&
      (window.keyboard().keyDown(Game::Window::Key::LShift) == true || _jokerAudience == true)) {
      _jokerAudience = false;

      // Grey out joker
      _quiz.entities["joker_audience"].setTargetColor(1.f, 1.f, 1.f, 0.5f);

      // Change state
      setJokerAudience();
      return;
    }

    // Phone
    if (window.keyboard().keyPressed(Game::Window::Key::Num3) == true &&
      (window.keyboard().keyDown(Game::Window::Key::LShift) == true || _jokerPhone == true)) {
      _jokerPhone = false;

      // Grey out joker
      _quiz.entities["joker_phone"].setTargetColor(1.f, 1.f, 1.f, 0.5f);

      // Change state
      setJokerPhone();
      return;
    }

    // Phone
    if (window.keyboard().keyPressed(Game::Window::Key::Num4) == true &&
      (window.keyboard().keyDown(Game::Window::Key::LShift) == true || _jokerHost == true)) {
      _jokerHost = false;

      // Grey out joker
      _quiz.entities["joker_host"].setTargetColor(1.f, 1.f, 1.f, 0.5f);

      // Change state
      setJokerHost();
      return;
    }

    // Switch
    if (window.keyboard().keyPressed(Game::Window::Key::Num5) == true &&
      (window.keyboard().keyDown(Game::Window::Key::LShift) == true || _jokerSwitch == true)) {
      _jokerSwitch = false;

      // Grey out joker
      _quiz.entities["joker_switch"].setTargetColor(1.f, 1.f, 1.f, 0.5f);

      // Change state
      setJokerSwitch();
    }
  }

  // Get player answers
  for (unsigned int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));

    // Register players answers
    for (unsigned int button : { QUIZ::Quiz::Button::ButtonBlue, QUIZ::Quiz::Button::ButtonOrange, QUIZ::Quiz::Button::ButtonGreen, QUIZ::Quiz::Button::ButtonYellow }) {
      if (window.joystick().buttonPressed(player.joystick, player.button + button) == true) {
        _answers[index] = 4 - button;

        // Blink player portrait
        entity.setColor(1.f, 1.f, 1.f, 0.5f);
        entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
        entity.setLerp(0.25f);

        // Reveal main player answer
        if (index == _player) {
          _quiz.entities["answer_0"].setTargetColor(0x51 / 255.f, 0x8a / 255.f, 0xc1 / 255.f, 1.f);
          _quiz.entities["answer_1"].setTargetColor(0xf1 / 255.f, 0x49 / 255.f, 0x02 / 255.f, 1.f);
          _quiz.entities["answer_2"].setTargetColor(0x3d / 255.f, 0x9f / 255.f, 0x05 / 255.f, 1.f);
          _quiz.entities["answer_3"].setTargetColor(0xeb / 255.f, 0xde / 255.f, 0x0a / 255.f, 1.f);
          _quiz.entities["answer_" + std::to_string(_answers[index])].setTargetColor(1.f, 1.f, 1.f, 1.f);
        }
      }
    }
  }
}

void  QUIZ::MillionaireQuizScene::setAnswer()
{
  // State state
  _state = State::Answer;

  // Set main player answer to orange
  _quiz.entities["answer_0"].setTargetColor(0x51 / 255.f, 0x8a / 255.f, 0xc1 / 255.f, 0.5f);
  _quiz.entities["answer_1"].setTargetColor(0xf1 / 255.f, 0x49 / 255.f, 0x02 / 255.f, 0.5f);
  _quiz.entities["answer_2"].setTargetColor(0x3d / 255.f, 0x9f / 255.f, 0x05 / 255.f, 0.5f);
  _quiz.entities["answer_3"].setTargetColor(0xeb / 255.f, 0xde / 255.f, 0x0a / 255.f, 0.5f);
  _quiz.entities["answer_" + std::to_string(_answers[_player])].setTargetColor(0xff / 255.f, 0x5c / 255.f, 0x00 / 255.f, 1.f);

  // Start question music
  if (SoundAnswer.at(_step).path.empty() == false) {
    if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / SoundAnswer.at(_step).path) == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    _music.setLoopPoints({ .offset = sf::seconds(SoundAnswer.at(_step).offset), .length = sf::seconds(SoundAnswer.at(_step).length) });
    _music.setLooping(true);
    _music.play();
  }

  // No suspence, go to result
  else {
    setResult();
    return;
  }

  // Host instructions
  Game::Utilities::Clear();
  std::cout << std::endl
    << "--- WHO WANT TO BE A MILLIONAIRE ? ---" << std::endl
    << "Suspence before answer reveal." << std::endl
    << std::endl;

  // Display correct answer
  const auto& question = _quiz.millionaires.at(_questions).questions.at(_question);
  std::wcout
    << "Question " << (_step + 1) << " / " << QuestionNumber << ": " << question.question << std::endl
    << "  A: " << question.answers.at(0) << std::endl
    << "  B: " << question.answers.at(1) << std::endl
    << "  C: " << question.answers.at(2) << std::endl
    << "  D: " << question.answers.at(3) << std::endl
    << std::endl
    << "Correct:  " << (char)('A' + question.correct) << ", " << question.answers.at(question.correct) << std::endl
    << "Info:" << std::endl << question.info << std::endl;

  // Commands
  std::cout << std::endl
    << "Commands:" << std::endl
    << "  [N]ext:   reveal answer" << std::endl;
}

void  QUIZ::MillionaireQuizScene::updateAnswer(float elapsed)
{
  // Display results
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::N) == true)
    setResult();
}

void  QUIZ::MillionaireQuizScene::setResult()
{
  // State state
  _state = State::Result;

  const auto& question = _quiz.millionaires.at(_questions).questions.at(_question);

  // Stop suspence
  if (_step >= 4 || _answers.at(_player) != question.correct)
    _music.stop();

  auto sound = Game::Audio::Sound::Instance().get();

  if (sound.buffer.loadFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / ((_answers.at(_player) == question.correct) ? SoundCorrect : SoundWrong).at(_step)) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  sound.sound.play();

  // Host instructions
  Game::Utilities::Clear();
  std::cout << std::endl
    << "--- WHO WANT TO BE A MILLIONAIRE ? ---" << std::endl
    << "Choose if player continue the game." << std::endl
    << std::endl;

  // Display correct answer
  std::wcout
    << "Question " << (_step + 1) << " / " << QuestionNumber << ": " << question.question << std::endl
    << "  Answer A: " << question.answers.at(0) << std::endl
    << "  Answer B: " << question.answers.at(1) << std::endl
    << "  Answer C: " << question.answers.at(2) << std::endl
    << "  Answer D: " << question.answers.at(3) << std::endl
    << std::endl
    << "Correct:  " << (char)('A' + question.correct) << ", " << question.answers.at(question.correct) << std::endl
    << "Info:     " << question.info << std::endl
    << std::endl;
  
  // Give score to each player
  std::cout << "Correct players win " << ScoreAll.at(_step) << " points." << std::endl;
  for (unsigned int index = 0; index < _quiz.players.size(); index++)
    if (_answers.at(index) == question.correct && index != _player)
      _quiz.players.at(index).score += ScoreAll.at(_step);

  // Main player still in game
  if (_lose == false) {
    if (_answers.at(_player) == question.correct) {
      _score = ScoreWin.at(_step);
      std::cout << "Main player is correct, he get " << _score << " points if he ends the game now." << std::endl;
    }
    else {
      _score = ScoreFail.at(_step);
      _lose = true;
      std::cout << "Main player lose the game, he get " << _score << " points." << std::endl;
    }
  }

  // Main played has already lost the game
  else
    std::cout << "Main player is already out with a score of " << _score << " points." << std::endl;

  // Host commands
  std::cout << std::endl << "Commands:" << std::endl;
  if (_step < QuestionNumber - 1) {
    std::cout
      << "  [N]ext:   next question (" << ScoreWin.at(_step + 1) << " points, " << ScoreFail.at(_step + 1) << " if failed)" << std::endl
      << "  [S]ave:   save score (" << _score << " points), no more points" << std::endl;
  }
  else {
    std::cout
      << "  [N]ext:   show final score" << std::endl;
  }

  // Set answer color
  _quiz.entities["answer_" + std::to_string(_answers[_player])].setTargetColor(0xff / 255.f, 0x00 / 255.f, 0x00 / 255.f, 1.f);
  _quiz.entities["answer_" + std::to_string(_quiz.millionaires[_questions].questions[_question].correct)].setTargetColor(0x00 / 255.f, 0xff / 255.f, 0x00 / 255.f, 1.f);

  // Change player color
  for (unsigned int index = 0; index < _quiz.players.size(); index++) {
    if (index == _player)
      continue;
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));
    if (_answers[index] == _quiz.millionaires[_questions].questions[_question].correct)
      entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
    else
      entity.setTargetColor(1.f, 1.f, 1.f, 0.312f);
  }
}

void  QUIZ::MillionaireQuizScene::updateResult(float elapsed)
{
  // Go to next question
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::N) == true)
    if (_step < QuestionNumber - 1)
      setLight(_step + 1);
    else
      setScore();

  // End game
  else if (_step < QuestionNumber - 1 && Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::S) == true)
    setScore();
}

void  QUIZ::MillionaireQuizScene::setScore()
{
  // Change state
  _state = State::Score;

  // Player exit music
  if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "exit.ogg") == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  _music.setLooping(false);
  _music.play();

  // Lock player score
  _lose = true;

  // Host instructions
  Game::Utilities::Clear();
  std::cout << std::endl
    << "--- WHO WANT TO BE A MILLIONAIRE ? ---" << std::endl
    << "Choose to continue or stop the game." << std::endl
    << std::endl
    << "Commands:" << std::endl;
  if (_step < QuestionNumber - 1)
    std::cout << "  [N]ext:   next question (" << ScoreWin.at(_step + 1) << " points, " << ScoreFail.at(_step + 1) << " if failed)" << std::endl;
  std::cout << "  [E]nd:    end game" << std::endl
    << std::endl;

  // Show main player score
  std::cout << "Main player won " << _score << " points during the game." << std::endl;

  // Show score
  auto& final = _quiz.entities["final"];
  final.setTargetScale(0.45f, 0.45f);
  final.setTargetColor(1.f, 1.f, 1.f, 1.f);
  final.setText(std::to_wstring(_score));
  _quiz.entities["answer_0"].setTargetColor(0x51 / 255.f, 0x8a / 255.f, 0xc1 / 255.f, 0.1f);
  _quiz.entities["answer_1"].setTargetColor(0xf1 / 255.f, 0x49 / 255.f, 0x02 / 255.f, 0.1f);
  _quiz.entities["answer_2"].setTargetColor(0x3d / 255.f, 0x9f / 255.f, 0x05 / 255.f, 0.1f);
  _quiz.entities["answer_3"].setTargetColor(0xeb / 255.f, 0xde / 255.f, 0x0a / 255.f, 0.1f);
}

void  QUIZ::MillionaireQuizScene::updateScore(float elapsed)
{
  // Go to next question
  if (_step < QuestionNumber - 1 && Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::N) == true)
    setLight(_step + 1);

  // Return to main menu
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::E) == true)
  {
    // Give main player its points
    _quiz.players.at(_player).score += _score;

    // Stop game
    _machine.pop();
    return;
  }
}

void  QUIZ::MillionaireQuizScene::setJokerPhone()
{
  // Set state
  _state = State::JokerPhone;

  // Start phone music
  if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "joker_phone.ogg") == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  _music.setLoopPoints({ .offset = sf::seconds(8.f), .length = sf::seconds(16.f) });
  _music.setLooping(true);
  _music.play();

  // Show timer
  _quiz.progress.show();
  _quiz.progress.set(1.f);
}

void  QUIZ::MillionaireQuizScene::updateJokerPhone(float elapsed)
{
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::N) == true)
  {
    // Start phone time
    if (_music.getStatus() == sf::Music::Status::Playing && _music.getPlayingOffset().asSeconds() < 28.05f) {
      _music.setLooping(false);
      _music.setPlayingOffset(sf::seconds(28.05f));
    }

    // End phone time early
    else if (_music.getStatus() == sf::Music::Status::Playing && _music.getPlayingOffset().asSeconds() < 58.05f) {
      _music.setPlayingOffset(sf::seconds(58.05f));
    } 
  }

  // Return to question
  if (_music.getStatus() != sf::Music::Status::Playing) {
    _state = State::Question;

    // Restart question music
    if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / SoundQuestion.at(_step).path) == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    _music.setLoopPoints({ .offset = sf::seconds(SoundQuestion.at(_step).offset), .length = sf::seconds(SoundQuestion.at(_step).length) });
    _music.setLooping(true);
    _music.play();

    // Hide phone bar
    _quiz.progress.hide();
    return;
  }

  // Update value of progress bar
  _quiz.progress.set(
    (_music.getStatus() == sf::Music::Status::Playing) ?
    std::clamp((30.f - (_music.getPlayingOffset().asSeconds() - 28.05f)) / 30.f, 0.f, 1.f) :
    0.f
  );
}

void  QUIZ::MillionaireQuizScene::setJokerAudience()
{
  // Set state
  _state = State::JokerAudience;

  // Reset audience answers
  _audience = std::vector<int>(_quiz.players.size(), -1);
  _audience[_player] =
    (std::rand() % 3 != 0) ?
    (_quiz.millionaires[_questions].questions[_question].correct) :
    (std::rand() % 4);
  
  // Show timer bar
  _quiz.progress.show();
  _quiz.progress.set(1.f);

  // Reset main player answer
  _quiz.entities["answer_0"].setTargetColor(0x51 / 255.f, 0x8a / 255.f, 0xc1 / 255.f, 1.f);
  _quiz.entities["answer_1"].setTargetColor(0xf1 / 255.f, 0x49 / 255.f, 0x02 / 255.f, 1.f);
  _quiz.entities["answer_2"].setTargetColor(0x3d / 255.f, 0x9f / 255.f, 0x05 / 255.f, 1.f);
  _quiz.entities["answer_3"].setTargetColor(0xeb / 255.f, 0xde / 255.f, 0x0a / 255.f, 1.f);
  _answers.at(_player) = -1;

  // Start music
  if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "joker_audience.ogg") == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  _music.setLoopPoints({ .offset = sf::seconds(4.f), .length = sf::seconds(24.f) });
  _music.setLooping(true);
  _music.play();
}

void  QUIZ::MillionaireQuizScene::updateJokerAudience(float elapsed)
{
  // Start answers
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::N) == true && _music.getStatus() == sf::Music::Status::Playing) {
    if (_music.getPlayingOffset().asSeconds() < 29.f) {
      _music.setLooping(false);
      _music.setPlayingOffset(sf::seconds(29.f));
    }
    else if (_music.getPlayingOffset().asSeconds() < 39.088f) {
      _music.setLooping(false);
      _music.setPlayingOffset(sf::seconds(39.088f));
    }
  }

  // Return to question
  if (_music.getStatus() != sf::Music::Status::Playing) {
    _state = State::Question;

    // Hide progress
    _quiz.progress.hide();

    // Restart question music
    if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / SoundQuestion.at(_step).path) == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    _music.setLoopPoints({ .offset = sf::seconds(SoundQuestion.at(_step).offset), .length = sf::seconds(SoundQuestion.at(_step).length) });
    _music.setLooping(true);
    _music.play();
    return;
  }

  const auto& window = Game::Window::Instance();

  // Player vote
  if (_music.getPlayingOffset().asSeconds() > 29.f && _music.getPlayingOffset().asSeconds() < 39.088f) {
    for (unsigned int index = 0; index < _quiz.players.size(); index++) {
      if (index == _player)
        continue;
      
      auto& player = _quiz.players[index];

      // Register players answers
      for (unsigned int button : { QUIZ::Quiz::Button::ButtonBlue, QUIZ::Quiz::Button::ButtonOrange, QUIZ::Quiz::Button::ButtonGreen, QUIZ::Quiz::Button::ButtonYellow }) {
        if (window.joystick().buttonPressed(player.joystick, player.button + button) == true) {
          _audience[index] = 4 - button;
        }
      }
    }
  }

  if (_music.getPlayingOffset().asSeconds() > 39.088f && _audience.empty() == false) {
    std::vector<unsigned int> results(4, 0);
    unsigned int sum = 0;

    // Sum results
    for (auto value : _audience)
      if (value != -1) {
        results[value] += 1;
        sum += 1;
      }

    for (unsigned int index = 0; index < 4; index++) {
      int result = (int)std::round((float)results[index] / (float)sum * 100.f);

      _quiz.entities["audience_" + std::to_string(index)].setText(std::to_wstring(result) + L" %");
      _quiz.entities["audience_" + std::to_string(index)].setTargetColor(1.f, 1.f, 1.f, 1.f);
    }

    // Reset audience
    _audience.clear();
  }

  // Update progress bar
  _quiz.progress.set(
    std::clamp(1.f - (_music.getPlayingOffset().asSeconds() - 29.f) / 10.088f, 0.f, 1.f)
  );
}

void  QUIZ::MillionaireQuizScene::setJokerHost()
{
  // Set state
  _state = State::JokerHost;

  // Start phone music
  if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "joker_host.ogg") == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  _music.setLoopPoints({ .offset = sf::seconds(20.6f), .length = sf::seconds(64.f) });
  _music.setLooping(true);
  _music.play();
}

void  QUIZ::MillionaireQuizScene::updateJokerHost(float elapsed)
{
  // Start phone time
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::N) == true && _music.getStatus() == sf::Music::Status::Playing && _music.getPlayingOffset().asSeconds() < 91.463f) {
    _music.setLooping(false);
    _music.setPlayingOffset(sf::seconds(91.463f));
  }

  // Return to question
  if (_music.getStatus() != sf::Music::Status::Playing) {
    _state = State::Question;

    // Restart question music
    if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / SoundQuestion.at(_step).path) == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    _music.setLoopPoints({ .offset = sf::seconds(SoundQuestion.at(_step).offset), .length = sf::seconds(SoundQuestion.at(_step).length) });
    _music.setLooping(true);
    _music.play();
    return;
  }
}

void  QUIZ::MillionaireQuizScene::setJokerSwitch()
{
  // Set state
  _state = State::JokerSwitch;

  // Switch sound
  if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "joker_switch.ogg") == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  _music.setLooping(false);
  _music.play();

  // Hide question boxes
  _quiz.entities["question"].setTargetPosition(0.5f, 0.125f - 0.5f);
  _quiz.entities["question"].setTargetColor(1.f, 1.f, 1.f, 1.f);
  _quiz.entities["answer_0"].setTargetPosition(0.5f, 0.34375f + 1.f);
  _quiz.entities["answer_0"].setTargetColor(0x51 / 255.f, 0x8a / 255.f, 0xc1 / 255.f, 1.f);
  _quiz.entities["answer_1"].setTargetPosition(0.5f, 0.53125f + 1.f);
  _quiz.entities["answer_1"].setTargetColor(0xf1 / 255.f, 0x49 / 255.f, 0x02 / 255.f, 1.f);
  _quiz.entities["answer_2"].setTargetPosition(0.5f, 0.71875f + 1.f);
  _quiz.entities["answer_2"].setTargetColor(0x3d / 255.f, 0x9f / 255.f, 0x05 / 255.f, 1.f);
  _quiz.entities["answer_3"].setTargetPosition(0.5f, 0.90625f + 1.f);
  _quiz.entities["answer_3"].setTargetColor(0xeb / 255.f, 0xde / 255.f, 0x0a / 255.f, 1.f);
  _quiz.entities["audience_0"].setTargetColor(1.f, 1.f, 1.f, 0.f);
  _quiz.entities["audience_1"].setTargetColor(1.f, 1.f, 1.f, 0.f);
  _quiz.entities["audience_2"].setTargetColor(1.f, 1.f, 1.f, 0.f);
  _quiz.entities["audience_3"].setTargetColor(1.f, 1.f, 1.f, 0.f);
}

void  QUIZ::MillionaireQuizScene::updateJokerSwitch(float elapsed)
{
  // Wait to switch question
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::N) == true)
    setQuestion();
}

bool  QUIZ::MillionaireQuizScene::update(float elapsed)
{
  switch (_state) {
  case State::Select:
    updateSelect(elapsed); break;
  case State::Light:
    updateLight(elapsed); break;
  case State::Question:
    updateQuestion(elapsed); break;
  case State::Answer:
    updateAnswer(elapsed); break;
  case State::Result:
    updateResult(elapsed); break;
  case State::Score:
    updateScore(elapsed); break;

  case State::JokerPhone:
    updateJokerPhone(elapsed); break;
  case State::JokerAudience:
    updateJokerAudience(elapsed); break;
  case State::JokerHost:
    updateJokerHost(elapsed); break;
  case State::JokerSwitch:
    updateJokerSwitch(elapsed); break;

  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
  return false;
}

void  QUIZ::MillionaireQuizScene::draw()
{}