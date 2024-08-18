#include <SFML/Graphics/Text.hpp>

#include "Quiz/QuestionQuizScene.hpp"
#include "System/Config.hpp"
#include "Math/Math.hpp"
#include "System/Window.hpp"
#include "System/Audio/Sound.hpp"
#include "System/Library/FontLibrary.hpp"
#include "System/Library/SoundLibrary.hpp"

#include <iostream>

QUIZ::QuestionQuizScene::QuestionQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz) :
  Game::AbstractScene(machine),
  _quiz(quiz),
  _score(std::max((int)_quiz.players.size(), 1)),
  _current((int)_quiz.players.size()),
  _buzz(-1),
  _cooldowns(_quiz.players.size(), 0.f),
  _cooldown(10.f)
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

  // Display usage at start-up
  usage();
}

void  QUIZ::QuestionQuizScene::usage() const
{
  // Host instructions
  std::cout
    << "--- QUESTION ---" << std::endl
    << "Instruction for players: use the big red friendly button to buzz" << std::endl
    << std::endl
    << "Commands:" << std::endl
    << "  [W]rong:      wrong answer, release buzzers" << std::endl
    << "  [C]orrect:    correct answer, reset for new question" << std::endl
    << "  [T]imeout:    timeout, skip to next question" << std::endl
    << "  [R]eset:      quiet timeout" << std::endl
    << "  [L]ock:       lock buzzers, use reset to release" << std::endl
    << "  Arrow[L/R]:   set cooldown" << std::endl
    << "  Arrow[U/D]:   change question score" << std::endl
    << "  [E]nd:        return to main menu" << std::endl
    << std::endl;
}

bool  QUIZ::QuestionQuizScene::update(float elapsed)
{
  // Wrong answer
  if (_buzz != -1 && Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::W) == true) {
    auto  ref = Game::Audio::Sound::Instance().get();

    // Play wrong answer sound
    ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_wrong.wav"));
    ref.sound.play();

    // Set player cooldown
    _cooldowns[_buzz] = (_cooldown == 0.f) ? std::numeric_limits<float>::infinity() : _cooldown;
    _buzz = -1;

    // Decrease question score
    _current = std::max(1, _current - 1);
    std::cout << "\rQuestion score decreased to " << _current << " points.        " << std::flush;
  }

  // Correct answer
  if (_buzz != -1 && Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::C) == true) {
    auto  ref = Game::Audio::Sound::Instance().get();

    // Play correct answer sound
    ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_correct.wav"));
    ref.sound.play();

    // Increment score
    _quiz.players.at(_buzz).score += _current;

    std::cout << "\rPlayer #" << _buzz << " won " << _current << " points!            " << std::flush;

    // Reset players cooldown
    std::fill(_cooldowns.begin(), _cooldowns.end(), 1.2f);
    _cooldowns.at(_buzz) = 0.f;
    _buzz = -1;

    // Reset question score
    _current = _score;
  }

  // Skip to next question
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::T) == true || Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::R) == true)
  {
    // Reset buzzers
    std::fill(_cooldowns.begin(), _cooldowns.end(), 0.f);
    _buzz = -1;

    // Reset question score
    _current = _score;

    // Play sound if not quiet
    if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::T) == true) {
      auto  ref = Game::Audio::Sound::Instance().get();

      // Play correct answer sound
      ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_timesup.wav"));
      ref.sound.play();

      // Wait sound before next buzz
      std::fill(_cooldowns.begin(), _cooldowns.end(), 1.2f);
    }

    std::cout << "\rNew question for " << _current << " points.        " << std::flush;
  }

  // Lock buzzers
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::L) == true)
  {
    // Infinite wait for every buzzers
    std::fill(_cooldowns.begin(), _cooldowns.end(), std::numeric_limits<float>::infinity());
    _buzz = -1;

    std::cout << "\rBuzzers locked.                                           " << std::flush;
  }

  // Increase cooldown
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Right) == true) {
    _cooldown = std::max(0.f, _cooldown + 0.25f * (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::LShift) == true ? 10.f : 1.f));
    std::cout << "\rCooldown set to " << _cooldown << " seconds.        " << std::flush;
  }

  // Decrease cooldown
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Left) == true) {
    _cooldown = std::max(0.f, _cooldown - 0.25f * (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::LShift) == true ? 10.f : 1.f));
    std::cout << "\rCooldown set to " << _cooldown << " seconds.        " << std::flush;
  }

  // Increase question score
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Up) == true) {
    _score = std::max(1, _score + 1);
    _current = _score;
    std::cout << "\rQuestion score set to " << _score << " points.        " << std::flush;
  }

  // Decrease question score
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Down) == true) {
    _score = std::max(1, _score - 1);
    _current = _score;
    std::cout << "\rQuestion score set to " << _score << " points.        " << std::flush;
  }

  // Return to main menu
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::E) == true) {
    _machine.pop();
    return false;
  }

  // Players can buzz
  if (_buzz == -1) {
    for (auto index = 0; index < _quiz.players.size(); index++)
    {
      // Decrease cooldown
      _cooldowns.at(index) = std::max(_cooldowns.at(index) - elapsed, 0.f);

      // Buzz!
      if (_cooldowns.at(index) <= 0.f && Game::Window::Instance().joystick().buttonPressed(_quiz.players[index].joystick, _quiz.players[index].button + QUIZ::Quiz::Button::ButtonBuzzer) == true) {
        auto  ref = Game::Audio::Sound::Instance().get();
        
        // Play buzzer sound
        ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_buzzer.wav"));
        ref.sound.play();

        // Set buzz index
        _buzz = index;

        // Player cooldown
        for (auto& cooldown : _cooldowns)
          cooldown = (_cooldown == 0.f) ? 0.6f : std::max(0.6f, cooldown);
        _cooldowns.at(_buzz) = 0.f;

        std::cout << "\rPlayer #" << _buzz << " buzzed for " << _current << " points!            " << std::flush;
        break;
      }
    }
  }

  // Update player color
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));

    if (_cooldowns[index] > 0.f)
      entity.setTargetColor(0.25f, 0.25f, 0.25f, 1.f);
    else
      entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
  }

  return false;
}

void  QUIZ::QuestionQuizScene::draw()
{}