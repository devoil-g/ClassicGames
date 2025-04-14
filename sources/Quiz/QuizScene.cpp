#include <SFML/Graphics/Text.hpp>

#include "Quiz/QuizScene.hpp"
#include "Quiz/ControllerQuizScene.hpp"
#include "Quiz/ScoresQuizScene.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Audio/Sound.hpp"
#include "System/Library/FontLibrary.hpp"

QUIZ::QuizScene::QuizScene(Game::SceneMachine& machine) :
  Game::AbstractScene(machine),
  _quiz(),
  _game(),
  _scores(false)
{
  // Start with player selection
  _game.push<QUIZ::ControllerQuizScene>(_quiz);
}

QUIZ::QuizScene::~QuizScene()
{
  // Interrupt playing sounds to avoid reading deleted buffers
  Game::Audio::Sound::Instance().clear();
}

bool  QUIZ::QuizScene::update(float elapsed)
{
  // Toogle score display
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Tab) == true)
    _scores = !_scores;

  // Change scores
  if (Game::Window::Instance().mouse().buttonPressed(Game::Window::MouseButton::Left) == true || Game::Window::Instance().mouse().buttonPressed(Game::Window::MouseButton::Right) == true) {
    for (auto& player : _quiz.players) {
      if (_quiz.entities.at("player_" + std::to_string(player.id)).hover() == true)
        player.score = player.score
        + (Game::Window::Instance().mouse().buttonPressed(Game::Window::MouseButton::Left) == true ? +1 : 0)
        + (Game::Window::Instance().mouse().buttonPressed(Game::Window::MouseButton::Right) == true ? -1 : 0);
    }
  }

  // Update game
  if (_game.update(elapsed) == true) {
    _game.push<QUIZ::ScoresQuizScene>(_quiz);
    return false;
  }

  std::list<std::string> toDelete;

  // Update entities
  for (auto& [name, entity] : _quiz.entities)
    if (entity.update(elapsed) == true)
      toDelete.emplace_front(name);

  // Remove dead entities
  for (const auto& name : toDelete)
    _quiz.entities.erase(name);
  
  // Update progress bar
  _quiz.progress.update(elapsed);

  return false;
}

void  QUIZ::QuizScene::draw()
{
  // Draw entities
  for (auto& [_, entity] : _quiz.entities)
    entity.draw();

  // Draw progress bar
  _quiz.progress.draw();

  // Draw scores
  if (_scores == true && false) {
    for (int index = 0; index < _quiz.players.size(); index++) {
      auto& player = _quiz.players[index];
      auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));
      auto& sprite = entity.sprite();

      sf::Text  score(Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "04b03.ttf"), std::to_string(player.score), 128);

      // Set score outline thickness
      score.setOutlineThickness(5.f);

      // Set size of the score
      float scale = (Game::Window::Instance().getSize().y() * 0.1f) / score.getLocalBounds().size.y;
      score.setScale({ scale, scale });

      // Set score position
      score.setOrigin({ (score.getLocalBounds().size.x + score.getLocalBounds().position.x) / 2.f, (score.getLocalBounds().size.y + score.getLocalBounds().position.y) / 2.f });
      score.setPosition({ sprite.getPosition().x, sprite.getGlobalBounds().position.y + sprite.getGlobalBounds().size.y });

      // Set transparency
      score.setFillColor(sprite.getColor());
      score.setOutlineColor(sf::Color(0, 0, 0, sprite.getColor().a));

      // Draw score
      Game::Window::Instance().draw(score);
    }
  }
  
  // Draw quiz
  _game.draw();
}