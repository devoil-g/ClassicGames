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
    RPG::ECS                      _ecs;     // Client game ECS

  public:
    ClientScene(Game::SceneMachine& machine, std::unique_ptr<RPG::Server>&& server);
    ClientScene(Game::SceneMachine& machine, std::uint16_t port, std::uint32_t address);
    ~ClientScene() = default;

    bool  update(float elapsed) override; // You should override this method
    void  draw() override;                // You should override this method
  };
}