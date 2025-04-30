#include <random>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "Math/Math.hpp"
#include "Quiz/FastestQuizScene.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Audio/Sound.hpp"
#include "System/Library/SoundLibrary.hpp"

QUIZ::FastestQuizScene::FastestQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz) :
  Game::AbstractScene(machine),
  _quiz(quiz),
  _music(),
  _state(State::Preparation),
  _timer(0.f),
  _question(0)
{
  // Start preparation
  setPreparation();

  // Need at least one fastest question
  if (_quiz.fastests.empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Set first unused question
  while (true) {
    if (_quiz.fastests.at(_question).done == false)
      break;
    else if (_question == _quiz.fastests.size() - 1) {
      _question = 0;
      break;
    }
    else
      _question += 1;
  }

}

QUIZ::FastestQuizScene::~FastestQuizScene()
{
  _quiz.entities["question"].setDead(true);
  _quiz.entities["question"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.entities["answer_1"].setDead(true);
  _quiz.entities["answer_1"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.entities["answer_2"].setDead(true);
  _quiz.entities["answer_2"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.entities["answer_3"].setDead(true);
  _quiz.entities["answer_3"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.entities["answer_4"].setDead(true);
  _quiz.entities["answer_4"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.progress.hide();

  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& chrono = _quiz.entities["chrono_" + std::to_string(player.id)];

    chrono.setDead(true);
    chrono.setTargetColor(0.f, 0.f, 0.f, 0.f);
  }
}

void  QUIZ::FastestQuizScene::setPreparation()
{
  // Set state
  _state = State::Preparation;

  // Set player visible
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));
    auto& chrono = _quiz.entities["chrono_" + std::to_string(player.id)];

    entity.setTargetPosition((index + 1.f) / (_quiz.players.size() + 1.f), 0.5f);
    entity.setTargetScale(0.9f / (_quiz.players.size() + 1.f), 0.75f);
    entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
    entity.setLerp(0.0625f);
    chrono.setDead(true);
    chrono.setTargetColor(0.f, 0.f, 0.f, 0.f);
  }

  // Hide question
  _quiz.entities["question"].setDead(true);
  _quiz.entities["question"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.entities["answer_1"].setDead(true);
  _quiz.entities["answer_1"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.entities["answer_2"].setDead(true);
  _quiz.entities["answer_2"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.entities["answer_3"].setDead(true);
  _quiz.entities["answer_3"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.entities["answer_4"].setDead(true);
  _quiz.entities["answer_4"].setTargetColor(0.f, 0.f, 0.f, 0.f);

  // Hide progress bar
  _quiz.progress.hide();

  // Start music
  if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "fastest_preparation.ogg") == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  _music.setLoopPoints({ .offset = sf::seconds(57.2f), .length = sf::seconds(25.3f) });
  _music.setLooping(true);
  _music.play();

  // Host instructions
  std::cout
    << "--- FASTEST FINGER ---" << std::endl
    << "Instruction for players: use colored button to answer the question," << std::endl
    << "  the fastest correct player is selected as main player. Only your" << std::endl
    << "  first answer will be registered." << std::endl
    << std::endl
    << "Commands:" << std::endl
    << "  [N]ext:   next step" << std::endl
    << "  [S]kip:   skip answer time" << std::endl
    << "  [R]eset:  return to preparation" << std::endl
    << "  [<]rrow   select previous question" << std::endl
    << "  [>]rrow   select next question" << std::endl
    << std::endl;

  // Display question to host
  std::wcout << "Question " << _question << ": '" << _quiz.fastests.at(_question).question << "'" << (_quiz.fastests.at(_question).done ? " (done)" : "") << "." << std::endl;
}

void  QUIZ::FastestQuizScene::updatePreparation(float elapsed)
{
  // Select question
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Left) == true ||
    Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Right) == true) {
    _question = (_question
      + (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Left) == true ? _quiz.fastests.size() - 1 : 0)
      + (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Right) == true ? 1 : 0)
      ) % (unsigned int)_quiz.fastests.size();
    std::wcout << "Question " << _question << ": '" << _quiz.fastests.at(_question).question << "'" << (_quiz.fastests.at(_question).done ? " (done)" : "") << "." << std::endl;
  }

  // Start question
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::N) == true)
    setStart();
}

void  QUIZ::FastestQuizScene::setStart()
{
  // Set state
  _state = State::Start;

  // Reset progress bar
  _quiz.progress.show();
  _quiz.progress.set(0.f);

  // Start music
  if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "fastest_question.ogg") == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  _music.setLooping(false);
  _music.play();

  // Move players to right
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));

    entity.setTargetPosition(0.925f, 0.25f + 0.675f * (index + 0.5f) / _quiz.players.size());
    entity.setTargetScale(0.15f, 0.675f / _quiz.players.size() * 0.925f);
  }

  // Initialize question box
  auto& question = _quiz.entities["question"];
  question.reset();
  question.setPosition(0.5f, -0.125f);
  question.setScale(0.95f, 0.175f);
  question.setColor(1.f, 1.f, 1.f, 0.f);
  question.setOutline(0.0125f);
  question.setLerp(0.0625f);
  question.setText(_quiz.fastests.at(_question).question);

  // Initialize answers
  auto& answer1 = _quiz.entities["answer_1"];
  answer1.reset();
  answer1.setPosition(-0.15f, 0.325f);
  answer1.setScale(0.30f, 0.125f);
  answer1.setColor(0x51 / 255.f, 0x8a / 255.f, 0xc1 / 255.f, 0.f);
  answer1.setOutline(0.0125f);
  answer1.setLerp(0.0625f);
  auto& answer2 = _quiz.entities["answer_2"];
  answer2.reset();
  answer2.setPosition(-0.15f, 0.500f);
  answer2.setScale(0.30f, 0.125f);
  answer2.setColor(0xf1 / 255.f, 0x49 / 255.f, 0x02 / 255.f, 0.f);
  answer2.setOutline(0.0125f);
  answer2.setLerp(0.0625f);
  auto& answer3 = _quiz.entities["answer_3"];
  answer3.reset();
  answer3.setPosition(-0.15f, 0.675f);
  answer3.setScale(0.30f, 0.125f);
  answer3.setColor(0x3d / 255.f, 0x9f / 255.f, 0x05 / 255.f, 0.f);
  answer3.setOutline(0.0125f);
  answer3.setLerp(0.0625f);
  auto& answer4 = _quiz.entities["answer_4"];
  answer4.reset();
  answer4.setPosition(-0.15f, 0.850f);
  answer4.setScale(0.30f, 0.125f);
  answer4.setColor(0xeb / 255.f, 0xde / 255.f, 0x0a / 255.f, 0.f);
  answer4.setOutline(0.0125f);
  answer4.setLerp(0.0625f);
}

void  QUIZ::FastestQuizScene::updateStart(float elapsed)
{
  // Show question box
  if (_music.getPlayingOffset().asSeconds() >= 0.375f) {
    auto& question = _quiz.entities["question"];
    question.setTargetPosition(0.5f, 0.125f);
    question.setTargetColor(1.f, 1.f, 1.f, 1.f);
  }

  // Show answers boxes
  if (_music.getPlayingOffset().asSeconds() >= 0.75f) {
    auto& answer1 = _quiz.entities["answer_1"];
    answer1.setTargetPosition(0.175f, 0.325f);
    answer1.setTargetColor(0x51 / 255.f, 0x8a / 255.f, 0xc1 / 255.f, 1.f);

    auto& answer2 = _quiz.entities["answer_2"];
    answer2.setTargetPosition(0.175f, 0.500f);
    answer2.setTargetColor(0xf1 / 255.f, 0x49 / 255.f, 0x02 / 255.f, 1.f);

    auto& answer3 = _quiz.entities["answer_3"];
    answer3.setTargetPosition(0.175f, 0.675f);
    answer3.setTargetColor(0x3d / 255.f, 0x9f / 255.f, 0x05 / 255.f, 1.f);

    auto& answer4 = _quiz.entities["answer_4"];
    answer4.setTargetPosition(0.175f, 0.850f);
    answer4.setTargetColor(0xeb / 255.f, 0xde / 255.f, 0x0a / 255.f, 1.f);
  }

  // Start answering questions
  if (_music.getPlayingOffset().asSeconds() >= 2.f)
    setAnswer();
}

void  QUIZ::FastestQuizScene::setAnswer()
{
  // Set state
  _state = State::Answer;

  auto answers = _quiz.fastests.at(_question).answers;

  // Shuffle answers
  std::shuffle(answers.begin(), answers.end(), std::default_random_engine((unsigned int)std::chrono::system_clock::now().time_since_epoch().count()));

  // Display answers
  _quiz.entities["answer_1"].setText(answers.at(0));
  _quiz.entities["answer_2"].setText(answers.at(1));
  _quiz.entities["answer_3"].setText(answers.at(2));
  _quiz.entities["answer_4"].setText(answers.at(3));

  // Reset answer timer
  _timer = 0.f;

  // Reset players answers
  _answers.clear();
  _answers.resize(_quiz.players.size(), { std::numeric_limits<float>().max(), {} });
}

void  QUIZ::FastestQuizScene::updateAnswer(float elapsed)
{
  // Update progress bar and answer timer
  _quiz.progress.set((_music.getPlayingOffset().asSeconds() - 2.f) / 20.f);
  _timer += elapsed;

  auto& window = Game::Window::Instance();

  // Check players inputs
  if (_music.getPlayingOffset().asSeconds() < 22.f) {
    for (int index = 0; index < _quiz.players.size(); index++) {
      auto& player = _quiz.players[index];
      auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));

      // Register pressed button in order and last press time
      for (unsigned int button : { QUIZ::Quiz::Button::ButtonBlue, QUIZ::Quiz::Button::ButtonOrange, QUIZ::Quiz::Button::ButtonGreen, QUIZ::Quiz::Button::ButtonYellow }) {
        if (window.joystick().buttonPressed(player.joystick, player.button + button) == true) {
          _answers[index].first = _timer;
          _answers[index].second.remove_if([button](auto value) { return value == 5 - button; });
          _answers[index].second.push_back(5 - button);

          // Blink player portrait
          entity.setColor(1.f, 1.f, 1.f, 0.5f);
          entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
          entity.setLerp(0.25f);
        }
      }
    }
  }

  // Skip timer
  if (_music.getPlayingOffset().asSeconds() < 22.f && Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::S) == true)
    _music.setPlayingOffset(sf::seconds(22.f));

  // Show player chronos
  if (_music.getPlayingOffset().asSeconds() >= 22.f)
    setChrono();
}

void  QUIZ::FastestQuizScene::setChrono()
{
  // Set state
  _state = State::Chrono;

  // Hide timer bar
  _quiz.progress.hide();

  // Display chronos
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));
    auto& chrono = _quiz.entities["chrono_" + std::to_string(player.id)];

    chrono.reset();
    chrono.setPosition(0.775f, entity.getTargetPosition().y());
    chrono.setScale(0.2f, entity.getTargetScale().y() * 0.5f);
    chrono.setColor(1.f, 1.f, 1.f, 0.f);
    if (_answers[index].first != std::numeric_limits<float>().max())
      chrono.setTargetColor(1.f, 1.f, 1.f, 1.f);
    else
      entity.setTargetColor(1.f, 1.f, 1.f, 0.25f);
    chrono.setLerp(0.0625f);

    std::wstringstream stream;
    stream << std::setprecision(3) << std::fixed << _answers[index].first;
    chrono.setText(stream.str());
  }
}

void  QUIZ::FastestQuizScene::updateChrono(float elapsed)
{
  // Show player chronos
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::N) == true)
    setCorrect();
}

void  QUIZ::FastestQuizScene::setCorrect()
{
  // Set state
  _state = State::Correct;

  // Start correct music
  if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "fastest_correct.ogg") == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  _music.play();

  // Show correct / wrong answers
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));
    auto& chrono = _quiz.entities["chrono_" + std::to_string(player.id)];

    if (_answers[index].first != std::numeric_limits<float>().max()) {
      if (_answers[index].second.size() == 4 &&
        _quiz.entities.at("answer_" + std::to_string(*std::next(_answers[index].second.begin(), 0))).getText() == _quiz.fastests.at(_question).answers.at(0) &&
        _quiz.entities.at("answer_" + std::to_string(*std::next(_answers[index].second.begin(), 1))).getText() == _quiz.fastests.at(_question).answers.at(1) &&
        _quiz.entities.at("answer_" + std::to_string(*std::next(_answers[index].second.begin(), 2))).getText() == _quiz.fastests.at(_question).answers.at(2) &&
        _quiz.entities.at("answer_" + std::to_string(*std::next(_answers[index].second.begin(), 3))).getText() == _quiz.fastests.at(_question).answers.at(3)) {
        entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
        chrono.setColor(0.f, 0.f, 0.f, 1.f);
        chrono.setTargetColor(0.2f, 1.f, 0.2f, 1.f);
        chrono.setOutline(0.0125f);
      }
      else {
        entity.setTargetColor(1.f, 1.f, 1.f, 0.25f);
        chrono.setColor(0.f, 0.f, 0.f, 1.f);
        chrono.setTargetColor(1.f, 0.2f, 0.2f, 1.f);
        chrono.setOutline(0.0125f);
      }
    }
  }

  float positions[] = { 0.325f, 0.500f, 0.675f, 0.850f };

  // displayed answers
  for (int index = 1; index <= 4; index++) {
    auto& answer = _quiz.entities["answer_" + std::to_string(index)];
    int position = 0;

    answer.setLerp(0.5f);
    for (int pos = 0; pos < 4; pos++)
      if (answer.getText() == _quiz.fastests.at(_question).answers.at(pos))
        answer.setTargetPosition(answer.getTargetPosition().x(), positions[pos]);
  }
}

void  QUIZ::FastestQuizScene::updateCorrect(float elapsed)
{
  // Show winner
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::N) == true)
    setWinner();
}

void  QUIZ::FastestQuizScene::setWinner()
{
  // Set state
  _state = State::Winner;

  float best = std::numeric_limits<float>().max();
  unsigned int number = 0;
  unsigned int winner;

  // Find best player
  for (unsigned int index = 0; index < _answers.size(); index++) {
    if (_answers[index].first < best) {
      best = _answers[index].first;
      number = 1;
      winner = index;
    }
    else if (_answers[index].first == best) {
      number += 1;
    }
  }

  // No winner
  if (number == 0) {
    if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "1000_wrong.ogg") == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    _music.play();
    std::cout << "No winner." << std::endl;
  }

  // Draw
  else if (number > 1) {
    if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "1000_wrong.ogg") == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    _music.play();
    std::cout << "Draw." << std::endl;
  }

  // Winner !
  else {
    if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "1000_correct.ogg") == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    _music.play();
    std::cout << "Winner: #" << winner << std::endl;

    auto& player = _quiz.players[winner];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));

    entity.setTargetPosition(0.5f, 0.5875f);
    entity.setTargetScale(0.33f, 0.66f);
  }
}

void  QUIZ::FastestQuizScene::updateWinner(float elapsed)
{
  // End fastest finger
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::N) == true)
    _machine.pop();
}

bool  QUIZ::FastestQuizScene::update(float elapsed)
{
  // Restart question
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::R) == true)
    setPreparation();

  // Update quiz
  switch (_state) {
  case State::Preparation:
    updatePreparation(elapsed); break;
  case State::Start:
    updateStart(elapsed); break;
  case State::Answer:
    updateAnswer(elapsed); break;
  case State::Chrono:
    updateChrono(elapsed); break;
  case State::Correct:
    updateCorrect(elapsed); break;
  case State::Winner:
    updateWinner(elapsed); break;
  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }

  return false;
}

void  QUIZ::FastestQuizScene::draw()
{}










/*
void  QUIZ::FastestQuizScene::reset()
{
  // Set state
  _state = State::Preparation;

  // Reset answers
  _answers.clear();
  _answers.resize(_quiz.players.size(), { std::numeric_limits<float>().max(), {} });
  _timer = 0.f;

  // Set player visible
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));
    auto& chrono = _quiz.entities["chrono_" + std::to_string(player.id)];

    entity.setTargetPosition((index + 1.f) / (_quiz.players.size() + 1.f), 0.5f);
    entity.setTargetScale(0.9f / (_quiz.players.size() + 1.f), 0.75f);
    entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
    entity.setLerp(0.0625f);
    chrono.setDead(true);
    chrono.setTargetColor(0.f, 0.f, 0.f, 0.f);
  }

  // Hide question
  _quiz.entities["question"].setDead(true);
  _quiz.entities["question"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.entities["answer_1"].setDead(true);
  _quiz.entities["answer_1"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.entities["answer_2"].setDead(true);
  _quiz.entities["answer_2"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.entities["answer_3"].setDead(true);
  _quiz.entities["answer_3"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.entities["answer_4"].setDead(true);
  _quiz.entities["answer_4"].setTargetColor(0.f, 0.f, 0.f, 0.f);
  _quiz.progress.hide();

  // Start music
  if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "fastest_preparation.ogg") == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  _music.setLoopPoints({ .offset = sf::seconds(57.2f), .length = sf::seconds(25.3f) });
  _music.setLooping(true);
  _music.play();

  // Host instructions
  std::cout
    << "--- FASTEST FINGER ---" << std::endl
    << "Instruction for players: use colored button to answer the question," << std::endl
    << "  the fastest correct player is selected as main player. Only your" << std::endl
    << "  first answer will be registered." << std::endl
    << std::endl
    << "Commands:" << std::endl
    << "  [N]ext:   start question" << std::endl
    << "  [<]rrow   select previous question" << std::endl
    << "  [>]rrow   select next question" << std::endl
    << std::endl;

  // Display question to host
  std::wcout << "Question " << _question << ": '" << _quiz.fastests.at(_question).question << "'" << (_quiz.fastests.at(_question).done ? " (done)" : "") << "." << std::endl;
}

bool  QUIZ::FastestQuizScene::update(float elapsed)
{
  // Select question
  if (_state == State::Preparation) {
    if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Left) == true ||
      Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Right) == true) {
      _question = (_question
        + (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Left) == true ? _quiz.fastests.size() - 1 : 0)
        + (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Right) == true ? 1 : 0)
        ) % (unsigned int)_quiz.fastests.size();
      std::wcout << "Question " << _question << ": '" << _quiz.fastests.at(_question).question << "'" << (_quiz.fastests.at(_question).done ? " (done)" : "") << "." << std::endl;
    }
  }

  // Start question
  if (_state == State::Preparation && Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::N) == true) {
    _quiz.progress.show();
    _quiz.progress.set(0.f);

    // Start music
    if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "fastest_question.ogg") == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    _music.setLooping(false);
    _music.play();

    // Host instructions
    std::cout
      << "--- FASTEST FINGER ---" << std::endl
      << "Wait for players to answer." << std::endl
      << std::endl
      << "Commands:" << std::endl
      << "  [R]eset:  return to preparation" << std::endl
      << "  [S]kip:   skip answer time" << std::endl
      << std::endl;

    _state = State::Beep1;
  }

  if (_state == State::Beep1 && _music.getPlayingOffset().asSeconds() >= 0.f) {
    for (int index = 0; index < _quiz.players.size(); index++) {
      auto& player = _quiz.players[index];
      auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));

      entity.setTargetPosition(0.925f, 0.25f + 0.675f * (index + 0.5f) / _quiz.players.size());
      entity.setTargetScale(0.15f, 0.675f / _quiz.players.size() * 0.925f);

      _state = State::Beep2;
    }
  }

  if (_state == State::Beep2 && _music.getPlayingOffset().asSeconds() >= 0.375f) {
    auto& entity = _quiz.entities["question"];
    entity.reset();
    entity.setPosition(0.5f, -0.125f);
    entity.setTargetPosition(0.5f, 0.125f);
    entity.setScale(0.95f, 0.175f);
    entity.setColor(1.f, 1.f, 1.f, 0.f);
    entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
    entity.setOutline(0.0125f);
    entity.setLerp(0.0625f);
    entity.setText(_quiz.fastests.at(_question).question);

    _state = State::Beep3;
  }

  if (_state == State::Beep3 && _music.getPlayingOffset().asSeconds() >= 0.75f) {
    auto& answer1 = _quiz.entities["answer_1"];
    answer1.reset();
    answer1.setPosition(-0.15f, 0.35f);
    answer1.setTargetPosition(0.175f, 0.325f);
    answer1.setScale(0.30f, 0.125f);
    answer1.setColor(0x51 / 255.f, 0x8a / 255.f, 0xc1 / 255.f, 0.f);
    answer1.setTargetColor(0x51 / 255.f, 0x8a / 255.f, 0xc1 / 255.f, 1.f);
    answer1.setOutline(0.0125f);
    answer1.setLerp(0.0625f);

    auto& answer2 = _quiz.entities["answer_2"];
    answer2.reset();
    answer2.setPosition(-0.15f, 0.55f);
    answer2.setTargetPosition(0.175f, 0.500f);
    answer2.setScale(0.30f, 0.125f);
    answer2.setColor(0xf1 / 255.f, 0x49 / 255.f, 0x02 / 255.f, 0.f);
    answer2.setTargetColor(0xf1 / 255.f, 0x49 / 255.f, 0x02 / 255.f, 1.f);
    answer2.setOutline(0.0125f);
    answer2.setLerp(0.0625f);

    auto& answer3 = _quiz.entities["answer_3"];
    answer3.reset();
    answer3.setPosition(-0.15f, 0.75f);
    answer3.setTargetPosition(0.175f, 0.675f);
    answer3.setScale(0.30f, 0.125f);
    answer3.setColor(0x3d / 255.f, 0x9f / 255.f, 0x05 / 255.f, 0.f);
    answer3.setTargetColor(0x3d / 255.f, 0x9f / 255.f, 0x05 / 255.f, 1.f);
    answer3.setOutline(0.0125f);
    answer3.setLerp(0.0625f);
    
    auto& answer4 = _quiz.entities["answer_4"];
    answer4.reset();
    answer4.setPosition(-0.15f, 0.95f);
    answer4.setTargetPosition(0.175f, 0.850f);
    answer4.setScale(0.30f, 0.125f);
    answer4.setColor(0xeb / 255.f, 0xde / 255.f, 0x0a / 255.f, 0.f);
    answer4.setTargetColor(0xeb / 255.f, 0xde / 255.f, 0x0a / 255.f, 1.f);
    answer4.setOutline(0.0125f);
    answer4.setLerp(0.0625f);
    
    _state = State::Start;
  }

  if (_state == State::Start && _music.getPlayingOffset().asSeconds() >= 2.f) {
    auto answers = _quiz.fastests.at(_question).answers;

    std::shuffle(answers.begin(), answers.end(), std::default_random_engine((unsigned int)std::chrono::system_clock::now().time_since_epoch().count()));

    _quiz.entities["answer_1"].setText(answers.at(0));
    _quiz.entities["answer_2"].setText(answers.at(1));
    _quiz.entities["answer_3"].setText(answers.at(2));
    _quiz.entities["answer_4"].setText(answers.at(3));

    _state = State::Timer;
  }

  if (_state == State::Timer)
  {
    // Update progress bar
    _quiz.progress.set((_music.getPlayingOffset().asSeconds() - 2.f) / 20.f);
    _timer += elapsed;

    auto& window = Game::Window::Instance();

    for (int index = 0; index < _quiz.players.size(); index++) {
      auto& player = _quiz.players[index];
      auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));

      // Register pressed button in order and last press time
      for (unsigned int button : { QUIZ::Quiz::Button::ButtonBlue, QUIZ::Quiz::Button::ButtonOrange, QUIZ::Quiz::Button::ButtonGreen, QUIZ::Quiz::Button::ButtonYellow }) {
        if (window.joystick().buttonPressed(player.joystick, player.button + button) == true) {
          _answers[index].first = _timer;
          _answers[index].second.remove_if([button](auto value) { return value == button; });
          _answers[index].second.push_back(button);

          entity.setColor(1.f, 1.f, 1.f, 0.5f);
          entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
          entity.setLerp(0.25f);
        }
      }
    }

    // Skip timer
    if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::S) == true)
      _music.setPlayingOffset(sf::seconds(22.f));

    // End of timer
    if (_music.getPlayingOffset().asSeconds() >= 22.f) {
      _state = State::Answer;
      _quiz.progress.hide();

      // Display chronos
      for (int index = 0; index < _quiz.players.size(); index++) {
        auto& player = _quiz.players[index];
        auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));
        auto& chrono = _quiz.entities["chrono_" + std::to_string(player.id)];

        chrono.reset();
        chrono.setPosition(0.75f, entity.getTargetPosition().y());
        chrono.setScale(0.2f, entity.getTargetScale().y() * 0.5f);
        chrono.setColor(1.f, 1.f, 1.f, 0.f);
        if (_answers[index].first != std::numeric_limits<float>().max())
          chrono.setTargetColor(1.f, 1.f, 1.f, 1.f);
        else
          entity.setTargetColor(1.f, 1.f, 1.f, 0.25f);
        chrono.setLerp(0.0625f);

        std::wstringstream stream;
        stream << std::setprecision(3) << std::fixed << _answers[index].first;
        chrono.setText(stream.str());
      }
    }
  }

  if (_state == State::Answer && Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::N) == true) {
    if (_music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "fastest_correct.ogg") == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    _music.play();

    // Show correct / wrong answers
    for (int index = 0; index < _quiz.players.size(); index++) {
      auto& player = _quiz.players[index];
      auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));
      auto& chrono = _quiz.entities["chrono_" + std::to_string(player.id)];
      
      std::cout << "answer:" << std::endl;
      std::wcout << _quiz.entities.at("answer_" + std::to_string(*std::next(_answers[index].second.begin(), 0))).getText()
        + _quiz.entities.at("answer_" + std::to_string(*std::next(_answers[index].second.begin(), 1))).getText()
        + _quiz.entities.at("answer_" + std::to_string(*std::next(_answers[index].second.begin(), 2))).getText()
        + _quiz.entities.at("answer_" + std::to_string(*std::next(_answers[index].second.begin(), 3))).getText()
        << std::endl;

      if (_answers[index].first != std::numeric_limits<float>().max()) {
        if (_answers[index].second.size() == 4 &&
          _quiz.entities.at("answer_" + std::to_string(*std::next(_answers[index].second.begin(), 0))).getText()
          + _quiz.entities.at("answer_" + std::to_string(*std::next(_answers[index].second.begin(), 1))).getText()
          + _quiz.entities.at("answer_" + std::to_string(*std::next(_answers[index].second.begin(), 2))).getText()
          + _quiz.entities.at("answer_" + std::to_string(*std::next(_answers[index].second.begin(), 3))).getText()
          ==
          _quiz.fastests.at(_question).answers.at(0)
          + _quiz.fastests.at(_question).answers.at(1)
          + _quiz.fastests.at(_question).answers.at(2)
          + _quiz.fastests.at(_question).answers.at(3)) {
          entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
          chrono.setColor(0.f, 0.f, 0.f, 1.f);
          chrono.setTargetColor(0.2f, 1.f, 0.2f, 1.f);
          chrono.setOutline(0.0125f);
        }
        else {
          entity.setTargetColor(1.f, 1.f, 1.f, 0.25f);
          chrono.setColor(0.f, 0.f, 0.f, 1.f);
          chrono.setTargetColor(1.f, 0.2f, 0.2f, 1.f);
          chrono.setOutline(0.0125f);
        }
      }
    }

    _state = State::Winner;
  }

  else if (_state == State::Winner && Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::N)) {
    float best = std::numeric_limits<float>().max();
    unsigned int number = 0;
    unsigned int winner;

    for (unsigned int index = 0; index < _answers.size(); index++) {
      if (_answers[index].first < best) {
        best = _answers[index].first;
        number = 1;
        winner = index;
      }
      else if (_answers[index].first == best) {
        number += 1;
      }
    }

    if (number == 0)
      std::cout << "No winner." << std::endl;
    else if (number > 1)
      std::cout << "Draw." << std::endl;
    else
      std::cout << "Winner " << winner << std::endl;
      ; // TODO: switch to question 1
  }

  // Start question
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::R) == true) {
    reset();
  }

  return false;
}
*/