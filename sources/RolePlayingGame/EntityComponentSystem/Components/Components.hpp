#pragma once

#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <queue>
#include <string>

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"
#include "RolePlayingGame/Types.hpp"

namespace RPG
{
  class ServerActionComponent
  {
  public:
    class Action
    {
    public:
      RPG::ECS&               ecs;  // Current entity component system
      const RPG::ECS::Entity  self; // Entity of action

      Action() = delete;
      Action(RPG::ECS& ecs, RPG::ECS::Entity self);
      Action(const Action&) = delete;
      Action(Action&&) = delete;
      virtual ~Action() = default;

      Action& operator=(const Action&) = delete;
      Action& operator=(Action&&) = delete;

      virtual Game::JSON::Object  json() const = 0; // Serialize action to JSON

      virtual void  execute() = 0;    // Execute action
      virtual void  refresh() = 0;    // Refresh action properties (ex: recompute speed)
      virtual void  interrupt() = 0;  // Request action to stop
    };

    ServerActionComponent();
    ServerActionComponent(const ServerActionComponent&) = default;
    ServerActionComponent(ServerActionComponent&&) = default;
    ~ServerActionComponent() = default;

    ServerActionComponent& operator=(const ServerActionComponent&) = default;
    ServerActionComponent& operator=(ServerActionComponent&&) = default;

    RPG::ActionMode mode;       // Action mode
    float           start, end; // Position in action bar at start/end of the action [0; +1]
    float           progress;   // Progress of current action [0; +1]
    float           speed;      // Progress speed

    std::unique_ptr<Action>                                 action; // Action to execute, null if no action
    std::optional<std::function<std::unique_ptr<Action>()>> next;   // Builder of next action
  };

  class ClientActionComponent
  {
  public:
    class Action
    {
    public:
      RPG::ECS&               ecs;    // Current entity component system
      const RPG::ECS::Entity  self;   // Entity of action

      Action() = delete;
      Action(RPG::ECS& ecs, RPG::ECS::Entity self);
      Action(const Action&) = delete;
      Action(Action&&) = delete;
      virtual ~Action() = default;

      virtual void  update(float elapsed) = 0;  // Update action
    };

    ClientActionComponent();
    ClientActionComponent(const ClientActionComponent&) = default;
    ClientActionComponent(ClientActionComponent&&) = default;
    ~ClientActionComponent() = default;

    ClientActionComponent& operator=(const ClientActionComponent&) = default;
    ClientActionComponent& operator=(ClientActionComponent&&) = default;

    RPG::ActionMode mode;       // Action mode
    float           start, end; // Position in action bar at start/end of the action [0; +1]
    float           progress;   // Progress of current action [0; +1]
    float           speed;

    struct NextAction
    {
    public:
      float                                     clock;     // Time at which the action begin
      std::function<std::unique_ptr<Action>()>  builder;  // Action builder

      NextAction() = delete;
      NextAction(float clock, std::function<std::unique_ptr<Action>()>&& builder);
      NextAction(const NextAction&) = delete;
      NextAction(NextAction&&) = delete;
      ~NextAction() = default;

      NextAction& operator=(const NextAction&) = delete;
      NextAction& operator=(NextAction&&) = delete;
    };

    std::unique_ptr<Action> action; // Current action
    std::queue<NextAction>  next;   // Next actions
  };
}