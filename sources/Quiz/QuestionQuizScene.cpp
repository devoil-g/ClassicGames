#include <iostream>

#include "Quiz/QuestionQuizScene.hpp"
#include "System/Config.hpp"
#include "System/Utilities.hpp"
#include "System/Window.hpp"
#include "System/Audio/Sound.hpp"
#include "System/Library/SoundLibrary.hpp"

QUIZ::QuestionQuizScene::QuestionQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz/*, QUIZ::Quiz::Question& question*/) :
  Game::AbstractScene(machine),
  _quiz(quiz),
  _cooldowns(_quiz.players.size(), 0.f),
  _buzz(-1)
{
  // Host instructions
  Game::Utilities::Clear();
  std::cout
    << "--- FREE QUESTIONS ---" << std::endl
    << "Instruction for players: use the red buzzer to answer" << std::endl
    << std::endl
    << "Commands:" << std::endl
    << "  Arrow[U/D]: change cooldown" << std::endl
    << "  [C]orrect:  correct answer" << std::endl
    << "  [W]rong:    wrong answer" << std::endl
    << "  [R]eset:    no answer, reset buzzer (shift for quiet)" << std::endl
    << "  [L]ock:     lock buzzers" << std::endl
    << "  [E]nd:      return to main menu" << std::endl
    << std::endl;

  // Preload sounds
  Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_correct.wav");
  Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_wrong.wav");
  Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_timesup.wav");
}

bool  QUIZ::QuestionQuizScene::update(float elapsed)
{
  // No one is buzzing
  if (_buzz == -1)
  {
    // For each player
    for (int index = 0; index < _quiz.players.size(); index++)
    {
      // Reduce cooldowns
      _cooldowns[index] = std::max(0.f, _cooldowns[index] - elapsed);

      // Check if player is buzzing
      if (_buzz == -1 && _cooldowns[index] <= 0.f && Game::Window::Instance().joystick().buttonPressed(_quiz.players[index].joystick, _quiz.players[index].button + QUIZ::Quiz::Button::ButtonBuzzer) == true) {
        auto  ref = Game::Audio::Sound::Instance().get();

        // Play buzzer sound
        ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_buzzer.wav"));
        ref.sound.play();

        // Register buzzing player
        _buzz = index;
      }
    }
  }

  // Host control
  {
    // Player is correct
    if (_buzz != -1 && Game::Window::Instance().keyboard().keyPressed(Game::Window::Keyboard::Key::C) == true) {
      auto  ref = Game::Audio::Sound::Instance().get();

      // Play correct answer sound
      ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_correct.wav"));
      ref.sound.play();

      // Increase score
      _quiz.players[_buzz].score += 1;

      // Grey out other players
      std::fill(_cooldowns.begin(), _cooldowns.end(), 1.2f);
      _cooldowns[_buzz] = 0.f;

      // Reset buzzer
      _buzz = -1;
    }

    // Player is wrong
    if (_buzz != -1 && Game::Window::Instance().keyboard().keyPressed(Game::Window::Keyboard::Key::W) == true) {
      auto  ref = Game::Audio::Sound::Instance().get();

      // Play wrong answer sound
      ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_wrong.wav"));
      ref.sound.play();

      // Fixed time out
      if (_cooldown > 0.f) {
        _cooldowns[_buzz] = _cooldown;
        for (int index = 0; index < _quiz.players.size(); index++) {
          _cooldowns[index] = std::max(0.5f, _cooldowns[index]);
        }
      }

      // Only wrong player can't answer
      else {
        for (int index = 0; index < _quiz.players.size(); index++) {
          _cooldowns[index] = 0.5f;
        }
        _cooldowns[_buzz] = std::numeric_limits<float>::max();
      }

      // Reset buzzer
      _buzz = -1;
    }

    // No answer, reset
    if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Keyboard::Key::R) == true)
    {
      // Shift for quiet mode
      if (Game::Window::Instance().keyboard().keyDown(Game::Window::Keyboard::Key::LShift) == false &&
        Game::Window::Instance().keyboard().keyDown(Game::Window::Keyboard::Key::RShift) == false) {
        auto  ref = Game::Audio::Sound::Instance().get();

        // Play correct answer sound
        ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_timesup.wav"));
        ref.sound.play();
      }

      // Reset cooldowns
      for (int index = 0; index < _quiz.players.size(); index++) {
        _cooldowns[index] = 1.f;
      }

      // Reset buzzer
      _buzz = -1;
    }

    // Change cooldown
    auto up = Game::Window::Instance().keyboard().keyPressed(Game::Window::Keyboard::Key::Up);
    auto down = Game::Window::Instance().keyboard().keyPressed(Game::Window::Keyboard::Key::Down);

    if (up == true || down == true) {
      auto shift = Game::Window::Instance().keyboard().keyDown(Game::Window::Keyboard::Key::LShift) || Game::Window::Instance().keyboard().keyDown(Game::Window::Keyboard::Key::RShift);

      _cooldown = std::max(0.f, _cooldown + ((up == true ? +0.5f : 0.f) + (down == true ? -0.5f : 0.f)) * (shift == true ? 6.f : 1.f));
      std::cout << "Cooldown set to " << _cooldown << "s.                \r" << std::flush;
    }

    // Lock player buzzers
    if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Keyboard::Key::L) == true)
    {
      _buzz = -1;
      for (int index = 0; index < _quiz.players.size(); index++) {
        _cooldowns[index] = std::numeric_limits<float>::max();
      }
    }
    
    // End questions
    if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Keyboard::Key::E) == true) {
      _machine.pop();
      return false;
    }
  }

  // Set players visibility
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));
    float opacity = 1.f;
    float scale = (_buzz == index) ? 1.f : 0.9f;

    if (_buzz != -1 && _buzz != index)
      opacity /= 2.f;
    if (_cooldowns[index] > 0.f && _buzz != index)
      opacity /= 2.f;

    entity.setTargetPosition((index + 1.f) / (_quiz.players.size() + 1.f), 0.5f);
    entity.setTargetScale(scale * 0.95f / (_quiz.players.size() + 1.f), scale * 0.95f);
    entity.setTargetColor(1.f, 1.f, 1.f, opacity);
    entity.setLerp(0.0625f);
  }

  return false;
}

void  QUIZ::QuestionQuizScene::draw()
{}
