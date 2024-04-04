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
  _display(true),
  _score((int)_quiz.players.size()),
  _current((int)_quiz.players.size()),
  _buzz(-1),
  _cooldowns(_quiz.players.size(), 0.f),
  _cooldown(10.f)
{
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
    << "  [H]elp:       display this usage" << std::endl
    << "  [S]core:      toogle score display" << std::endl
    << "  [W]rong:      wrong answer, release buzzers" << std::endl
    << "  [C]orrect:    correct answer, reset for new question" << std::endl
    << "  [N]ext:       no answers, skip to next question" << std::endl
    << "  Arrow[L/R]:   set cooldown" << std::endl
    << "  Arrow[U/D]:   change question score" << std::endl
    << "  Left click:   increase player score" << std::endl
    << "  Right click:  decrease player score" << std::endl
    << "  [E]nd:        return to main menu" << std::endl
    << std::endl;
}

bool  QUIZ::QuestionQuizScene::update(float elapsed)
{
  // Toggle score display
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::S) == true)
    _display = !_display;

  // Display usage
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::H) == true)
    usage();

  // Wrong answer
  if (_buzz != -1 && Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::W) == true) {
    auto  ref = Game::Audio::Sound::Instance().get();

    // Play wrong answer sound
    ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_wrong.wav"));
    ref.sound.play();

    // Set player cooldown
    for (auto& cooldown : _cooldowns)
      cooldown = std::max(cooldown, 0.6f);
    _cooldowns.at(_buzz) = _cooldown + 0.6f;
    _buzz = -1;

    // Decrease question score
    _current = std::max(0, _current - 1);
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
    std::fill(_cooldowns.begin(), _cooldowns.end(), 0.6f);
    _cooldowns.at(_buzz) = 0.f;
    _buzz = -1;

    // Reset question score
    _current = _score;
  }

  // Skip to next question
  if (_buzz == -1 && Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::N) == true) {
    auto  ref = Game::Audio::Sound::Instance().get();

    // Play correct answer sound
    ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_timesup.wav"));
    ref.sound.play();

    // Reset players cooldown
    _buzz = -1;
    std::fill(_cooldowns.begin(), _cooldowns.end(), 1.2f);

    // Reset question score
    _current = _score;

    std::cout << "\rNew question for " << _current << " points.        " << std::flush;
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
    _score = std::max(0, _score + 1);
    _current = _score;
    std::cout << "\rQuestion score set to " << _score << " points.        " << std::flush;
  }

  // Decrease question score
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Down) == true) {
    _score = std::max(0, _score - 1);
    _current = _score;
    std::cout << "\rQuestion score set to " << _score << " points.        " << std::flush;
  }

  // Return to main menu
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::E) == true) {
    _machine.pop();
    return false;
  }

  // Change scores
  if (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Left) == true || Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Right) == true) {
    for (auto player_index = 0; player_index < _quiz.players.size(); player_index++) {
      auto& player = _quiz.players.at(player_index);
      if (player.sprite.getGlobalBounds().contains((float)Game::Window::Instance().mouse().position().x, (float)Game::Window::Instance().mouse().position().y) == true) {
        if (_display == true)
          player.score = player.score
          + (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Left) == true ? +1 : 0)
          + (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Right) == true ? -1 : 0);
        std::cout << "\rScore of player #" << (player_index + 1) << ": " << player.score << ".        " << std::flush;
      }
    }
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

        std::cout << "\rPlayer #" << _buzz << " buzzed for " << _current << " points!            " << std::flush;

        break;
      }
    }
  }

  return false;
}

void  QUIZ::QuestionQuizScene::draw()
{
  auto  screen = Game::Window::Instance().window().getSize();
 
  for (unsigned int player_index = 0; player_index < _quiz.players.size(); player_index++) {
    auto&       player = _quiz.players[player_index];
    sf::Sprite& sprite = player.sprite;

    float x_position = screen.x / (_quiz.players.size() + 1.f) * (player_index + 1.f);
    float y_position = screen.y / 2.f;

    // Set sprite position
    sprite.setPosition(x_position, y_position);
    sprite.setOrigin(sprite.getLocalBounds().width / 2.f, sprite.getLocalBounds().height / 2.f);

    float x_scale = (screen.x * 0.9f / (_quiz.players.size() + 1.f)) / sprite.getLocalBounds().width;
    float y_scale = (screen.y * 0.75f) / sprite.getLocalBounds().height;
    float scale = std::min(x_scale, y_scale);

    // Set sprite size
    sprite.setScale(scale, scale);

    std::uint8_t  alpha = 255;

    // Cooldown transparency
    if (_buzz == -1 && _cooldowns.at(player_index) > 0.f)
      alpha = (int)(std::pow(std::clamp(1.f / (_cooldowns.at(player_index) + 1.f), 0.f, 1.f), 20) * 192.f) + 63;
    else if (_buzz != -1 && _buzz != player_index)
      alpha = 63;

    // Set transparency
    sprite.setColor(sf::Color(255, 255, 255, alpha));

    // Draw sprite
    Game::Window::Instance().window().draw(sprite);

    // Skip score display
    if (_display == false)
      continue;

    sf::Text  score(std::to_string(player.score), Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "pixelated.ttf"), 128);

    // Set score outline thickness
    score.setOutlineThickness(5.f);

    // Set size of the score
    scale = (screen.y * 0.1f) / score.getLocalBounds().height;
    score.setScale(scale, scale);

    // Set score position
    score.setOrigin(score.getLocalBounds().width / 2.f, score.getLocalBounds().height / 2.f);
    score.setPosition(x_position, sprite.getGlobalBounds().top + sprite.getGlobalBounds().height);

    // Set transparency
    score.setFillColor(sf::Color(255, 255, 255, alpha));

    // Draw score
    Game::Window::Instance().window().draw(score);
  }
}