#include <SFML/Graphics/Text.hpp>

#include "Quiz/ScoresQuizScene.hpp"
#include "Quiz/BlindtestQuizScene.hpp"
#include "Quiz/ControllerQuizScene.hpp"
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
  if (_music.openFromFile(Game::Config::ExecutablePath + "/assets/quiz/musics/scores.ogg") == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Set music parameters
  _music.setLoop(true);
  _music.setVolume(50.f);
}

bool  QUIZ::ScoresQuizScene::update(sf::Time elapsed)
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
      << "  [C]ontroller: controller selection" << std::endl;
    if (_quiz.blindtests.empty() == false)
      std::cout
      << "  [B]lintest:   play blindtest (" << _quiz.blindtests.size() << " remaining)" << std::endl;
  }

  // Toggle score display
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::S) == true)
    _display = !_display;

  // Controller selection
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::C) == true) {
    _machine.push<QUIZ::ControllerQuizScene>(_quiz);
    _music.stop();
  }

  // Start blindtest
  else if (_quiz.blindtests.empty() == false && Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::B) == true) {
    _machine.push<QUIZ::BlindtestQuizScene>(_quiz);
    _music.stop();
  }

  return false;
}

void  QUIZ::ScoresQuizScene::draw()
{
  auto  screen = Game::Window::Instance().window().getSize();
  float x_ratio = 1.f / (1.5f * _quiz.players.size() + 0.5f);

  int   y_grid = drawGrid();
  int   x_grid = (int)std::ceil((float)_quiz.players.size() / (float)y_grid);
  float s_grid = std::min((float)screen.x / (x_grid + 1.f), (float)screen.y / (y_grid + 1.f));

  sf::Vector2f  offset_grid = {
    (screen.x - (x_grid + 1) * s_grid) / 2.f + s_grid / 2.f,
    (screen.y - (y_grid + 1) * s_grid) / 2.f + s_grid / 2.f
  };

  for (unsigned int player_index = 0; player_index < _quiz.players.size(); player_index++) {
    sf::Sprite    sprite;

    sprite.setTexture(_quiz.avatars[_quiz.players[player_index].avatar], true);

    // Set sprite size
    float scale = std::min(
      s_grid / (float)sprite.getLocalBounds().width,
      s_grid / (float)sprite.getLocalBounds().height
    ) * 0.85f;
    sprite.setScale(scale, scale);

    // Set sprite position
    sprite.setPosition(
      offset_grid.x + (player_index % x_grid) * s_grid + (s_grid - sprite.getGlobalBounds().width) / 2.f,
      offset_grid.y + (player_index / x_grid) * s_grid + (s_grid - sprite.getGlobalBounds().height) / 2.f
    );
    
    // Draw sprite
    Game::Window::Instance().window().draw(sprite);

    // Skip score display
    if (_display == false)
      continue;

    sf::Text  score(std::to_string(_quiz.players[player_index].score), Game::FontLibrary::Instance().get(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf"), 128);

    // Set score outline thickness
    score.setOutlineThickness(5.f);

    // Set size of the score
    scale = std::min(
      s_grid / (float)score.getLocalBounds().width,
      s_grid / (float)score.getLocalBounds().height
    ) * 0.25f;
    score.setScale(scale, scale);

    // Set score position
    score.setPosition(
      offset_grid.x + (player_index % x_grid) * s_grid + (s_grid - score.getGlobalBounds().width) / 2.f,
      offset_grid.y + (player_index / x_grid) * s_grid + (s_grid - score.getGlobalBounds().height) * 3.f / 4.f
    );

    // Draw score
    Game::Window::Instance().window().draw(score);
  }
}

int QUIZ::ScoresQuizScene::drawGrid() const
{
  float screen_ratio = (float)Game::Window::Instance().window().getSize().x / (float)Game::Window::Instance().window().getSize().y;
  int   best_line = 1;
  float best_ratio = ((float)_quiz.players.size() + 1.f) / 2.f;

  // Check each possible grid size
  for (int line = 2; line <= _quiz.players.size(); line++) {
    float ratio = std::ceil((float)_quiz.players.size() / (float)line + 1.f) / (float)(line + 1.f);

    // Check new ratio
    if (std::abs(ratio - screen_ratio) < std::abs(best_ratio - screen_ratio)) {
      best_line = line;
      best_ratio = ratio;
    }
  }

  return best_line;
}