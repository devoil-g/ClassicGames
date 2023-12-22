#include "Math/Math.hpp"
#include "Quiz/BlindtestQuizScene.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Audio/Sound.hpp"
#include "System/Library/SoundLibrary.hpp"

#include <iostream>

#include <SFML/Graphics/CircleShape.hpp>

QUIZ::BlindtestQuizScene::BlindtestQuizScene(Game::SceneMachine& machine, QUIZ::Quiz& quiz) :
  Game::AbstractScene(machine),
  _quiz(quiz),
  _music(),
  _cover(),
  _play(),
  _pause(),
  _bar(sf::Vector2f(1.f, 1.f)),
  _state(StatePlaying),
  _player(-1)
{
  // No more blindtests!
  if (_quiz.blindtests.empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Select random blindtest
  auto  iterator = std::next(_quiz.blindtests.begin(), std::rand() % _quiz.blindtests.size());
  
  // Saved question infos
  _blindtest = *iterator;

  // Remove question from blindtests
  _quiz.blindtests.erase(iterator);

  // Load music
  if (_music.openFromFile(Game::Config::ExecutablePath.string() + "/" + _blindtest.music) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Load texture
  if (_cover.loadFromFile(Game::Config::ExecutablePath.string() + "/" + _blindtest.cover) == false ||
    _play.loadFromFile((Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "play.png").string()) == false ||
    _pause.loadFromFile((Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "pause.png").string()) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Smooth textures
  _cover.setSmooth(true);
  _play.setSmooth(true);
  _pause.setSmooth(true);

  // Initialize timer bar
  _bar.setSize(sf::Vector2f(1.f, 1.f));
  _bar.setFillColor(sf::Color::White);

  // Host text
  std::cout << "--- BLINDTEST ---" << std::endl
    << "Instruction for players: use the red buzzer to answer" << std::endl
    << std::endl;

  // Start game
  setPlaying();

  // Set default volume to 66%
  _music.setVolume(66.6f);

  // Set initial cooldown to 0.1s
  for (int index = 0; index < _quiz.players.size(); index++)
    _cooldown[index] = 0.1f;
}

void  QUIZ::BlindtestQuizScene::setPlaying()
{
  _state = StatePlaying;
  _player = -1;
  std::cout
    << "Ready to play music" << std::endl
    << std::endl
    << "Commands:" << std::endl
    << "  [P]lay/pause: pause/resume music" << std::endl
    << "  [R]eset:      replay music from the begining" << std::endl
    << "  [+/-]:        control volume" << std::endl
    << "  [A]nswer:     display answer, no winner" << std::endl
    << "  [E]nd:        end blindtest" << std::endl
    << std::endl
    << "Answer: " << _blindtest.answer << std::endl
    << "Info:   " << _blindtest.info << std::endl
    << "Score:  " << _blindtest.score << std::endl
    << std::endl;
}

void  QUIZ::BlindtestQuizScene::setPending(int player)
{
  _music.pause();
  _state = StatePending;
  _player = player;
  std::cout
    << "Player " << _player << " has buzzed!" << std::endl
    << std::endl
    << "Commands:" << std::endl
    << "  [C]orrect:  correct answer, give points, display answer" << std::endl
    << "  [W]rong:    wrong answer, return to game" << std::endl
    << "  [+/-]:      control volume" << std::endl
    << "  [E]nd:      end blindtest" << std::endl
    << std::endl
    << "Answer: " << _blindtest.answer << std::endl
    << "Info:   " << _blindtest.info << std::endl
    << "Score:  " << _blindtest.score << std::endl
    << std::endl;
}

void  QUIZ::BlindtestQuizScene::setAnswer()
{
  if (_player != -1)
    _quiz.players[_player].score += _blindtest.score;

  _state = StateAnswer;
  std::cout
    << "Displaying answer." << std::endl
    << std::endl
    << "Commands:" << std::endl
    << "  [P]ause/play: pause/resume music" << std::endl
    << "  [R]eset:      replay music from the begining" << std::endl
    << "  [+/-]:        control volume" << std::endl;
  if (_quiz.blindtests.empty() == false)
    std::cout << "  [N]ext:       next blindtest (" << _quiz.blindtests.size() << " remaining)" << std::endl;
  std::cout
    << "  [E]nd:        end blindtest" << std::endl
    << std::endl
    << "Answer: " << _blindtest.answer << std::endl
    << "Info:   " << _blindtest.info << std::endl
    << "Score:  " << _blindtest.score << std::endl
    << std::endl;
}

bool  QUIZ::BlindtestQuizScene::update(float elapsed)
{
  // Change volume
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::PageUp))
    _music.setVolume(std::clamp(_music.getVolume() + elapsed * 84.f, 0.f, 200.f));
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::PageDown))
    _music.setVolume(std::clamp(_music.getVolume() - elapsed * 84.f, 0.f, 200.f));

  switch (_state)
  {
  case QUIZ::BlindtestQuizScene::StatePlaying:
    updatePlaying(elapsed);
    break;
  case QUIZ::BlindtestQuizScene::StatePending:
    updatePending(elapsed);
    break;
  case QUIZ::BlindtestQuizScene::StateAnswer:
    updateAnswer(elapsed);
    break;
  default:
    break;
  }

  return false;
}

void  QUIZ::BlindtestQuizScene::updatePlaying(float elapsed)
{
  // Update player cooldown
  if (_music.getStatus() == sf::Sound::Playing) {
    for (int index = 0; index < _quiz.players.size(); index++)
      _cooldown[index] -= elapsed;
  }

  // Pause/resume music
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::P) == true) {
    if (_music.getStatus() == sf::Sound::Playing)
      _music.pause();
    else
      _music.play();
  }

  // Reset music
  else if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::R) == true) {
    _music.setPlayingOffset(sf::Time::Zero);
    _music.play();
  }

  // Skip to answer
  else if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::A) == true)
    setAnswer();

  // End blindtest
  else if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::E) == true)
    _machine.pop();

  // Check player buzzer
  else if (_music.getStatus() == sf::Sound::Playing) {
    for (int index = 0; index < _quiz.players.size(); index++) {
      if (_cooldown[index] <= 0.f && Game::Window::Instance().joystick().buttonPressed(_quiz.players[index].joystick, _quiz.players[index].button + QUIZ::Quiz::Button::ButtonBuzzer) == true) {
        auto  ref = Game::Audio::Sound::Instance().get();

        // Play buzzer sound
        ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "buzzer1.wav"));
        ref.sound.play();

        setPending(index);
        break;
      }
    }
  }

}

void  QUIZ::BlindtestQuizScene::updatePending(float elapsed)
{
  // Correct answer
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::C) == true) {
    auto  ref = Game::Audio::Sound::Instance().get();

    // Play correct answer sound
    ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "correct1.wav"));
    ref.sound.play();

    // Resume music at reduced volume
    _music.setVolume(_music.getVolume() * 0.66f);
    _music.play();

    setAnswer();
  }

  // Wrong answer
  else if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::W) == true) {
    auto  ref = Game::Audio::Sound::Instance().get();

    // Play wrong answer sound
    ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "wrong1.wav"));
    ref.sound.play();

    // Set player cooldown
    _cooldown[_player] = 8.4f;

    setPlaying();
  }

  // End blindtest
  else if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::E) == true)
    _machine.pop();
}

void  QUIZ::BlindtestQuizScene::updateAnswer(float elapsed)
{
  // Pause/resume music
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::P) == true) {
    if (_music.getStatus() == sf::Sound::Playing)
      _music.pause();
    else
      _music.play();
  }

  // Reset music
  else if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::R) == true) {
    _music.setPlayingOffset(sf::Time::Zero);
    _music.play();
  }

  // Next blindtest
  else if (_quiz.blindtests.empty() == false && Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::N) == true) {
    _machine.swap<QUIZ::BlindtestQuizScene>(_quiz);
    return;
  }
    

  // End blindtest
  else if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::E) == true) {
    _machine.pop();
    return;
  }

}

void  QUIZ::BlindtestQuizScene::draw()
{
  switch (_state)
  {
  case QUIZ::BlindtestQuizScene::StatePlaying:
    drawTexture(_music.getStatus() == sf::Sound::Playing ? _play : _pause, 0.5f);
    break;
  case QUIZ::BlindtestQuizScene::StatePending:
    drawTexture(_quiz.avatars[_quiz.players[_player].avatar], 0.75f);
    break;
  case QUIZ::BlindtestQuizScene::StateAnswer:
    drawTexture(_cover, 0.9f);
    break;
  default:
    break;
  }

  // Draw timer
  drawTimer();
}

void  QUIZ::BlindtestQuizScene::drawTimer()
{
  float completion = std::clamp(_music.getPlayingOffset().asSeconds() / _music.getDuration().asSeconds(), 0.f, 1.f);

  // Draw timer bar
  _bar.setScale(Game::Window::Instance().window().getSize().x * completion, 16.f);
  _bar.setPosition(0.f, Game::Window::Instance().window().getSize().y - _bar.getScale().y);
  Game::Window::Instance().window().draw(_bar);
}

void  QUIZ::BlindtestQuizScene::drawTexture(const sf::Texture& texture, float size)
{
  auto  texture_size = texture.getSize();
  auto  screen_size = Game::Window::Instance().window().getSize();
  float scale = std::min(
    screen_size.x * size / texture_size.x,
    screen_size.y * size / texture_size.y);

  sf::Sprite  sprite;

  sprite.setTexture(texture, true);
  sprite.setScale(scale, scale);
  sprite.setPosition(
    (screen_size.x - texture_size.x * scale) / 2.f,
    (screen_size.y - texture_size.y * scale) / 2.f);

  Game::Window::Instance().window().draw(sprite);
}
