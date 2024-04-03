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
  _music(),
  _display(true)
{
  // Load music
  if (_music.openFromFile((Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "scores.ogg").string()) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Set music parameters
  _music.setLoop(true);
  _music.setVolume(50.f);
}

bool  QUIZ::ScoresQuizScene::update(float elapsed)
{
  // Resume score display
  if (_music.getStatus() == sf::Music::Stopped) {
    _music.play();

    // Host instructions
    std::cout
      << "--- SCOREBOARD ---" << std::endl
      << "Instruction for players: nothing" << std::endl
      << std::endl
      << "Commands:" << std::endl
      << "  [S]core:      toogle score display" << std::endl
      << "  [C]ontroller: controller selection" << std::endl
      << "  [Q]uestion:   free questions system" << std::endl;
    if (_quiz.blindtests.empty() == false)
      std::cout
      << "  [B]lintest:   play blindtest (" << std::count_if(_quiz.blindtests.begin(), _quiz.blindtests.end(), [](const auto& entry) { return entry.done == false; }) << " remaining)" << std::endl;
  }

  // Controller selection
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::C) == true) {
    _machine.push<QUIZ::ControllerQuizScene>(_quiz);
    _music.stop();
    return false;
  }

  // Free question
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Q) == true) {
    _machine.push<QUIZ::QuestionQuizScene>(_quiz);
    _music.stop();
    return false;
  }

  // Start blindtest
  if (_quiz.blindtests.empty() == false && Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::B) == true) {
    _machine.push<QUIZ::BlindtestQuizScene>(_quiz);
    _music.stop();
    return false;
  }

  // Toggle score display
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::S) == true)
    _display = !_display;

  // Change scores
  if (_display == true && (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Left) == true || Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Right) == true)) {
    for (auto& player : _quiz.players) {
      if (player.sprite.getGlobalBounds().contains((float)Game::Window::Instance().mouse().position().x, (float)Game::Window::Instance().mouse().position().y) == true)
        player.score = player.score
        + (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Left) == true ? +1 : 0)
        + (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Right) == true ? -1 : 0);
    }
  }

  return false;
}

void  QUIZ::ScoresQuizScene::draw()
{
  auto  screen = Game::Window::Instance().window().getSize();
  
  for (unsigned int player_index = 0; player_index < _quiz.players.size(); player_index++) {
    sf::Sprite& sprite = _quiz.players[player_index].sprite;

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

    // Set default color
    sprite.setColor(sf::Color::White);

    // Draw sprite
    Game::Window::Instance().window().draw(sprite);

    // Skip score display
    if (_display == false)
      continue;

    sf::Text  score(std::to_string(_quiz.players[player_index].score), Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "pixelated.ttf"), 128);

    // Set score outline thickness
    score.setOutlineThickness(5.f);

    // Set size of the score
    scale = (screen.y * 0.1f) / score.getLocalBounds().height;
    score.setScale(scale, scale);

    // Set score position
    score.setOrigin(score.getLocalBounds().width / 2.f, score.getLocalBounds().height / 2.f);
    score.setPosition(x_position, sprite.getGlobalBounds().top + sprite.getGlobalBounds().height);

    // Draw score
    Game::Window::Instance().window().draw(score);
  }
}