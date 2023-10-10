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
  _bar(sf::Vector2f(1.f, 1.f)),
  _ping(0)
{
  // Load music
  if (_music.openFromFile(Game::Config::ExecutablePath + "/assets/quiz/musics/contestants.ogg") == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Check minimum duration
  if (_music.getDuration().asSeconds() < TimerLimit + TimerOver)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Play music
  _music.play();

  // Initialize timer bar
  _bar.setSize(sf::Vector2f(1.f, 1.f));
  _bar.setFillColor(sf::Color::White);

  // Host instructions
  std::cout
    << "--- PLAYER SELECTION ---" << std::endl
    << "Instruction for players: use the red buzzer to join the game," << std::endl
    << "  blue and orange to select your avatar," << std::endl
    << "  yellow to quit" << std::endl
    << std::endl
    << "Commands:" << std::endl
    << "  [R]eset:  reset timer" << std::endl
    << "  [S]kip:   skip timer to end" << std::endl
    << "  [E]nd:    end player selection" << std::endl
    << std::endl;
}

bool  QUIZ::ControllerQuizScene::update(sf::Time elapsed)
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
            .avatar = avatar
            });

          auto  ref = Game::Audio::Sound::Instance().get();

          // Play ping sound
          ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath + "/assets/quiz/sounds/ping" + std::to_string(_ping++ % 4 + 1) + ".wav"));
          ref.sound.play();
        }
      }
    }
  }
}

void  QUIZ::ControllerQuizScene::updateUnregister()
{
  // Inspect each player
  for (auto iterator = _quiz.players.begin(); iterator != _quiz.players.end();)
  {
    // Remove player when yellow is pressed
    if (Game::Window::Instance().joystick().buttonPressed(iterator->joystick, iterator->button + QUIZ::Quiz::Button::ButtonYellow) == true)
      iterator = _quiz.players.erase(iterator);
    else
      iterator++;
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
          break;
        }
      }
    }
  }
}

void  QUIZ::ControllerQuizScene::updateHost()
{
  // Restart timer
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::R) == true) {
    _music.setPlayingOffset(sf::seconds(TimerStart));
    //_music.play();
  }

  // Skip music to end
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::S) == true)
    _music.setPlayingOffset(sf::seconds(QUIZ::ControllerQuizScene::TimerLimit));

  // End player select
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::E) == true)
    _machine.pop();
}

void  QUIZ::ControllerQuizScene::draw()
{
  // Draw nothing if timer ended
  if (_music.getStatus() != sf::Music::Status::Playing || _music.getPlayingOffset().asSeconds() >= TimerLimit + TimerOver)
    return;

  // Draw players
  drawPlayers();

  // Draw timer bar
  drawTimer();
}

int QUIZ::ControllerQuizScene::drawPlayersGrid()
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

void  QUIZ::ControllerQuizScene::drawPlayers()
{
  auto  screen = Game::Window::Instance().window().getSize();
  float x_ratio = 1.f / (1.5f * _quiz.players.size() + 0.5f);
  float alpha = std::clamp(1.f - (_music.getPlayingOffset().asSeconds() - TimerLimit) / TimerOver, 0.f, 1.f);

  int   y_grid = drawPlayersGrid();
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

    // Transparency
    sprite.setColor(sf::Color(255, 255, 255, (std::uint8_t)(255 * alpha)));

    // Draw sprite
    Game::Window::Instance().window().draw(sprite);
  }
}

void  QUIZ::ControllerQuizScene::drawTimer()
{
  float completion = std::clamp(_music.getPlayingOffset().asSeconds() / TimerLimit, 0.f, 1.f);
  float over = std::clamp((_music.getPlayingOffset().asSeconds() - TimerLimit) / TimerOver, 0.f, 1.f);

  // Draw timer bar
  _bar.setScale(Game::Window::Instance().window().getSize().x * completion, (1.f - over) * 16.f);
  _bar.setPosition(0.f, Game::Window::Instance().window().getSize().y - _bar.getScale().y);
  Game::Window::Instance().window().draw(_bar);
}