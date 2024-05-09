#include <iostream>

#include <SFML/Graphics/Text.hpp>

#include "Quiz/QuizScene.hpp"
#include "Quiz/ControllerQuizScene.hpp"
#include "Quiz/ScoresQuizScene.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Audio/Sound.hpp"
#include "System/Library/FontLibrary.hpp"

const float QUIZ::QuizScene::ForcedExit = 1.f;

QUIZ::QuizScene::QuizScene(Game::SceneMachine& machine) :
  Game::AbstractScene(machine),
  _quiz(),
  _game(),
  _scores(false),
  _elapsed(0.f),
  _bar(sf::Vector2f(1.f, 1.f))
{
  // Initialize force exit bar
  _bar.setSize(sf::Vector2f(1.f, 1.f));
  _bar.setFillColor(sf::Color::White);
}

QUIZ::QuizScene::~QuizScene()
{
  // Interrupt playing sounds to avoid reading deleted buffers
  Game::Audio::Sound::Instance().clear();
}

bool  QUIZ::QuizScene::update(float elapsed)
{
  // Update exit timer
  _elapsed += elapsed;

  // Reset timer when ESC is not pressed
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Escape) == false)
    _elapsed = 0.f;

  // Toogle score display
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Tab) == true)
    _scores = !_scores;

  // Exit if limit reached
  if (_elapsed > QUIZ::QuizScene::ForcedExit) {
    _machine.pop();
    return false;
  }

  // Change scores
  if (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Left) == true || Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Right) == true) {
    for (auto& player : _quiz.players) {
      if (_quiz.entities.at("player_" + std::to_string(player.id)).hover() == true)
        player.score = player.score
        + (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Left) == true ? +1 : 0)
        + (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Right) == true ? -1 : 0);
    }
  }

  // Update game
  if (_game.update(elapsed) == true) {
    _game.push<QUIZ::ScoresQuizScene>(_quiz);
    return false;
  }

  // Update scores
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));
    auto& sprite = entity.sprite();

  // Update entities
  for (auto& [_, entity] : _quiz.entities)
    entity.update(elapsed);

  return false;
}

void  QUIZ::QuizScene::draw()
{
  // Draw entities
  for (auto& [_, entity] : _quiz.entities)
    entity.draw();

  // Draw scores
  if (_scores == true) {
    for (int index = 0; index < _quiz.players.size(); index++) {
      auto& player = _quiz.players[index];
      auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));
      auto& sprite = entity.sprite();

      sf::Text  score(std::to_string(player.score), Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "04b03.ttf"), 128);

      // Set score outline thickness
      score.setOutlineThickness(5.f);

      // Set size of the score
      float scale = (Game::Window::Instance().window().getSize().y * 0.1f) / score.getLocalBounds().height;
      score.setScale(scale, scale);

      // Set score position
      score.setOrigin((score.getLocalBounds().width + score.getLocalBounds().left) / 2.f, (score.getLocalBounds().height + score.getLocalBounds().top) / 2.f);
      score.setPosition(sprite.getPosition().x, sprite.getGlobalBounds().top + sprite.getGlobalBounds().height);

      // Set transparency
      score.setFillColor(sprite.getColor());
      score.setOutlineColor(sf::Color(0, 0, 0, sprite.getColor().a));

      // Draw score
      Game::Window::Instance().window().draw(score);
    }
  }
  
  // Draw quiz
  _game.draw();
  
  // Draw forced exit bar
  _bar.setScale(Game::Window::Instance().window().getSize().x * _elapsed / QUIZ::QuizScene::ForcedExit, 4.f);
  Game::Window::Instance().window().draw(_bar);
}