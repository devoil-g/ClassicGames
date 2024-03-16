#pragma once

#include <filesystem>
#include <list>
#include <unordered_map>

#include "RolePlayingGame/TcpServer.hpp"
#include "RolePlayingGame/Level.hpp"

#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  namespace RFC
  {
    enum ClientToServer : std::uint32_t
    {
      CSFileRequest,  // Request a file, std::string filename
      CSFileInfo,     // Request file info, std::string filename

      CSCount
    };

    enum ServerToClient : std::uint32_t
    {
      StC_Mask = 0xFF000000,

      StC_FileMask = 0x01000000,

      StC_FileSend,     // Send file to client, std::string filename, std::size_t size, void* data
      StC_FileInfo,     // Send file info to client, std::string filename, std::size_t size, std::size_t hash
      StC_FileUnknown,  // Requested file does not exist, std::string filename

      // Load a new level to client
      StC_LoadMask = 0x01000000,

      StC_LoadStart,  // Start level loading
      StC_LoadCell,   // Load a cell, std::int32_t x, std::int32_t y, ... 
      StC_LoadEntity, // Load an entity
      StC_LoadEnd,    // End level loading

      SCCount
    };
  }

  class GameServer : public TcpServer
  {
  private:
    class Client
    {

    public:
      const std::size_t id;
      
      Client(std::size_t id);
    };

    std::list<RPG::GameServer::Client>  _clients;
    std::size_t                         _tick;
    RPG::Level                          _level;

    void  handleFileRequest(std::size_t id, sf::Packet& packet);  // Request server to send a file
    void  handleFileInfo(std::size_t id, sf::Packet& packet);     // Request server to send file info

    void  handleFileRequest(RPG::GameServer::Client& client, Game::JSON::Object& json); // Request server to send a file
    void  handleFileInfo(RPG::GameServer::Client& client, Game::JSON::Object& json);    // Request server to send file info

    virtual void  onConnect(std::size_t id) override;                                 // Called when a new TCP client connect
    virtual void  onDisconnect(std::size_t id) override;                              // Called when a TCP client disconnect
    virtual void  onReceive(std::size_t id, const Game::JSON::Object& json) override; // Called when a packet is received from TCP client
    virtual void  onTick() override;                                                  // Called once per tick

  public:
    GameServer(const std::filesystem::path& config, std::uint16_t port = 0, std::uint32_t address = 0);
    ~GameServer();

    void  send(std::size_t id, const std::string& type, Game::JSON::Object& json);  // Send JSON to client, add type and tick
    void  broadcast(const std::string& type, Game::JSON::Object& json);             // Broadcast JSON to every TCP client, add type and tick
    void  kick(std::size_t id);                                                     // Kick client

  };
}