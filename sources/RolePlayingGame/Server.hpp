#pragma once

#include <filesystem>
#include <unordered_map>

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"
#include "RolePlayingGame/TcpServer.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class Server
  {
  private:
    RPG::ECS  _ecs; // Server game ECS

  public:
    Server(const std::filesystem::path& config, std::uint16_t port = 0, std::uint32_t address = 0);
    ~Server();

    std::uint16_t getPort();
    std::uint32_t getAddress();
  };
}