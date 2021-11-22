#include <iostream>
#include <limits>

#include "Scenes/MidiScene.hpp"
#include "System/Window.hpp"
#include "Math/Math.hpp"
#include "System/Midi.hpp"
#include "System/Config.hpp"

const float         Game::MidiScene::MiddleCFrequency = 261.626f;

Game::MidiScene::MidiScene(Game::SceneMachine& machine) :
  Game::AbstractScene(machine),
  _buffer(),
  _offset(0),
  _octave(0),
  _wave()
{
  for (int index = 0; index < _wave.size(); index++)
    _wave[index] = std::cos((float)index / (float)_wave.size() * 2.f * Math::Pi);

  _buffer.fill(0);
  sf::SoundStream::initialize(Game::MidiScene::ChannelCount, Game::MidiScene::SampleRate);
  sf::SoundStream::play();
}

bool  Game::MidiScene::update(sf::Time elapsed)
{
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Enter) == true) {
    Game::Midi  midi(Game::Config::ExecutablePath + "/assets/levels/beethoven.mid");
    midi.generate(midi.sequences.front(), 44100);
  }

  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Escape) == true)
    _machine.pop();

  // Octave control
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Subtract) == true)
    _octave += -1;
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Add) == true)
    _octave += +1;

  return false;
}

bool  Game::MidiScene::onGetData(sf::SoundStream::Chunk& data)
{
  // Base infos
  data.sampleCount = Game::MidiScene::FramePerChunk;
  data.samples = _buffer.data();

  // Piano!
  float C = MiddleCFrequency * (float)std::pow(2, _octave);

  static const std::array<std::pair<sf::Keyboard::Key, Game::MidiScene::Note>, Game::MidiScene::Note::NoteCount> binding = {
    std::pair<sf::Keyboard::Key, Game::MidiScene::Note>(sf::Keyboard::Q, Game::MidiScene::Note::Do),
    std::pair<sf::Keyboard::Key, Game::MidiScene::Note>(sf::Keyboard::Z, Game::MidiScene::Note::DoD),
    std::pair<sf::Keyboard::Key, Game::MidiScene::Note>(sf::Keyboard::S, Game::MidiScene::Note::Re),
    std::pair<sf::Keyboard::Key, Game::MidiScene::Note>(sf::Keyboard::E, Game::MidiScene::Note::ReD),
    std::pair<sf::Keyboard::Key, Game::MidiScene::Note>(sf::Keyboard::D, Game::MidiScene::Note::Mi),
    std::pair<sf::Keyboard::Key, Game::MidiScene::Note>(sf::Keyboard::F, Game::MidiScene::Note::Fa),
    std::pair<sf::Keyboard::Key, Game::MidiScene::Note>(sf::Keyboard::T, Game::MidiScene::Note::FaD),
    std::pair<sf::Keyboard::Key, Game::MidiScene::Note>(sf::Keyboard::G, Game::MidiScene::Note::Sol),
    std::pair<sf::Keyboard::Key, Game::MidiScene::Note>(sf::Keyboard::Y, Game::MidiScene::Note::SolD),
    std::pair<sf::Keyboard::Key, Game::MidiScene::Note>(sf::Keyboard::H, Game::MidiScene::Note::La),
    std::pair<sf::Keyboard::Key, Game::MidiScene::Note>(sf::Keyboard::U, Game::MidiScene::Note::LaD),
    std::pair<sf::Keyboard::Key, Game::MidiScene::Note>(sf::Keyboard::J, Game::MidiScene::Note::Si)
  };

  _buffer.fill(0);
  for (const auto& key : binding)
    if (Game::Window::Instance().keyboard().keyDown(key.first) == true)
      for (int index = 0; index < Game::MidiScene::ChannelCount * Game::MidiScene::FramePerChunk; index++)
        _buffer[index] = (int16_t)std::clamp((int)((float)_buffer[index] + 8192.f * std::cos((float)(index / ChannelCount + _offset) / (float)SampleRate * Game::MidiScene::GetRatio(key.second) * C * 2.f * Math::Pi)), (int)std::numeric_limits<int16_t>::min(), (int)std::numeric_limits<int16_t>::max());

  // Add number of sample generated to counter
  _offset += data.sampleCount;

  return true;
}

void  Game::MidiScene::onSeek(sf::Time timeOffset)
{
  _offset = (size_t)(timeOffset.asSeconds() * Game::MidiScene::SampleRate);
}

void  Game::MidiScene::draw()
{}