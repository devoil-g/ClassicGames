#pragma once

#include <list>
#include <string>
#include <unordered_map>

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class Server;
  class ClientScene;

  class ControllerSystem : public RPG::ECS::System
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
    ControllerSystem() = delete;
    ControllerSystem(RPG::ECS& ecs);
    ControllerSystem(const ControllerSystem&) = delete;
    ControllerSystem(ControllerSystem&&) = delete;
    ~ControllerSystem() = default;

    const Player& getPlayer(std::size_t controller) const;  // Get player informations

    ControllerSystem& operator=(const ControllerSystem&) = delete;
    ControllerSystem& operator=(ControllerSystem&&) = delete;
  };

  class ServerControllerSystem : public RPG::ControllerSystem
  {
  public:
    ServerControllerSystem() = delete;
    ServerControllerSystem(RPG::ECS& ecs);
    ServerControllerSystem(const ServerControllerSystem&) = delete;
    ServerControllerSystem(ServerControllerSystem&&) = delete;
    ~ServerControllerSystem() = default;

    ServerControllerSystem&  operator=(const ServerControllerSystem&) = delete;
    ServerControllerSystem&  operator=(ServerControllerSystem&&) = delete;

    void  connect(RPG::ECS& ecs, RPG::Server& server, std::size_t id);    // New player join the game
    void  disconnect(RPG::ECS& ecs, RPG::Server& server, std::size_t id); // A player leave the game
  };

  class ClientControllerSystem : public RPG::ControllerSystem
  {
  private:
    std::size_t                 _self;        // ID of player
    RPG::ECS::Entity            _controlled;  // Entity currently controlled
    std::list<RPG::ECS::Entity> _assigned;    // Entities assigned to player

  public:
    ClientControllerSystem() = delete;
    ClientControllerSystem(RPG::ECS& ecs);
    ClientControllerSystem(const ClientControllerSystem&) = delete;
    ClientControllerSystem(ClientControllerSystem&&) = delete;
    ~ClientControllerSystem() = default;

    ClientControllerSystem& operator=(const ClientControllerSystem&) = delete;
    ClientControllerSystem& operator=(ClientControllerSystem&&) = delete;

    void  handlePacket(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);      // Handle a packet
    void  handleConnect(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);     // New player join the game
    void  handleDisconnect(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);  // A player leave the game
    void  handleAssign(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);      // Assign an entity to a player
    void  handlePlayer(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);      // Update player informations

    void  executeUpdate(RPG::ECS& ecs, float elapsed);  // Update controller bar
    void  executeDraw(RPG::ECS& ecs);                   // Draw controller bar

    void  select(RPG::ECS& ecs, RPG::ECS::Entity entity); // Select an entity to be controlled
  };
}