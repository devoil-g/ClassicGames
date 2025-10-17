#include <iostream>
#include <limits>

#include "Scenes/MidiScene.hpp"
#include "System/Window.hpp"
#include "Math/Math.hpp"
#include "System/Audio/Synthesizer.hpp"
#include "System/Config.hpp"

const float         Game::MidiScene::MiddleCFrequency = 261.626f;

Game::MidiScene::MidiScene(Game::SceneMachine& machine) :
  Game::AbstractScene(machine),
  sf::SoundStream(),
  _buffer(),
  _offset(0),
  _octave(0),
  _wave()
{
  for (int index = 0; index < _wave.size(); index++)
    _wave[index] = std::cos((float)index / (float)_wave.size() * 2.f * Math::Pi);

  _buffer.fill(0);
  sf::SoundStream::initialize(Game::MidiScene::ChannelCount, Game::MidiScene::SampleRate, { sf::SoundChannel::Mono });
  sf::SoundStream::play();
}

bool  Game::MidiScene::update(float elapsed)
{
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Enter) == true) {
    Game::Audio::Synthesizer  midi(Game::Config::ExecutablePath / "assets" / "levels" / "beethoven.mid", Game::Config::ExecutablePath / "assets" / "levels" / "gzdoom.sf2");
    midi.generate(0);
  }

  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Escape) == true)
    _machine.pop();

  // Octave control
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Subtract) == true)
    _octave += -1;
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Add) == true)
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

  static const std::array<std::pair<Game::Window::Key, Game::MidiScene::Note>, Game::MidiScene::Note::NoteCount> binding = {
    std::pair<Game::Window::Key, Game::MidiScene::Note>(Game::Window::Key::Q, Game::MidiScene::Note::Do),
    std::pair<Game::Window::Key, Game::MidiScene::Note>(Game::Window::Key::Z, Game::MidiScene::Note::DoD),
    std::pair<Game::Window::Key, Game::MidiScene::Note>(Game::Window::Key::S, Game::MidiScene::Note::Re),
    std::pair<Game::Window::Key, Game::MidiScene::Note>(Game::Window::Key::E, Game::MidiScene::Note::ReD),
    std::pair<Game::Window::Key, Game::MidiScene::Note>(Game::Window::Key::D, Game::MidiScene::Note::Mi),
    std::pair<Game::Window::Key, Game::MidiScene::Note>(Game::Window::Key::F, Game::MidiScene::Note::Fa),
    std::pair<Game::Window::Key, Game::MidiScene::Note>(Game::Window::Key::T, Game::MidiScene::Note::FaD),
    std::pair<Game::Window::Key, Game::MidiScene::Note>(Game::Window::Key::G, Game::MidiScene::Note::Sol),
    std::pair<Game::Window::Key, Game::MidiScene::Note>(Game::Window::Key::Y, Game::MidiScene::Note::SolD),
    std::pair<Game::Window::Key, Game::MidiScene::Note>(Game::Window::Key::H, Game::MidiScene::Note::La),
    std::pair<Game::Window::Key, Game::MidiScene::Note>(Game::Window::Key::U, Game::MidiScene::Note::LaD),
    std::pair<Game::Window::Key, Game::MidiScene::Note>(Game::Window::Key::J, Game::MidiScene::Note::Si)
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