#pragma once

#include <list>
#include <string>
#include <unordered_map>

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"
#include "RolePlayingGame/TcpClient.hpp"
#include "RolePlayingGame/TcpServer.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class Server;
  class ClientScene;

  class NetworkSystem : public RPG::ECS::System
  {
  public:
    class Player
    {
    public:
      static const std::wstring DefaultName;
      static const Player       ErrorPlayer;

    public:
      std::wstring  name; // Pseudo of the player
      
      Player();
      Player(const Game::JSON::Object& json);
      Player(const Player&) = default;
      Player(Player&&) = default;
      ~Player() = default;

      Player& operator=(const Player&) = default;
      Player& operator=(Player&&) = default;

      Game::JSON::Object  json() const; // Serialize player informations
    };

  protected:
    std::unordered_map<std::size_t, Player> _players; // Description of each player

  public:
    NetworkSystem() = delete;
    NetworkSystem(RPG::ECS& ecs);
    NetworkSystem(const NetworkSystem&) = delete;
    NetworkSystem(NetworkSystem&&) = delete;
    ~NetworkSystem() = default;

    const Player& getPlayer(std::size_t controller) const;      // Get player informations
    std::size_t   getController(RPG::ECS::Entity entity) const; // Get controller of entity

    NetworkSystem&  operator=(const NetworkSystem&) = delete;
    NetworkSystem&  operator=(NetworkSystem&&) = delete;
  };

  class ServerNetworkSystem : public RPG::NetworkSystem, public RPG::TcpServer
  {
  private:
    std::size_t _tick;  // Tick count

    virtual void  onConnect(std::size_t id) override;                                 // Called when a new TCP client connect
    virtual void  onDisconnect(std::size_t id) override;                              // Called when a TCP client disconnect
    virtual void  onReceive(std::size_t id, const Game::JSON::Object& json) override; // Called when a packet is received from TCP client 
    virtual void  onTick() override;                                                  // Called once per tick

    void  header(Game::JSON::Object& json, const std::vector<std::wstring>& type) const;  // Add type and tick to packet

  public:
    ServerNetworkSystem() = delete;
    ServerNetworkSystem(RPG::ECS& ecs, std::uint16_t port = 0, std::uint32_t address = 0);
    ServerNetworkSystem(const ServerNetworkSystem&) = delete;
    ServerNetworkSystem(ServerNetworkSystem&&) = delete;
    ~ServerNetworkSystem() = default;

    ServerNetworkSystem&  operator=(const ServerNetworkSystem&) = delete;
    ServerNetworkSystem&  operator=(ServerNetworkSystem&&) = delete;

    void  send(std::size_t id, const std::vector<std::wstring>& type, Game::JSON::Object& json);  // Send JSON to client, add type and tick
    void  broadcast(const std::vector<std::wstring>& type, Game::JSON::Object& json);             // Broadcast JSON to every TCP client, add type and tick
    void  kick(std::size_t id);                                                                   // Kick client
  };

  class ClientNetworkSystem : public RPG::NetworkSystem, public RPG::TcpClient
  {
  private:
    std::size_t                 _self;        // ID of player
    RPG::ECS::Entity            _controlled;  // Entity currently controlled
    std::list<RPG::ECS::Entity> _assigned;    // Entities assigned to player

    void  handlePacket(const Game::JSON::Object& json);      // Handle a packet
    void  handleConnect(const Game::JSON::Object& json);     // New player join the game
    void  handleDisconnect(const Game::JSON::Object& json);  // A player leave the game
    void  handleAssign(const Game::JSON::Object& json);      // Assign an entity to a player
    void  handlePlayer(const Game::JSON::Object& json);      // Update player informations

  public:
    ClientNetworkSystem() = delete;
    ClientNetworkSystem(RPG::ECS& ecs, std::uint16_t port, std::uint32_t address);
    ClientNetworkSystem(const ClientNetworkSystem&) = delete;
    ClientNetworkSystem(ClientNetworkSystem&&) = delete;
    ~ClientNetworkSystem() = default;

    ClientNetworkSystem&  operator=(const ClientNetworkSystem&) = delete;
    ClientNetworkSystem&  operator=(ClientNetworkSystem&&) = delete;

    void  receive();                                                              // Receive every pending packets
    void  send(const std::vector<std::wstring>& type, Game::JSON::Object& json);  // Send a JSON to server
    void  send();                                                                 // Send pending packets

    void  executeUpdate(float elapsed); // Update controller bar
    void  executeDraw();                // Draw controller bar

    void                        select(RPG::ECS::Entity entity);          // Select an entity to be controlled
    RPG::ECS::Entity            selected() const;                         // Get selected entity
    std::list<RPG::ECS::Entity> assigned() const;                         // Get every assigned entities
    bool                        assigned(RPG::ECS::Entity entity) const;  // Check if an entity is assigned to player
  };
}