#include <fstream>
#include <filesystem>

#include "GameBoyColor/EmulationScene.hpp"
#include "Math/Math.hpp"
#include "System/Window.hpp"

const sf::Time  GBC::EmulationScene::ForcedExit = sf::seconds(1.f);

GBC::EmulationScene::EmulationScene(Game::SceneMachine& machine, const std::string& filename) :
  Game::AbstractScene(machine),
  _gbc(filename),
  _texture(),
  _sprite(),
  _fps(sf::seconds(-1.f)),
  _exit(sf::Time::Zero),
  _bar(sf::Vector2f(1.f, 1.f)),
  _stream()
{
  // Initialize force exit bar
  _bar.setSize(sf::Vector2f(1.f, 1.f));
  _bar.setFillColor(sf::Color::White);
}

GBC::EmulationScene::~EmulationScene()
{}

bool  GBC::EmulationScene::update(sf::Time elapsed)
{
  // Update timers
  _exit += elapsed;
  _fps += elapsed;

  // Reset exit timer when ESC is not pressed
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Escape) == false)
    _exit = sf::Time::Zero;

  // Exit if limit reached
  if (_exit > GBC::EmulationScene::ForcedExit) {
    _machine.pop();
    return false;
  }

  // Simulate frames at 59.72 fps
  // NOTE: simulate at most one frame per call to avoid exponential delay
  if (_fps.asSeconds() >= 70224.f / Math::Pow<22>(2)) {
    _gbc.simulate();
    _fps -= sf::seconds(70224.f / Math::Pow<22>(2));

    // Push sound buffer to sound stream queue
    _stream.push(_gbc.sound());
  }

  return false;
}

void  GBC::EmulationScene::draw()
{
  const sf::Image&  image = _gbc.screen();

  // Only draw if rendering target is visible
  if (image.getSize().x != 0 && image.getSize().y != 0)
  {
    // Update texture on VRam
    if (_texture.getSize() != image.getSize() && _texture.create(image.getSize().x, image.getSize().y) == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    _texture.update(image);

    // Making sure the texture is not filtered
    _texture.setSmooth(false);

    // Update sprite texture
    _sprite.setTexture(_texture, true);

    // Compute sprite scale and position
    float scale = std::min((float)Game::Window::Instance().window().getSize().x / (float)image.getSize().x, (float)Game::Window::Instance().window().getSize().y / (float)image.getSize().y);
    float pos_x = (((float)Game::Window::Instance().window().getSize().x - ((float)image.getSize().x * scale)) / 2.f);
    float pos_y = (((float)Game::Window::Instance().window().getSize().y - ((float)image.getSize().y * scale)) / 2.f);

    // Position sprite in window
    _sprite.setScale(sf::Vector2f(scale, scale));
    _sprite.setPosition(sf::Vector2f(pos_x, pos_y));

    // Draw DOOM rendering target
    Game::Window::Instance().window().draw(_sprite);
  }

  // Draw forced exit bar
  _bar.setScale(Game::Window::Instance().window().getSize().x * _exit.asSeconds() / GBC::EmulationScene::ForcedExit.asSeconds(), 4.f);
  Game::Window::Instance().window().draw(_bar);
}

GBC::EmulationScene::SoundStream::SoundStream() :
  _sounds(),
  _lock(),
  _buffer(),
  _index(0)
{
  initialize(2, GBC::GameBoyColor::SoundSampleRate);
  play();
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
{}

void  GBC::EmulationScene::SoundStream::push(const std::array<std::int16_t, GBC::GameBoyColor::SoundBufferSize>& sound)
{
  std::unique_lock<std::mutex>  lock(_lock);

  // Push buffer to queue
  _sounds.push(sound);

  // Limit number of sounds in queue
  if (_sounds.size() > 5)
    _sounds.pop();
}