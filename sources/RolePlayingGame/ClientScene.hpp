#pragma once

#include <memory>

#include "Scenes/AbstractScene.hpp"
#include "RolePlayingGame/Server.hpp"
#include "RolePlayingGame/World.hpp"
#include "RolePlayingGame/TcpClient.hpp"

namespace RPG
{
  class ClientScene : public Game::AbstractScene
  {
  private:
    std::unique_ptr<RPG::Server>  _server;  // Server instance (localhost)
    RPG::TcpClient                _client;  // Client TCP connexion
    RPG::ClientWorld              _world;   // RPG world
    
    void  updatePacketReceive(float elapsed); // Read available packets
    void  updatePacketSend(float elapsed);    // Send pending packets

    void  updateInputs(float elapsed);  // Handle cursor events


    void  drawWorld();  // Draw world

  public:
    ClientScene(Game::SceneMachine& machine, std::unique_ptr<RPG::Server>&& server);
    ClientScene(Game::SceneMachine& machine, std::uint16_t port, std::uint32_t address);
    ~ClientScene() = default;

    void  send(const std::string& type, Game::JSON::Object& json);  // Send JSON to server, add type field

    bool  update(float elapsed) override; // You should override this method
    void  draw() override;                // You should override this method
  };
}