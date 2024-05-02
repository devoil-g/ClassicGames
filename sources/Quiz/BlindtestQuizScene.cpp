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
  _blindtest(_quiz.blindtests.end()),
  _music(),
  _bar(sf::Vector2f(1.f, 1.f)),
  _state(StatePlaying),
  _buzz(-1),
  _cooldowns(_quiz.players.size(), 0.f),
  _cooldown(10.f)
{
  // Initialize timer bar
  _bar.setSize(sf::Vector2f(1.f, 1.f));
  _bar.setFillColor(sf::Color::White);

  // Load entities
  auto& play = _quiz.entities.emplace(std::make_pair("play", Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "play.png")).first->second;
  auto& pause = _quiz.entities.emplace(std::make_pair("pause", Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "pause.png")).first->second;
  auto& cover = _quiz.entities.emplace(std::make_pair("cover", Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "default.png")).first->second;
  auto& winner = _quiz.entities.emplace(std::make_pair("winner", Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "default.png")).first->second;

  play.setPosition(1.f / 12.f, 1.f / 12.f);
  play.setScale(1.f / 8.f, 1.f / 8.f);
  play.setColor(1.f, 1.f, 1.f, 0.f);
  play.setLerp(0.0625f);
  pause.setPosition(1.f / 12.f, 1.f / 12.f);
  pause.setScale(1.f / 8.f, 1.f / 8.f);
  pause.setColor(1.f, 1.f, 1.f, 0.f);
  pause.setLerp(0.0625f);
  cover.setPosition(0.5f, 0.5f);
  cover.setScale(0.9f, 0.9f);
  cover.setColor(1.f, 1.f, 1.f, 0.f);
  cover.setLerp(0.125f);
  winner.setPosition(11.f / 12.f, 1.f / 12.f);
  winner.setScale(1.f / 8.f, 1.f / 8.f);
  winner.setColor(1.f, 1.f, 1.f, 0.f);
  winner.setLerp(0.125f);

  // Select first unplayed blindtest
  _blindtest = std::find_if(_quiz.blindtests.begin(), _quiz.blindtests.end(), [](const auto& entry) { return entry.done == false; });
  if (_blindtest == _quiz.blindtests.end())
    _blindtest = _quiz.blindtests.begin();

  // Start blindtest
  start();
}

QUIZ::BlindtestQuizScene::~BlindtestQuizScene()
{
  // Delete blindtest entities
  _quiz.entities.erase("play");
  _quiz.entities.erase("pause");
  _quiz.entities.erase("cover");
  _quiz.entities.erase("winner");
}

void  QUIZ::BlindtestQuizScene::start()
{
  // Load music
  if (_music.openFromFile(_blindtest->music.string()) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Reset volume
  _music.setVolume(50.f);

  // Reset status
  std::fill(_cooldowns.begin(), _cooldowns.end(), 0.01f);
  setPlaying();

  // Set player visible
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));

    entity.setTargetPosition((index + 1.f) / (_quiz.players.size() + 1.f), 0.5f);
    entity.setTargetScale(0.9f / (_quiz.players.size() + 1.f), 0.75f);
    entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
    entity.setLerp(0.0625f);
  }

  // Reset winner
  _quiz.entities.at("winner").setTargetColor(1.f, 1.f, 1.f, 0.f);

  auto count = std::count_if(_quiz.blindtests.begin(), _quiz.blindtests.end(), [](const auto& entry) { return entry.done == false; });

  std::cout << std::endl
    << "--- BLINDTEST ---" << std::endl
    << "Instruction for players: use the red buzzer to answer" << std::endl
    << std::endl
    << "Commands:" << std::endl
    << "  [Enter]:      play/pause music" << std::endl
    << "  [Backspace]:  replay music from the begining" << std::endl
    << "  Arrow[L/R]:   jump music backward/forward" << std::endl
    << "  Arrow[U/D]:   change volume" << std::endl
    << "  Page[+/-]:    change cooldown" << std::endl
    << "  [C]orrect:    correct answer, give points, display answer" << std::endl
    << "  [W]rong:      wrong answer, return to game" << std::endl
    << "  [A]nswer:     display answer, no winner" << std::endl
    << "  [L]ock:       lock buzzers" << std::endl
    << "  [R]elease:    release buzzers" << std::endl
    << "  [N]ext:       next blindtest" << std::endl
    << "  [B]ack:       previous blindtest" << std::endl
    << "  [E]nd:        end blindtest" << std::endl
    << std::endl
    << "Remaining: " << count << std::endl
    << "Answer:    " << _blindtest->answer << (_blindtest->done == true ? " (done)" : "") << std::endl
    << std::endl;
}

void  QUIZ::BlindtestQuizScene::next()
{
  // No blindtests!
  if (_quiz.blindtests.empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Next blindtest
  if (_blindtest == _quiz.blindtests.end() || std::next(_blindtest) == _quiz.blindtests.end())
    _blindtest = _quiz.blindtests.begin();
  else
    _blindtest = std::next(_blindtest);

  // Start selected blintest
  start();
}

void  QUIZ::BlindtestQuizScene::previous()
{
  // No blindtests!
  if (_quiz.blindtests.empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Next blindtest
  if (_blindtest == _quiz.blindtests.begin())
    _blindtest = std::prev(_quiz.blindtests.end());
  else
    _blindtest = std::prev(_blindtest);

  // Start selected blintest
  start();
}

bool  QUIZ::BlindtestQuizScene::update(float elapsed)
{
  // Return to main menu
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::E) == true) {
    _machine.pop();
    return false;
  }

  // Forward
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Right) == true && _music.getStatus() != sf::SoundSource::Stopped)
    _music.setPlayingOffset(_music.getPlayingOffset() + sf::seconds(3.f));

  // Backward
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Left) == true && _music.getStatus() != sf::SoundSource::Stopped)
    _music.setPlayingOffset(_music.getPlayingOffset() - sf::seconds(3.f));

  // Increase cooldown
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::PageUp) == true) {
    _cooldown = std::max(0.f, _cooldown + 0.25f * (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::LShift) == true ? 10.f : 1.f));
    std::cout << "\rCooldown set to " << _cooldown << " seconds.        " << std::flush;
  }

  // Decrease cooldown
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::PageDown) == true) {
    _cooldown = std::max(0.f, _cooldown - 0.25f * (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::LShift) == true ? 10.f : 1.f));
    std::cout << "\rCooldown set to " << _cooldown << " seconds.        " << std::flush;
  }

  // Increase volume
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Up) == true)
    _music.setVolume(std::min(100.f, _music.getVolume() + elapsed * 100.f * (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::LShift) == true ? 2.f : 1.f)));

  // Decrease volume
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Down) == true)
    _music.setVolume(std::max(0.f, _music.getVolume() - elapsed * 100.f * (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::LShift) == true ? 2.f : 1.f)));

  // Next blindtest
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::N) == true)
    next();

  // Previous blindtest
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::B) == true)
    previous();

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

  // Update player color
  for (int index = 0; index < _quiz.players.size(); index++) {
    auto& player = _quiz.players[index];
    auto& entity = _quiz.entities.at("player_" + std::to_string(player.id));

    if (std::isnan(_cooldowns[index]) == true)
      entity.setTargetColor(0.25f, 0.25f, 0.25f, 0.f);
    else if (_cooldowns[index] > 0.f)
      entity.setTargetColor(0.25f, 0.25f, 0.25f, 1.f);
    else
      entity.setTargetColor(1.f, 1.f, 1.f, 1.f);
  }

  // Update status icon
  if (_music.getStatus() == sf::Sound::Playing) {
    _quiz.entities.at("play").setTargetColor(1.f, 1.f, 1.f, 1.f);
    _quiz.entities.at("pause").setTargetColor(1.f, 1.f, 1.f, 0.f);
  }
  else {
    _quiz.entities.at("play").setTargetColor(1.f, 1.f, 1.f, 0.f);
    _quiz.entities.at("pause").setTargetColor(1.f, 1.f, 1.f, 1.f);
  }

  return false;
}

void  QUIZ::BlindtestQuizScene::updatePlaying(float elapsed)
{
  // Pause/resume music
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Enter) == true) {
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
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Backspace) == true)
    _music.setPlayingOffset(sf::Time::Zero);

  // Lock buzzers
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::L) == true)
    std::fill(_cooldowns.begin(), _cooldowns.end(), std::numeric_limits<float>::infinity());

  // Release buzzers
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::R) == true)
    std::fill(_cooldowns.begin(), _cooldowns.end(), 0.f);

  // Skip to answer
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::A) == true) {
    auto  ref = Game::Audio::Sound::Instance().get();

    // Play correct answer sound
    ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_timesup.wav"));
    ref.sound.play();

    // No points
    _blindtest->done = true;

    // Go to answer mode
    setAnswer();
  }
  
  // Play the game!
  else
  {
    // Update player cooldown
    if (_music.getStatus() == sf::SoundSource::Status::Playing)
      for (int index = 0; index < _quiz.players.size(); index++)
        _cooldowns[index] = std::max(0.f, _cooldowns[index] - elapsed);

    // Check player buzzer
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
          cooldown = (_cooldown == 0.f) ? 0.6f : std::max(0.6f, cooldown);
        _cooldowns[index] = 0.f;

        // Go to pending mode
        _buzz = index;
        setPending();

        break;
      }
    }
  }

}

void  QUIZ::BlindtestQuizScene::updatePending(float elapsed)
{
  // Lock buzzers
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::L) == true) {
    std::fill(_cooldowns.begin(), _cooldowns.end(), std::numeric_limits<float>::infinity());
    setPlaying();
  }

  // Release buzzers
  else if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::R) == true) {
    std::fill(_cooldowns.begin(), _cooldowns.end(), 0.f);
    setPlaying();
  }

  // Correct answer
  else if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::C) == true) {
    auto  ref = Game::Audio::Sound::Instance().get();

    // Play correct answer sound
    ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_correct.wav"));
    ref.sound.play();

    // Increase score
    if (_blindtest->done == false) {
      _quiz.players[_buzz].score += 1;
      _blindtest->done = true;
    }

    // Resume music
    _music.play();

    // Set winner
    auto& winner = _quiz.entities.at("winner");
    winner.setTexture(_quiz.avatars[_quiz.players[_buzz].avatar][_quiz.players[_buzz].skin]);
    winner.setTargetColor(1.f, 1.f, 1.f, 1.f);

    // Go to answer mode
    setAnswer();
  }

  // Wrong answer
  else if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::W) == true) {
    auto  ref = Game::Audio::Sound::Instance().get();

    // Play wrong answer sound
    ref.sound.setBuffer(Game::SoundLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "quiz" / "sounds" / "question_wrong.wav"));
    ref.sound.play();

    // Set player cooldown
    _cooldowns[_buzz] = (_cooldown == 0.f) ? std::numeric_limits<float>::infinity() : _cooldown;
    
    // Resume music
    _music.play();

    // Return to playing mode
    setPlaying();
  }
}

void  QUIZ::BlindtestQuizScene::updateAnswer(float elapsed)
{
  // Pause/resume music
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Enter) == true) {
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
  else if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::BackSpace) == true)
    _music.setPlayingOffset(sf::Time::Zero);
}

void  QUIZ::BlindtestQuizScene::setPlaying()
{
  _state = State::StatePlaying;
  _buzz = -1;

  // Hide cover
  _quiz.entities.at("cover").setTargetColor(1.f, 1.f, 1.f, 0.f);
}

void  QUIZ::BlindtestQuizScene::setPending()
{
  _state = State::StatePending;

  // Hide cover
  _quiz.entities.at("cover").setColor(1.f, 1.f, 1.f, 0.f);
}

void  QUIZ::BlindtestQuizScene::setAnswer()
{
  _state = StateAnswer;

  // Hide players
  std::fill(_cooldowns.begin(), _cooldowns.end(), std::numeric_limits<float>::quiet_NaN());

  // Show cover
  _quiz.entities.at("cover").setTexture(_blindtest->cover);
  _quiz.entities.at("cover").setTargetColor(1.f, 1.f, 1.f, 1.f);
}

void  QUIZ::BlindtestQuizScene::draw()
{
  float completion = std::clamp(_music.getPlayingOffset().asSeconds() / _music.getDuration().asSeconds(), 0.f, 1.f);

  // Draw timer bar
  _bar.setScale(Game::Window::Instance().window().getSize().x * completion, 16.f);
  _bar.setPosition(0.f, Game::Window::Instance().window().getSize().y - _bar.getScale().y);
  Game::Window::Instance().window().draw(_bar);
}