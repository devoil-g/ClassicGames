#include "Math/Math.hpp"
#include "Quiz/BlindtestQuizScene.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Audio/Sound.hpp"
#include "System/Library/SoundLibrary.hpp"
#include "System/Library/FontLibrary.hpp"

#include <iostream>

#include <SFML/Graphics/Text.hpp>
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
  _buzz(-1),
  _display(true),
  _cooldowns(_quiz.players.size(), 0.f),
  _cooldown(10.f)
{
  // Initialize timer bar
  _bar.setSize(sf::Vector2f(1.f, 1.f));
  _bar.setFillColor(sf::Color::White);

  // Load blindtest
  next();
}

void  QUIZ::BlindtestQuizScene::usage() const
{
  std::cout << std::endl
    << "--- BLINDTEST ---" << std::endl
    << "Instruction for players: use the red buzzer to answer" << std::endl
    << std::endl
    << "Commands:" << std::endl
    << "  [P]lay/pause: pause/resume music" << std::endl
    << "  [R]eset:      replay music from the begining" << std::endl
    << "  Arrow[L/R]:   change cooldown" << std::endl
    << "  Arrow[U/D]:   change volume" << std::endl
    << "  [S]cores:     toggle score display" << std::endl
    << "  [C]orrect:    correct answer, give points, display answer" << std::endl
    << "  [W]rong:      wrong answer, return to game" << std::endl
    << "  [A]nswer:     display answer, no winner" << std::endl
    << "  [N]ext:       next/skip blindtest (" << _quiz.blindtests.size() << " remaining)" << std::endl
    << "  [E]nd:        end blindtest" << std::endl
    << std::endl
    << "Answer: " << _blindtest.answer << std::endl
    << std::endl;
}

void  QUIZ::BlindtestQuizScene::next()
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
  if (_music.openFromFile(_blindtest.music.string()) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Reset volume
  _music.setVolume(50.f);

  // Load texture
  if (_cover.loadFromFile(_blindtest.cover.string()) == false ||
    _play.loadFromFile((Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "play.png").string()) == false ||
    _pause.loadFromFile((Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "pause.png").string()) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Smooth textures
  _cover.setSmooth(true);
  _play.setSmooth(true);
  _pause.setSmooth(true);

  // Generate mipmaps
  _cover.generateMipmap();
  _play.generateMipmap();
  _pause.generateMipmap();

  // Reset status
  _state = State::StatePlaying;
  _buzz = -1;
  std::fill(_cooldowns.begin(), _cooldowns.end(), 0.f);

  // Display usage
  usage();
}

bool  QUIZ::BlindtestQuizScene::update(float elapsed)
{
  // Return to main menu
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::E) == true) {
    _machine.pop();
    return false;
  }

  // Toggle score display
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::S) == true)
    _display = !_display;

  // Play forward
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::PageUp) == true)
    _music.setPlayingOffset(_music.getPlayingOffset() + sf::seconds(3.f));

  // Play forward
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::PageDown) == true)
    _music.setPlayingOffset(_music.getPlayingOffset() - sf::seconds(3.f));

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

  // Increase volume
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Up) == true)
    _music.setVolume(std::min(100.f, _music.getVolume() + elapsed * 100.f * (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::LShift) == true ? 2.f : 1.f)));

  // Decrease volume
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Down) == true)
    _music.setVolume(std::max(0.f, _music.getVolume() - elapsed * 100.f * (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::LShift) == true ? 2.f : 1.f)));

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
  for (auto& cooldown : _cooldowns)
    cooldown = std::max(0.f, cooldown - elapsed);

  // Pause/resume music
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::P) == true) {
    if (_music.getStatus() == sf::Sound::Playing)
      _music.pause();
    else if (_music.getStatus() == sf::Sound::Stopped) {
      _music.setPlayingOffset(sf::Time::Zero);
      _music.play();
    }
    else
      _music.play();
  }

  // Reset music
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::R) == true) {
    _music.setPlayingOffset(sf::Time::Zero);
  }

  // Change scores
  if (_display == true && (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Left) == true || Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Right) == true)) {
    for (auto player_index = 0; player_index < _quiz.players.size(); player_index++) {
      auto& player = _quiz.players.at(player_index);
      if (player.sprite.getGlobalBounds().contains((float)Game::Window::Instance().mouse().position().x, (float)Game::Window::Instance().mouse().position().y) == true) {
        auto old = player.score;
        player.score = player.score
          + (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Left) == true ? +1 : 0)
          + (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Right) == true ? -1 : 0);
        std::cout << "\rScore of player #" << (player_index + 1) << ": " << player.score << ".        " << std::flush;
      }
    }
  }

  // Skip to answer
  else if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::A) == true) {
    auto  ref = Game::Audio::Sound::Instance().get();

    // Play correct answer sound
    ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_timesup.wav"));
    ref.sound.play();

    _state = StateAnswer;
  }
  
  // Skip to next blindtest, restore question
  else if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::N) == true) {
    _quiz.blindtests.emplace_back(_blindtest);
    next();
  }

  // Check player buzzer
  else {
    for (int index = 0; index < _quiz.players.size(); index++) {
      if (_cooldowns[index] <= 0.f && Game::Window::Instance().joystick().buttonPressed(_quiz.players[index].joystick, _quiz.players[index].button + QUIZ::Quiz::Button::ButtonBuzzer) == true) {
        auto  ref = Game::Audio::Sound::Instance().get();

        // Play buzzer sound
        ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_buzzer.wav"));
        ref.sound.play();
        
        // Stop music
        _music.pause();

        // Grey out every other players
        for (auto& cooldown : _cooldowns)
          cooldown = std::max(0.01f, cooldown);
        _cooldowns[index] = 0.f;

        // Go to pending mode
        _state = StatePending;
        _buzz = index;

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
    ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_correct.wav"));
    ref.sound.play();

    // Increase score
    _quiz.players[_buzz].score += 1;

    // Resume music
    _music.play();

    // Go to answer mode
    _state = StateAnswer;
  }

  // Wrong answer
  else if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::W) == true) {
    auto  ref = Game::Audio::Sound::Instance().get();

    // Play wrong answer sound
    ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_wrong.wav"));
    ref.sound.play();

    // Set player cooldown
    _cooldowns[_buzz] = _cooldown;
    _buzz = -1;

    // Resume music
    _music.play();

    // Return to playing mode
    _state = StatePlaying;
  }
}

void  QUIZ::BlindtestQuizScene::updateAnswer(float elapsed)
{
  // Pause/resume music
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::P) == true) {
    if (_music.getStatus() == sf::Sound::Playing)
      _music.pause();
    else if (_music.getStatus() == sf::Sound::Stopped) {
      _music.setPlayingOffset(sf::Time::Zero);
      _music.play();
    }
    else
      _music.play();
  }

  // Reset music
  else if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::R) == true) {
    _music.setPlayingOffset(sf::Time::Zero);
  }

  // Skip to next blindtest, restore question
  else if (_quiz.blindtests.empty() == false && Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::N) == true) {
    next();
  }
}

void  QUIZ::BlindtestQuizScene::draw()
{
  switch (_state)
  {
  case QUIZ::BlindtestQuizScene::StatePlaying:
    drawPlaying();
    break;
  case QUIZ::BlindtestQuizScene::StatePending:
    drawPending();
    break;
  case QUIZ::BlindtestQuizScene::StateAnswer:
    drawAnswer();
    break;
  default:
    break;
  }

  // Draw timer
  drawTimer();
}

void  QUIZ::BlindtestQuizScene::drawPlaying()
{
  auto  screen = Game::Window::Instance().window().getSize();

  for (unsigned int player_index = 0; player_index < _quiz.players.size(); player_index++) {
    auto& player = _quiz.players[player_index];
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

    std::uint8_t  alpha = _cooldowns[player_index] > 0.f ? 63 : 255;

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

  sf::Sprite  sprite;

  // Select symbol to draw
  sprite.setTexture(_music.getStatus() == sf::Sound::Playing ? _play : _pause, true);

  auto size = std::min(screen.x / 8.f, screen.y / 8.f);
  auto scale = size / std::max(sprite.getTextureRect().width, sprite.getTextureRect().height);

  sprite.setPosition(size / 4.f, size / 4.f);
  sprite.setScale(scale, scale);

  Game::Window::Instance().window().draw(sprite);
}

void  QUIZ::BlindtestQuizScene::drawPending()
{
  // Draw buzzing player
  drawPlaying();
}

void  QUIZ::BlindtestQuizScene::drawAnswer()
{
  // Draw cover
  drawTexture(_cover, 0.9f);

  // Display avatar of winning player
  if (_buzz != -1) {
    auto  screen = Game::Window::Instance().window().getSize();
    auto& player = _quiz.players[_buzz];
    sf::Sprite& sprite = player.sprite;

    auto size = std::min(screen.x / 8.f, screen.y / 8.f);
    auto scale = size / std::max(sprite.getTextureRect().width, sprite.getTextureRect().height);

    sprite.setPosition(size / 4.f, size / 4.f);
    sprite.setScale(scale, scale);
    sprite.setOrigin(0.f, 0.f);

    Game::Window::Instance().window().draw(sprite);
  }
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
