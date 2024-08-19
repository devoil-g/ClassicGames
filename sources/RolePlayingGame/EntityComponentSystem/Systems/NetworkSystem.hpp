#pragma once

#include <string>
#include <unordered_map>

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"
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
      static const std::string  DefaultName;
      static const Player       ErrorPlayer;

    public:
      std::string name; // Pseudo of the player
      
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
    NetworkSystem() = default;
    NetworkSystem(const NetworkSystem&) = delete;
    NetworkSystem(NetworkSystem&&) = delete;
    ~NetworkSystem() = default;

    const Player& getPlayer(std::size_t controller) const;  // Get player informations

    NetworkSystem& operator=(const NetworkSystem&) = delete;
    NetworkSystem& operator=(NetworkSystem&&) = delete;
  };

  class ServerNetworkSystem : public RPG::NetworkSystem
  {
  public:
    ServerNetworkSystem() = default;
    ServerNetworkSystem(const ServerNetworkSystem&) = delete;
    ServerNetworkSystem(ServerNetworkSystem&&) = delete;
    ~ServerNetworkSystem() = default;

    ServerNetworkSystem&  operator=(const ServerNetworkSystem&) = delete;
    ServerNetworkSystem&  operator=(ServerNetworkSystem&&) = delete;

    void  connect(RPG::ECS& ecs, RPG::Server& server, std::size_t id);    // New player join the game
    void  disconnect(RPG::ECS& ecs, RPG::Server& server, std::size_t id); // A player leave the game
  };

  class ClientNetworkSystem : public RPG::NetworkSystem
  {
  private:
    std::size_t self; // ID of player

  public:
    ClientNetworkSystem();
    ClientNetworkSystem(const ClientNetworkSystem&) = delete;
    ClientNetworkSystem(ClientNetworkSystem&&) = delete;
    ~ClientNetworkSystem() = default;

    ClientNetworkSystem& operator=(const ClientNetworkSystem&) = delete;
    ClientNetworkSystem& operator=(ClientNetworkSystem&&) = delete;

    void  handlePacket(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);      // Handle a packet
    void  handleConnect(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);     // New player join the game
    void  handleDisconnect(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);  // A player leave the game
    void  handleAssign(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);      // Assign an entity to a player
    void  handlePlayer(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);      // Update player informations
  };
}