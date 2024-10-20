#include "Math/Math.hpp"
#include "Quiz/ControllerQuizScene.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Audio/Sound.hpp"
#include "System/Library/SoundLibrary.hpp"

#include <iostream>

const float QUIZ::ControllerQuizScene::TimerStart = 0.184f;
const float QUIZ::ControllerQuizScene::TimerLimit = 29.8f;
const float QUIZ::ControllerQuizScene::TimerOver = 3.5f;

QUIZ::ControllerQuizScene::ControllerQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz) :
  Game::AbstractScene(machine),
  _quiz(quiz),
  _music(),
  _bar(sf::Vector2f(1.f, 1.f))
{
  // Load music
  if (_music.openFromFile((Game::Config::ExecutablePath / "assets" / "quiz" / "musics" / "contestants.ogg").string()) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Check minimum duration
  if (_music.getDuration().asSeconds() < TimerLimit + TimerOver)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Play music
  _music.play();

  // Initialize timer bar
  _bar.setSize(sf::Vector2f(1.f, 1.f));
  _bar.setFillColor(sf::Color::White);

  // Set player visible
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));

    entity.setTargetPosition((index + 1.f) / (_quiz.players.size() + 1.f), 0.5f);
    entity.setTargetScale(0.9f / (_quiz.players.size() + 1.f), 0.75f);
    entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
    entity.setLerp(0.0625f);
  }

  // Host instructions
  std::cout
    << "--- PLAYER SELECTION ---" << std::endl
    << "Instruction for players: use the red buzzer to join the game," << std::endl
    << "  blue and orange to select your avatar," << std::endl
    << "  green and yellow to change the skin" << std::endl
    << std::endl
    << "Commands:" << std::endl
    << "  [R]eset:  reset timer" << std::endl
    << "  [S]kip:   skip timer to end" << std::endl
    << "  [E]nd:    end controller selection" << std::endl
    << std::endl;
}

bool  QUIZ::ControllerQuizScene::update(float elapsed)
{
  // Manage player inputs
  if (_music.getStatus() == sf::Music::Status::Playing && _music.getPlayingOffset().asSeconds() < TimerLimit) {
    updateRegister();
    updateUnregister();
    updateAvatar();
  }

  // End scene
  if (_music.getStatus() == sf::Music::Stopped)
    _machine.pop();

  // Manage host inputs
  updateHost();

  return false;
}

void  QUIZ::ControllerQuizScene::updateRegister()
{
  // Check for new player
  for (unsigned int joystick = 0; joystick < sf::Joystick::Count; joystick++) {
    if (sf::Joystick::isConnected(joystick) == true) {
      for (unsigned int button = 0; button < 20; button += 5) {
        if (Game::Window::Instance().joystick().buttonPressed(joystick, button) == true)
        {
          // Check if joystick/button is already used
          if (std::find_if(_quiz.players.begin(), _quiz.players.end(), [joystick, button](const auto& player) { return player.joystick == joystick && player.button == button; }) != _quiz.players.end())
            continue;

          unsigned int avatar = (unsigned int)-1;

          // Find first available avatar
          for (int index = 0; index < _quiz.avatars.size(); index++)
          {
            // Check registered player avatar index
            if (std::find_if(_quiz.players.begin(), _quiz.players.end(), [index](const auto& player) { return player.avatar == index; }) != _quiz.players.end())
              continue;

            // Use this index
            avatar = index;
            break;
          }

          // Skip, no more avatar availables
          if (avatar == (unsigned int)-1)
            continue;

          // Register new player
          _quiz.players.push_back({
            .joystick = joystick,
            .button = button,
            .avatar = avatar,
            .skin = 0,
            .score = 0,
            .id = joystick * 20 + button
            });

          // Add player entity
          auto& player = _quiz.entities.emplace(std::make_pair("player_" + std::to_string(_quiz.players.back().id), _quiz.avatars[_quiz.players.back().avatar][_quiz.players.back().skin])).first->second;
          player.setPosition((float)_quiz.players.size() / (_quiz.players.size() + 1.f), 0.5f);
          player.setScale(0.f, 0.f);
          player.setColor(0.f, 0.f, 0.f, 0.f);
          player.setLerp(0.0625f);

          // Move every player to fit new player
          for (int index = 0; index < _quiz.players.size(); index++) {
            auto& entity = _quiz.entities.at("player_" + std::to_string(_quiz.players[index].id));

            entity.setTargetPosition((index + 1.f) / (_quiz.players.size() + 1.f), 0.5f);
            entity.setTargetScale(0.9f / (_quiz.players.size() + 1.f), 0.75f);
            entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
          }

          auto  ref = Game::Audio::Sound::Instance().get();

          // Play ping sound
          static int ping = 0;
          ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / ("ping" + std::to_string(ping++ % 4 + 1) + ".wav")));
          ref.sound.play();
        }
      }
    }
  }
}

void  QUIZ::ControllerQuizScene::updateUnregister()
{
  // Inspect each player
  if (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Button::Right) == true) {
    for (auto iterator = _quiz.players.begin(); iterator != _quiz.players.end();)
    {
      // Remove player when sprite is clicked
      if (_quiz.entities.at("player_" + std::to_string(iterator->id)).hover() == true) {
        _quiz.entities.erase("player_" + std::to_string(iterator->id));
        iterator = _quiz.players.erase(iterator);

        // Move every player to fit screen
        for (int index = 0; index < _quiz.players.size(); index++) {
          auto& entity = _quiz.entities.at("player_" + std::to_string(_quiz.players[index].id));

          entity.setTargetPosition((index + 1.f) / (_quiz.players.size() + 1.f), 0.5f);
          entity.setTargetScale(0.9f / (_quiz.players.size() + 1.f), 0.75f);
          entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
        }
      }
      else
        iterator++;
    }
  }
}

void  QUIZ::ControllerQuizScene::updateAvatar()
{
  // Inspect each player
  for (auto& player : _quiz.players)
  {
    // Next avatar
    if (Game::Window::Instance().joystick().buttonPressed(player.joystick, player.button + QUIZ::Quiz::Button::ButtonBlue) == true) {
      for (unsigned int avatar = (player.avatar + 1) % _quiz.avatars.size(); avatar != player.avatar; avatar = (avatar + 1) % _quiz.avatars.size()) {
        if (std::find_if(_quiz.players.begin(), _quiz.players.end(), [avatar](const auto& player) { return player.avatar == avatar; }) == _quiz.players.end())
        {
          // Register new avatar
          player.avatar = avatar;
          player.skin = 0;
          _quiz.entities.at("player_" + std::to_string(player.id)).setTexture(_quiz.avatars[player.avatar][player.skin]);
          break;
        }
      }
    }

    // Previous avatar
    if (Game::Window::Instance().joystick().buttonPressed(player.joystick, player.button + QUIZ::Quiz::Button::ButtonOrange) == true) {
      for (unsigned int avatar = (player.avatar + (unsigned int)_quiz.avatars.size() - 1) % _quiz.avatars.size(); avatar != player.avatar; avatar = (avatar + (unsigned int)_quiz.avatars.size() - 1) % _quiz.avatars.size()) {
        if (std::find_if(_quiz.players.begin(), _quiz.players.end(), [avatar](const auto& player) { return player.avatar == avatar; }) == _quiz.players.end())
        {
          // Register new avatar
          player.avatar = avatar;
          player.skin = 0;
          _quiz.entities.at("player_" + std::to_string(player.id)).setTexture(_quiz.avatars[player.avatar][player.skin]);
          break;
        }
      }
    }

    // Next costume
    if (Game::Window::Instance().joystick().buttonPressed(player.joystick, player.button + QUIZ::Quiz::Button::ButtonGreen) == true) {
      player.skin = Math::Modulo((int)player.skin + 1, (int)_quiz.avatars[player.avatar].size());
      _quiz.entities.at("player_" + std::to_string(player.id)).setTexture(_quiz.avatars[player.avatar][player.skin]);
    }

    // Previous costume
    if (Game::Window::Instance().joystick().buttonPressed(player.joystick, player.button + QUIZ::Quiz::Button::ButtonYellow) == true) {
      player.skin = Math::Modulo((int)player.skin - 1, (int)_quiz.avatars[player.avatar].size());
      _quiz.entities.at("player_" + std::to_string(player.id)).setTexture(_quiz.avatars[player.avatar][player.skin]);
    }
  }
}

void  QUIZ::ControllerQuizScene::updateHost()
{
  // Restart timer
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::R) == true)
    _music.setPlayingOffset(sf::seconds(TimerStart));

  // Skip music to end
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::S) == true)
    _music.setPlayingOffset(sf::seconds(QUIZ::ControllerQuizScene::TimerLimit));

  // End player select
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::E) == true)
    _machine.pop();
}

void  QUIZ::ControllerQuizScene::draw()
{
  float completion = std::clamp(_music.getPlayingOffset().asSeconds() / TimerLimit, 0.f, 1.f);
  float over = std::clamp((_music.getPlayingOffset().asSeconds() - TimerLimit) / TimerOver, 0.f, 1.f);

  // Draw timer bar
  _bar.setScale(Game::Window::Instance().window().getSize().x * completion, (1.f - over) * 16.f);
  _bar.setPosition(0.f, Game::Window::Instance().window().getSize().y - _bar.getScale().y);
  Game::Window::Instance().window().draw(_bar);
}