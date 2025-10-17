#include <SFML/Graphics/Text.hpp>

#include "Quiz/QuestionQuizScene.hpp"
#include "System/Config.hpp"
#include "Math/Math.hpp"
#include "System/Window.hpp"
#include "System/Audio/Sound.hpp"
#include "System/Library/FontLibrary.hpp"
#include "System/Library/SoundLibrary.hpp"

#include <iostream>

QUIZ::QuestionQuizScene::QuestionQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz/*, QUIZ::Quiz::Question& question*/) :
  Game::AbstractScene(machine),
  _quiz(quiz)
  //_question(question)
{
  // Question box
  auto& questionBox = _quiz.entities["questionBox"];

  questionBox.reset();
  questionBox.setOutline(0.005f);
  questionBox.setText(L"Dans l'episode TOMB RAIDER de Joueur du Grenier,\nArchibald Von Grenier est trahi par Traitropoulos,\nJafar et...");
  questionBox.setColor(1.f, 1.f, 1.f, 0.f);
  questionBox.setTargetColor(1.f, 1.f, 1.f, 1.f);
  questionBox.setPosition(0.25f, 0.1f);
  questionBox.setScale(0.225f, 0.075f);
  questionBox.setTargetScale(0.45f, 0.15f);
  questionBox.setLerp(0.0625f);

  // Answers box
  for (int index = 0; index < 4; index++)
  {
    const std::array<sf::Color, 4>  colors = {
      sf::Color(0x51, 0x8a, 0xc1),
      sf::Color(0xf1, 0x49, 0x02),
      sf::Color(0x3d, 0x9f, 0x05),
      sf::Color(0xeb, 0xde, 0x0a)
    };

    auto& answerBox = _quiz.entities["answerBox" + std::to_string(index)];

    answerBox.reset();
    answerBox.setOutline(0.005f);
    answerBox.setColor(colors[index].r / 255.f, colors[index].g / 255.f, colors[index].b / 255.f, 0.f);
    answerBox.setTargetColor(colors[index].r / 255.f, colors[index].g / 255.f, colors[index].b / 255.f, 1.f);
    answerBox.setPosition(0.25f, 0.2f + (index + 0.5f) * 0.15f);
    answerBox.setScale(0.2f, 0.0625f);
    answerBox.setTargetScale(0.4f, 0.125f);
    answerBox.setLerp(0.0625f);

    auto& answerText = _quiz.entities["answerText" + std::to_string(index)];

    answerText.reset();
    answerText.setText(L"Reponse " + std::to_wstring(index));
    answerText.setColor(colors[index].r / 255.f, colors[index].g / 255.f, colors[index].b / 255.f, 0.f);
    answerText.setTargetColor(colors[index].r / 255.f, colors[index].g / 255.f, colors[index].b / 255.f, 1.f);
    answerText.setPosition(0.25f, 0.2f + (index + 0.5f) * 0.15f);
    answerText.setScale(0.2f, 0.0625f);
    answerText.setTargetScale(0.4f, 0.075f);
    answerText.setLerp(0.0625f);
  }


  // Set player visible
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));

    entity.setTargetPosition((index + 1.f) / (_quiz.players.size() + 1.f) / 2.f, 0.9f);
    entity.setTargetScale(0.45f / (_quiz.players.size() + 1.f), 0.15f);{}
    entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
    entity.setLerp(0.0625f);
  }
}

bool  QUIZ::QuestionQuizScene::update(float elapsed)
{
  return false;
}

void  QUIZ::QuestionQuizScene::draw()
{}
