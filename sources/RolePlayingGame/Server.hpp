#pragma once

#include <filesystem>
#include <list>
#include <vector>

#include "RolePlayingGame/TcpServer.hpp"
#include "RolePlayingGame/World.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class Server : public RPG::TcpServer
  {
  private:
    RPG::ServerWorld  _world;
    std::size_t       _tick;
    
    virtual void  onConnect(std::size_t id) override;                                 // Called when a new TCP client connect
    virtual void  onDisconnect(std::size_t id) override;                              // Called when a TCP client disconnect
    virtual void  onReceive(std::size_t id, const Game::JSON::Object& json) override; // Called when a packet is received from TCP client
    virtual void  onTick() override;                                                  // Called once per tick

    void  header(Game::JSON::Object& json, const std::vector<std::string>& type) const; // Add type and tick to packet

  public:
    Server(const std::filesystem::path& config, std::uint16_t port = 0, std::uint32_t address = 0);
    ~Server();

    void  send(std::size_t id, const std::vector<std::string>& type, Game::JSON::Object& json); // Send JSON to client, add type and tick
    void  broadcast(const std::vector<std::string>& type, Game::JSON::Object& json);            // Broadcast JSON to every TCP client, add type and tick
    void  kick(std::size_t id);                                                                 // Kick client
  };
}