#pragma once

#include <iostream>
#include <queue>

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/Components.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/EntitySystem.hpp"
#include "RolePlayingGame/Types.hpp"
#include "System/JavaScriptObjectNotation.hpp"

#include "RolePlayingGame/TcpServer.hpp"

namespace RPG
{
  class ServerActionSystem : public RPG::ECS::System
  {
  private:
    float _clock; // Main action clock

    void  handleMove(std::size_t id, const Game::JSON::Object& json); // Handle a move action

    template<typename Action, typename ... Args>
    void  action(RPG::ECS::Entity entity, Args&& ... args)  // Push a new action to entity
    {
      auto& action = ecs.getComponent<RPG::ServerActionComponent>(entity);

      // Add move action to entity
      action.next = [this, entity, ... args = std::forward<Args>(args)]() {
        return std::make_unique<Action>(ecs, entity, args...);
        };

      // Interrupt previous action
      // TODO: remove this in combat mode
      if (action.action != nullptr)
        action.action->interrupt();
    }

  public:
    ServerActionSystem() = delete;
    ServerActionSystem(RPG::ECS& ecs);
    ServerActionSystem(const ServerActionSystem&) = delete;
    ServerActionSystem(ServerActionSystem&&) = delete;
    ~ServerActionSystem() = default;

    ServerActionSystem& operator=(const ServerActionSystem&) = delete;
    ServerActionSystem& operator=(ServerActionSystem&&) = delete;

    void  execute(float elapsed); // Update and execute actions

    Game::JSON::Array   jsonActions() const;                        // Serialize every entity actions
    Game::JSON::Object  jsonAction(RPG::ECS::Entity entity) const;  // Serialize an entity action
    Game::JSON::Object  jsonClock() const;                          // Serialize clock
    
    void  handlePacket(std::size_t id, const Game::JSON::Object& json); // Handle a packet
  };

  class ClientActionSystem : public RPG::ECS::System
  {
  public:
    static constexpr float  DefaultTimeout = RPG::TcpServer::DefaultTimeout;
    static constexpr float  DefaultBufferingMin = 2.f;
    static constexpr float  DefaultBufferingMax = 3.f + 10.f;

  private:
    float _clock;         // Current local clock of actions
    float _target;        // Last registered time of server
    float _timeout;       // Timeout of the server (maximal duration between two packets)
    float _bufferingMin;  // Minimal time (value * timeout) in buffer before slowing time
    float _bufferingMax;  // Maximal time (value * timeout) in buffer before accelerating time

    template<typename Action, typename ... Args>
    void  action(const Game::JSON::Object& json, Args&& ... args)  // Push a null action to entity
    {
      auto entity = ecs.getSystem<RPG::ClientEntitySystem>().getEntity(json.get(L"id").string());

      // Check entity exist
      if (entity == RPG::ECS::InvalidEntity) {
        std::wcerr << "[RPG::ClientActionSystem]: unknow entity '" << json.get(L"id").string() << "'." << std::endl;
        return;
      }

      auto& action = ecs.getComponent<RPG::ClientActionComponent>(entity);

      RPG::ActionMode mode = RPG::StringToActionMode(json.get(L"mode").string());
      float           clock = (float)json.get(L"clock").number();
      float           start = (float)json.get(L"start").number();
      float           end = (float)json.get(L"end").number();
      float           progress = (float)json.get(L"progress").number();
      float           speed = (float)json.get(L"speed").number();

      // Add action to entity
      action.next.emplace(clock,
        [this, entity, &action, mode, start, end, progress, speed, ... args = std::forward<Args>(args)]()
        {
          // Copy base properties of action
          action.mode = mode;
          action.start = start;
          action.end = end;
          action.progress = progress;
          action.speed = speed;

          // Specialized behavior
          return std::make_unique<Action>(ecs, entity, args...);
        });
    }

    void  handleClock(const Game::JSON::Object& json); // Handle a move action
    void  handleEntity(const Game::JSON::Object& json); // Handle a move action
    void  handleLoad(const Game::JSON::Object& json); // Handle a move action
    void  handleLoadEntities(const Game::JSON::Object& json); // Handle a move action

  public:
    ClientActionSystem() = delete;
    ClientActionSystem(RPG::ECS& ecs);
    ClientActionSystem(const ClientActionSystem&) = delete;
    ClientActionSystem(ClientActionSystem&&) = delete;
    ~ClientActionSystem();

    ClientActionSystem& operator=(const ClientActionSystem&) = delete;
    ClientActionSystem& operator=(ClientActionSystem&&) = delete;

    void  execute(float elapsed); // Update and execute actions

    void  handlePacket(const Game::JSON::Object& json); // Handle a packet

    void                    setTimeout(float timeout);          // Set maximal time between two ticks of the server
    float                   getTimeout() const;                 // Get maximal time between two ticks of the server
    void                    setBuffering(float min, float max); // Set minimal/maximal time of buffering
    std::pair<float, float> getBuffering() const;               // Get minimal/maximal time of buffering

  };

  class ServerMoveAction : public RPG::ServerActionComponent::Action
  {
  private:
    RPG::Coordinates  _target;  // Target of move

  public:
    ServerMoveAction() = delete;
    ServerMoveAction(RPG::ECS& ecs, RPG::ECS::Entity self, const RPG::Coordinates& target);
    ServerMoveAction(const ServerMoveAction&) = delete;
    ServerMoveAction(ServerMoveAction&&) = delete;
    ~ServerMoveAction() = default;

    ServerMoveAction& operator=(const ServerMoveAction&) = delete;
    ServerMoveAction& operator=(ServerMoveAction&&) = delete;

    Game::JSON::Object json() const;  // Serialize action to JSON

    void  execute() override;   // Execute action
    void  refresh() override;   // Refresh action properties (ex: recompute speed)
    void  interrupt() override; // Stop move on entity current cell
  };

  class ClientNullAction : public RPG::ClientActionComponent::Action
  {
  public:
    ClientNullAction() = delete;
    ClientNullAction(RPG::ECS& ecs, RPG::ECS::Entity self);
    ClientNullAction(const ClientNullAction&) = delete;
    ClientNullAction(ClientNullAction&&) = delete;
    ~ClientNullAction() = default;

    ClientNullAction& operator=(const ClientNullAction&) = delete;
    ClientNullAction& operator=(ClientNullAction&&) = delete;

    void  update(float elapsed) override; // Does nothing
  };

  class ClientMoveAction : public RPG::ClientActionComponent::Action
  {
  private:
    RPG::Coordinates  _target;        // Final move target coordinates
    RPG::Coordinates  _coordinates;   // Move destination coordinates
    RPG::Position     _position;      // Move destination position in cell
    RPG::Direction    _direction;     // Move direction
    float             _remaining;         // Duration of move

  public:
    ClientMoveAction() = delete;
    ClientMoveAction(RPG::ECS& ecs, RPG::ECS::Entity self, RPG::Coordinates target, RPG::Coordinates coordinates, RPG::Position position, RPG::Direction direction);
    ClientMoveAction(const ClientMoveAction&) = delete;
    ClientMoveAction(ClientMoveAction&&) = delete;
    ~ClientMoveAction();

    ClientMoveAction& operator=(const ClientMoveAction&) = delete;
    ClientMoveAction& operator=(ClientMoveAction&&) = delete;

    void  update(float elapsed) override; // Update move action
  };
}