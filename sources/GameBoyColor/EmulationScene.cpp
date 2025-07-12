#include <fstream>
#include <filesystem>

#include "GameBoyColor/EmulationScene.hpp"
#include "Math/Math.hpp"
#include "System/Window.hpp"

GBC::EmulationScene::EmulationScene(Game::SceneMachine& machine, const std::filesystem::path& filename) :
  Game::AbstractScene(machine),
  _texture(sf::Vector2u(GBC::PixelProcessingUnit::ScreenWidth, GBC::PixelProcessingUnit::ScreenHeight)),
  _gbc(filename, _texture, Math::Vector<2, unsigned int>((unsigned int)0, (unsigned int)0)),
  _fps(-0.42f),
  _stream(),
  _vsync(Game::Window::Instance().getVerticalSync())
{
  // Texture is not filtered
  _texture.setSmooth(false);

  // Start sound stream
  _stream.setVolume(30.f);
  _stream.play();

  // Force disable vertical sync
  Game::Window::Instance().setVerticalSync(false);
}

GBC::EmulationScene::~EmulationScene()
{
  // Stop the stream before destroying the class
  _stream.stop();

  // Restore vertical sync
  Game::Window::Instance().setVerticalSync(_vsync);
}

bool  GBC::EmulationScene::update(float elapsed)
{
  // Update timers
  _fps = std::min(_fps + elapsed, 2.f * (float)GBC::PixelProcessingUnit::FrameDuration / (float)GBC::CentralProcessingUnit::Frequency);

  // Sound volume control
  if (Game::Window::Instance().keyboard().keyDown(Game::Window::Key::Subtract) == true)
    _stream.setVolume(std::clamp(_stream.getVolume() - elapsed * 64.f, 0.f, 100.f));
  if (Game::Window::Instance().keyboard().keyDown(Game::Window::Key::Add) == true)
    _stream.setVolume(std::clamp(_stream.getVolume() + elapsed * 64.f, 0.f, 100.f));

  const std::array<Game::Window::Key, 12> save_slots = {
    Game::Window::Key::F1, Game::Window::Key::F2, Game::Window::Key::F3, Game::Window::Key::F4,
    Game::Window::Key::F5, Game::Window::Key::F6, Game::Window::Key::F7, Game::Window::Key::F8,
    Game::Window::Key::F9, Game::Window::Key::F10, Game::Window::Key::F11, Game::Window::Key::F12
  };
  
  // Save/load states
  for (std::size_t index = 0; index < save_slots.size(); index++) {
    if (Game::Window::Instance().keyboard().keyPressed(save_slots[index]) == true) {
      if (Game::Window::Instance().keyboard().keyDown(Game::Window::Key::LShift) == true ||
        Game::Window::Instance().keyboard().keyDown(Game::Window::Key::RShift) == true)
        _gbc.save(index + 1);
      else
        _gbc.load(index + 1);
    }
  }

  // Simulate frames at 59.72 fps
  // NOTE: simulate at most one frame per call to avoid exponential delay
  if (_fps >= (float)GBC::PixelProcessingUnit::FrameDuration / (float)GBC::CentralProcessingUnit::Frequency ||
    Game::Window::Instance().keyboard().keyDown(Game::Window::Key::LControl) == true ||
    Game::Window::Instance().joystick().position(0, Game::Window::JoystickAxis::Z) < -64.f) {
    _gbc.simulate();
    _fps = std::max(_fps - (float)GBC::PixelProcessingUnit::FrameDuration / (float)GBC::CentralProcessingUnit::Frequency, 0.f);

    // Push sound buffer to sound stream queue
    _stream.push(_gbc.sound());
  }

  return false;
}

void  GBC::EmulationScene::draw()
{
  // Draw GBC rendering target
  Game::Window::Instance().draw(_gbc.lcd());
}

GBC::EmulationScene::SoundStream::SoundStream() :
  _sounds(),
  _lock(),
  _buffer(),
  _index(0),
  _status(GBC::EmulationScene::SoundStream::Buffering)
{
  initialize(2, GBC::AudioProcessingUnit::SampleRate, { sf::SoundChannel::FrontLeft, sf::SoundChannel::FrontRight });
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