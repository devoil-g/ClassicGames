#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>
#include <queue>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include "GameBoyColor/GameBoyColor.hpp"
#include "Scenes/AbstractScene.hpp"

namespace GBC
{
  class EmulationScene : public Game::AbstractScene
  {
  private:
    sf::Texture         _texture; // Rendering target
    GBC::GameBoyColor   _gbc;     // Game Boy emulator
    float               _fps;     // Timer for FPS control
    bool                _vsync;   // Vertical sync before the scene

    class SoundStream : public sf::SoundStream
    {
      std::queue<std::array<std::int16_t, GBC::AudioProcessingUnit::BufferSize>>  _sounds;  // Sound buffer
      std::array<std::int16_t, GBC::AudioProcessingUnit::BufferSize>              _buffer;  // Buffer sent to play
      std::mutex                                                                  _lock;    // Sounds buffer lock
      std::size_t                                                                 _index;   // Currently played index

      enum {
        Playing,
        Buffering
      } _status;

      virtual bool  onGetData(sf::SoundStream::Chunk& chunk) override;  // Feed audio buffer
      virtual void  onSeek(sf::Time) override;                          // Does nothing

    public:
      SoundStream();
      ~SoundStream() = default;

      void  push(const std::array<std::int16_t, GBC::AudioProcessingUnit::BufferSize>& sound);  // Feed a new sound buffer to stream
    };

    GBC::EmulationScene::SoundStream  _stream;  // Sound stream of Game Boy Color

  public:
    EmulationScene(Game::SceneMachine& machine, const std::filesystem::path& filename);
    ~EmulationScene();

    bool  update(float elapsed) override; // Simulate the GBC
    void  draw() override;                // Draw GBC rendering texture
  };
}