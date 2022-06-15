#pragma once

#include <array>
#include <cstdint>
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
    static const sf::Time ForcedExit; // Forced exit time limit

    GBC::GameBoyColor   _gbc;     // Game Boy emulator
    sf::Sprite          _sprite;  // Display rendered texture
    sf::Time            _fps;     // Timer for FPS control
    sf::Time            _exit;    // Timer of forced exit
    sf::RectangleShape  _bar;     // Forced exit bar

    class SoundStream : public sf::SoundStream
    {
      std::queue<std::array<std::int16_t, GBC::GameBoyColor::SoundBufferSize>>  _sounds;  // Sound buffer
      std::array<std::int16_t, GBC::GameBoyColor::SoundBufferSize>              _buffer;  // Buffer sent to play
      std::mutex                                                                _lock;    // Sounds buffer lock
      std::size_t                                                               _index;   // Currently played index

      enum {
        Playing,
        Buffering
      } _status;

      virtual bool  onGetData(sf::SoundStream::Chunk& chunk) override;  // Base class
      virtual void  onSeek(sf::Time) override;                          // Base class

    public:
      SoundStream();
      ~SoundStream() = default;

      void  push(const std::array<std::int16_t, GBC::GameBoyColor::SoundBufferSize>& sound);  // Feed a new sound buffer to stream
    };

    GBC::EmulationScene::SoundStream  _stream;  // Sound stream of Game Boy Color

  public:
    EmulationScene(Game::SceneMachine& machine, const std::string& filename);
    ~EmulationScene();

    bool  update(sf::Time elapsed) override;  // TODO: comment
    void  draw() override;                    // TODO: comment
  };
}