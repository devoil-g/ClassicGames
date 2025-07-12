#pragma once

#include <queue>

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/Components.hpp"
#include "RolePlayingGame/Types.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class ServerActionSystem : public RPG::ECS::System
  {
  private:
    void  handleMove(std::size_t id, const Game::JSON::Object& json); // Handle a move action

  public:
    ServerActionSystem() = delete;
    ServerActionSystem(RPG::ECS& ecs);
    ServerActionSystem(const ServerActionSystem&) = delete;
    ServerActionSystem(ServerActionSystem&&) = delete;
    ~ServerActionSystem() = default;

    ServerActionSystem& operator=(const ServerActionSystem&) = delete;
    ServerActionSystem& operator=(ServerActionSystem&&) = delete;

    void  execute(float elapsed); // Update and execute actions

    void  handlePacket(std::size_t id, const Game::JSON::Object& json); // Handle a packet
  };

  class ClientActionSystem : public RPG::ECS::System
  {
  private:
    std::queue<std::size_t>  _blocking;  // Indexes of blocking actions

    void  handleMove(const Game::JSON::Object& json); // Handle a move action

  public:
    ClientActionSystem() = delete;
    ClientActionSystem(RPG::ECS& ecs);
    ClientActionSystem(const ClientActionSystem&) = delete;
    ClientActionSystem(ClientActionSystem&&) = delete;
    ~ClientActionSystem() = default;

    ClientActionSystem& operator=(const ClientActionSystem&) = delete;
    ClientActionSystem& operator=(ClientActionSystem&&) = delete;

    void  execute(float elapsed); // Update and execute actions

    void  handlePacket(const Game::JSON::Object& json); // Handle a packet
  };

  class ServerMoveAction : public RPG::ServerActionComponent::Action
  {
  private:
    RPG::Coordinates  _target;      // Target of move
    bool              _interrupted; // True if move should stop

  public:
    ServerMoveAction() = delete;
    ServerMoveAction(RPG::ECS& ecs, RPG::ECS::Entity self, const Game::JSON::Object& json);
    ServerMoveAction(const ServerMoveAction&) = delete;
    ServerMoveAction(ServerMoveAction&&) = delete;
    ~ServerMoveAction() = default;

    ServerMoveAction& operator=(const ServerMoveAction&) = delete;
    ServerMoveAction& operator=(ServerMoveAction&&) = delete;

    void  atWait() override;    // Start execution
    void  atCommand() override; // Should not happen
    void  atExecute() override; // Move one step

    void  interrupt() override; // Stop move
  };

  class ClientMoveAction : public RPG::ClientActionComponent::Action
  {
  private:
    RPG::Coordinates  _targetCoordinates;    // Final move target coordinates
    RPG::Coordinates  _moveCoordinates;
    RPG::Position     _movePosition;  // Targeted position in cell
    RPG::Direction    _moveDirection;
    float             _remaining; // Time before end of move

  public:
    ClientMoveAction() = delete;
    ClientMoveAction(RPG::ECS& ecs, RPG::ECS::Entity self, const Game::JSON::Object& json);
    ClientMoveAction(const ClientMoveAction&) = delete;
    ClientMoveAction(ClientMoveAction&&) = delete;
    ~ClientMoveAction() = default;

    ClientMoveAction& operator=(const ClientMoveAction&) = delete;
    ClientMoveAction& operator=(ClientMoveAction&&) = delete;

    float update(float elapsed) override; // Update move action



  };

  /*
  class MovingSystem : public RPG::ECS::System
  {
  public:
    MovingSystem() = default;
    MovingSystem(const MovingSystem&) = delete;
    MovingSystem(MovingSystem&&) = delete;
    ~MovingSystem() = default;

    MovingSystem& operator=(const MovingSystem&) = delete;
    MovingSystem& operator=(MovingSystem&&) = delete;

    void  setMove(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, RPG::ECS::Entity entity, Math::Vector<3, float> position, float duration);

    void  execute(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, float elapsed); // Update move of entities
  };
  */
}