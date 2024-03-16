#pragma once

#include <memory>

#include "Math/Vector.hpp"
#include "Scenes/AbstractScene.hpp"
#include "RolePlayingGame/Camera.hpp"
#include "RolePlayingGame/Level.hpp"
#include "RolePlayingGame/GameServer.hpp"
#include "RolePlayingGame/TcpClient.hpp"

namespace RPG
{
  class GameScene : public Game::AbstractScene
  {
  private:
    std::unique_ptr<RPG::GameServer>  _server;  // Self-hosted server instance, empty when not self-hosted
    RPG::TcpClient                    _client;  // Client TCP connexion

    RPG::Camera _camera;  // World camera
    RPG::Level  _level;   // Current level

    Math::Vector<2, int>  _cursor;  // 
    Math::Vector<2, int>  _select;

    void  updatePacketReceive(float elapsed); // Read available packets
    void  updatePacketSend(float elapsed);    // Send pending packets

    void  updateInputs(float elapsed);  // Handle cursor events


    void  drawWorld();  // Draw world

  public:
    GameScene(Game::SceneMachine& machine, std::unique_ptr<RPG::GameServer>&& server);
    GameScene(Game::SceneMachine& machine, std::uint16_t port, std::uint32_t address);
    ~GameScene();

    bool  update(float elapsed) override; // You should override this method
    void  draw() override;                // You should override this method

    void  send(const std::string& type, Game::JSON::Object& json);  // Send JSON to server, add type field
  };
}