#include <iostream>

#include "System/Config.hpp"
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
  joker_5050.setTexture(Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "joker_5050.png");
  joker_5050.setColor(1.f, 1.f, 1.f, 0.f);
  joker_5050.setPosition(0.9f, 0.1f);
  joker_5050.setScale(0.18f, 0.18f);
  joker_5050.setLerp(0.125f);
  auto& joker_audience = _quiz.entities["joker_audience"];
  joker_audience.setTexture(Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "joker_audience.png");
  joker_audience.setColor(1.f, 1.f, 1.f, 0.f);
  joker_audience.setPosition(0.9f, 0.3f);
  joker_audience.setScale(0.18f, 0.18f);
  joker_audience.setLerp(0.125f);
  auto& joker_phone = _quiz.entities["joker_phone"];
  joker_phone.setTexture(Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "joker_phone.png");
  joker_phone.setColor(1.f, 1.f, 1.f, 0.f);
  joker_phone.setPosition(0.9f, 0.5f);
  joker_phone.setScale(0.18f, 0.18f);
  joker_phone.setLerp(0.125f);
  auto& joker_host = _quiz.entities["joker_host"];
  joker_host.setTexture(Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "joker_host.png");
  joker_host.setColor(1.f, 1.f, 1.f, 0.f);
  joker_host.setPosition(0.9f, 0.7f);
  joker_host.setScale(0.18f, 0.18f);
  joker_host.setLerp(0.125f);
  auto& joker_switch = _quiz.entities["joker_switch"];
  joker_switch.setTexture(Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "joker_switch.png");
  joker_switch.setColor(1.f, 1.f, 1.f, 0.f);
  joker_switch.setPosition(0.9f, 0.9f);
  joker_switch.setScale(0.18f, 0.18f);
  joker_switch.setLerp(0.125f);

  // Host instructions
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

  std::wcout << "Question set " << _questions << ": '" << _quiz.millionaires.at(_questions).name << "'" << (_quiz.millionaires.at(_questions).done ? " (done)" : "") << "." << std::endl;
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

    std::wcout << "Question set " << _questions << ": '" << _quiz.millionaires.at(_questions).name << "'" << (_quiz.millionaires.at(_questions).done ? " (done)" : "") << "." << std::endl;
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
      entity.setTargetPosition(0.1f, _quiz.players.size() == 1 ? 0.5f : 0.4f);
      entity.setTargetScale(0.18f, _quiz.players.size() == 1 ? 0.95f : 0.75f);
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

  // Play light sound
  if (SoundLight.at(_step).empty() == false) {
    auto sound = Game::Audio::Sound::Instance().get();

    sound.buffer.loadFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / SoundLight.at(_step));
    sound.sound.play();
  }

  // Skip to question
  else {
    setQuestion();
    return;
  }

  // TODO: display (hide question and answers boxes)

  // Host instructions
  std::cout << std::endl
    << "--- WHO WANT TO BE A MILLIONAIRE ? ---" << std::endl
    << "Lights at the center, ready to start next question." << std::endl
    << std::endl
    << "Commands:" << std::endl
    << "  [N]ext:   start question" << std::endl
    << std::endl;

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
    _music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / SoundQuestion.at(_step).path);
    _music.setLoopPoints({ .offset = sf::seconds(SoundQuestion.at(_step).offset), .length = sf::seconds(SoundQuestion.at(_step).length) });
    _music.setLooping(true);
    _music.play();
  }

  // Show jokers
  _quiz.entities["joker_5050"].setTargetColor(1.f, 1.f, 1.f, _joker5050 == true ? 1.f : 0.5f);
  _quiz.entities["joker_phone"].setTargetColor(1.f, 1.f, 1.f, _jokerPhone == true ? 1.f : 0.5f);
  _quiz.entities["joker_audience"].setTargetColor(1.f, 1.f, 1.f, _jokerAudience == true ? 1.f : 0.5f);
  _quiz.entities["joker_host"].setTargetColor(1.f, 1.f, 1.f, _jokerHost == true ? 1.f : 0.5f);
  _quiz.entities["joker_switch"].setTargetColor(1.f, 1.f, 1.f, _jokerSwitch == true ? 1.f : 0.5f);

  // TODO: display (show question and answer boxes)

  // Host instructions
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
    << std::endl;

  std::cout << "Q:[ ] A:[ ] B:[ ] C:[ ] D:[ ]" << std::flush;

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
      std::cout << "\rQ:[X] A:[ ] B:[ ] C:[ ] D:[ ]" << std::flush;
      _quiz.millionaires.at(_questions).questions.at(_question).done = true;
      _quiz.millionaires.at(_questions).done = true;
      break;
    case 1: // Display answer A
      _display++;
      std::cout << "\rQ:[X] A:[X] B:[ ] C:[ ] D:[ ]" << std::flush;
      break;
    case 2: // Display answer B
      _display++;
      std::cout << "\rQ:[X] A:[X] B:[X] C:[ ] D:[ ]" << std::flush;
      break;
    case 3: // Display answer C
      _display++;
      std::cout << "\rQ:[X] A:[X] B:[X] C:[X] D:[ ]" << std::flush;
      break;
    case 4: // Display answer D
      _display++;
      std::cout << "\rQ:[X] A:[X] B:[X] C:[X] D:[X]" << std::endl;
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
      }
    }
  }

  // TODO: jokers
}

void  QUIZ::MillionaireQuizScene::setAnswer()
{
  // State state
  _state = State::Answer;

  // Start question music
  if (SoundAnswer.at(_step).path.empty() == false) {
    _music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / SoundAnswer.at(_step).path);
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
    << "  [N]ext:   reveal answer" << std::endl
    << std::endl;
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

  sound.buffer.loadFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / ((_answers.at(_player) == question.correct) ? SoundCorrect : SoundWrong).at(_step));
  sound.sound.play();

  // Host instructions
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
  if (_step < QuestionNumber - 1)
    std::cout << "  [N]ext:   next question (" << ScoreWin.at(_step + 1) << " points, " << ScoreFail.at(_step + 1) << " if failed)" << std::endl;
  else
    std::cout << "  [N]ext:   end the game" << std::endl;
  if (_step < QuestionNumber - 1)
    std::cout << "  [S]ave:   save score (" << _score << " points), no more points" << std::endl;
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
  // Player exit music
  _music.openFromFile(Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "exit.ogg");
  _music.setLooping(false);
  _music.play();

  // Lock player score
  _lose = true;

  // Host instructions
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
}

void  QUIZ::MillionaireQuizScene::updateScore(float elapsed)
{
  // Go to next question
  if (_step < QuestionNumber - 1 && Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::E) == true)
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
  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
  return false;
}

void  QUIZ::MillionaireQuizScene::draw()
{}