#pragma once

#include <list>
#include <unordered_map>

#include "RolePlayingGame/TcpServer.hpp"
#include "RolePlayingGame/Level.hpp"

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


      SCFileSend,     // Send file to client, std::string filename, std::size_t size, void* data
      SCFileInfo,     // Send file info to client, std::string filename, std::size_t size, std::size_t hash
      SCFileUnknown,  // Requested file does not exist, std::string filename


      // Load a new level to client
      SCLoadMask = 0x01000000,

      SCLoadStart,  // Start level loading
      SCLoadCell,   // Load a cell, std::int32_t x, std::int32_t y, ... 
      SCLoadEntity, // Load an entity
      SCLoadEnd,    // End level loading

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

    std::list<RPG::GameServer::Client>          _clients;
    std::unordered_map<std::string, RPG::Level> _levels;  // Levels accessible by name

    void  handleFileRequest(std::size_t id, sf::Packet& packet);  // Request server to send a file
    void  handleFileInfo(std::size_t id, sf::Packet& packet);     // Request server to send file info

    virtual void  onConnect(std::size_t id) override;                     // Called when a new TCP client connect
    virtual void  onDisconnect(std::size_t id) override;                  // Called when a TCP client disconnect
    virtual void  onReceive(std::size_t id, sf::Packet& packet) override; // Called when a packet is received from TCP client
    virtual void  onTick() override;                                      // Called once per tick

  public:
    GameServer();
    ~GameServer();
  };
}