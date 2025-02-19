#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Scenes/AbstractScene.hpp"
#include "RolePlayingGame/Server.hpp"
#include "RolePlayingGame/TcpClient.hpp"
#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class ClientScene : public Game::AbstractScene
  {
  private:
    std::unique_ptr<RPG::Server>  _server;  // Server instance (localhost)
    RPG::TcpClient                _client;  // Client TCP connexion
    RPG::ECS                      _ecs;     // Client game ECS

    void  updateReceive(float elapsed);                                 // Read available packets
    void  updatePacket(const Game::JSON::Object& json, float elapsed);  // Handle received packet
    void  updateSend(float elapsed);                                    // Send pending packets

  public:
    ClientScene(Game::SceneMachine& machine, std::unique_ptr<RPG::Server>&& server);
    ClientScene(Game::SceneMachine& machine, std::uint16_t port, std::uint32_t address);
    ~ClientScene() = default;

    void  send(const std::vector<std::string>& type, Game::JSON::Object& json); // Send JSON to server, add type field

    bool  update(float elapsed) override; // You should override this method
    void  draw() override;                // You should override this method
  };
}