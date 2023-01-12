#include <fstream>
#include <filesystem>

#include "GameBoyColor/EmulationScene.hpp"
#include "Math/Math.hpp"
#include "System/Window.hpp"

const sf::Time  GBC::EmulationScene::ForcedExit = sf::seconds(1.f);

GBC::EmulationScene::EmulationScene(Game::SceneMachine& machine, const std::string& filename) :
  Game::AbstractScene(machine),
  _gbc(filename),
  _sprite(_gbc.lcd()),
  _fps(sf::seconds(-0.42f)),
  _exit(sf::Time::Zero),
  _bar(sf::Vector2f(1.f, 1.f)),
  _stream(),
  _sync(Game::Window::Instance().getVerticalSync())
{
  // Initialize force exit bar
  _bar.setSize(sf::Vector2f(1.f, 1.f));
  _bar.setFillColor(sf::Color::White);

  // Start sound stream
  _stream.setVolume(30.f);
  _stream.play();

  // Force disable vertical sync
  Game::Window::Instance().window().setVerticalSyncEnabled(false);
}

GBC::EmulationScene::~EmulationScene()
{
  // Stop the stream before destroying the class
  _stream.stop();

  // Restore vertical sync
  Game::Window::Instance().window().setVerticalSyncEnabled(_sync);
}

bool  GBC::EmulationScene::update(sf::Time elapsed)
{
  // Update timers
  _exit += elapsed;
  _fps = std::min(_fps + elapsed, sf::seconds(2.f * (GBC::PixelProcessingUnit::ScreenHeight + GBC::PixelProcessingUnit::ScreenBlank) * GBC::PixelProcessingUnit::ScanlineDuration / Math::Pow<22>(2)));

  // Reset exit timer when ESC is not pressed
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Escape) == false)
    _exit = sf::Time::Zero;

  // Sound volume control
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Subtract) == true)
    _stream.setVolume(std::clamp(_stream.getVolume() - elapsed.asSeconds() * 64.f, 0.f, 100.f));
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Add) == true)
    _stream.setVolume(std::clamp(_stream.getVolume() + elapsed.asSeconds() * 64.f, 0.f, 100.f));

  // Exit if limit reached
  if (_exit > GBC::EmulationScene::ForcedExit) {
    _machine.pop();
    return false;
  }

  const std::array<sf::Keyboard::Key, 12> save_slots = {
    sf::Keyboard::F1, sf::Keyboard::F2, sf::Keyboard::F3, sf::Keyboard::F4,
    sf::Keyboard::F5, sf::Keyboard::F6, sf::Keyboard::F7, sf::Keyboard::F8,
    sf::Keyboard::F9, sf::Keyboard::F10, sf::Keyboard::F11, sf::Keyboard::F12
  };
  
  // Save/load states
  for (std::size_t index = 0; index < save_slots.size(); index++) {
    if (Game::Window::Instance().keyboard().keyPressed(save_slots[index]) == true) {
      if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Key::LShift) == true ||
        Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Key::RShift) == true)
        _gbc.save(index + 1);
      else
        _gbc.load(index + 1);
    }
  }

  // Simulate frames at 59.72 fps
  // NOTE: simulate at most one frame per call to avoid exponential delay
  if (_fps.asSeconds() >= 70224.f / Math::Pow<22>(2) ||
    Game::Window::Instance().keyboard().keyDown(sf::Keyboard::LControl) == true ||
    Game::Window::Instance().joystick().position(0, 2) < -64.f) {
    _gbc.simulate();
    _fps = sf::seconds(std::max(_fps.asSeconds() - 70224.f / Math::Pow<22>(2), 0.f));

    // Push sound buffer to sound stream queue
    _stream.push(_gbc.sound());
  }

  return false;
}

void  GBC::EmulationScene::draw()
{
  // Get rendering target
  _sprite.setTexture(_gbc.lcd(), true);

  // Draw GBC rendering target
  Game::Window::Instance().draw(_sprite);

  // Draw forced exit bar
  _bar.setScale(Game::Window::Instance().window().getSize().x * _exit.asSeconds() / GBC::EmulationScene::ForcedExit.asSeconds(), 4.f);
  Game::Window::Instance().window().draw(_bar);
}

GBC::EmulationScene::SoundStream::SoundStream() :
  _sounds(),
  _lock(),
  _buffer(),
  _index(0),
  _status(GBC::EmulationScene::SoundStream::Buffering)
{
  initialize(2, GBC::AudioProcessingUnit::SampleRate);
  setVolume(50.f);
}

bool  GBC::EmulationScene::SoundStream::onGetData(sf::SoundStream::Chunk& chunk)
{
  // Set chunk data
  chunk.sampleCount = _buffer.size() / 1;
  chunk.samples = _buffer.data();
  
  // Fill chunk buffer
  for (std::size_t index = 0; index < chunk.sampleCount / 2; index++)
  {
    // Change status
    if (_sounds.size() > 1)
      _status = Playing;
    else if (_sounds.empty() == true)
      _status = Buffering;

    // Get sample from queue
    if (_status == Playing) {
      _buffer[index * 2 + 0] = _sounds.front()[_index * 2 + 0];
      _buffer[index * 2 + 1] = _sounds.front()[_index * 2 + 1];
      _index++;

      // Finished sound
      if (_index >= _buffer.size() / 2) {
        _index = 0;

        // Pop sounds from queue
        std::unique_lock<std::mutex>  lock(_lock);
        _sounds.pop();
      }
    }

    // Empty buffer, silent
    else {
      _buffer[index * 2 + 0] = 0;
      _buffer[index * 2 + 1] = 0;
    }
  }

  return true;
}

void  GBC::EmulationScene::SoundStream::onSeek(sf::Time)
{
  // Does nothing
}

void  GBC::EmulationScene::SoundStream::push(const std::array<std::int16_t, GBC::AudioProcessingUnit::BufferSize>& sound)
{
  std::unique_lock<std::mutex>  lock(_lock);

  // Push buffer to queue
  _sounds.push(sound);

  // Limit number of sounds in queue
  if (_sounds.size() > 3) {
    _sounds.pop();
    _index = 0;
  }
}