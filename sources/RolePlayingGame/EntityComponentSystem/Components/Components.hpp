#pragma once

#include <memory>
#include <queue>

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"

namespace RPG
{
  class ActionComponent
  {
  public:
    enum class Mode {
      Wait,     // Waiting before taking command
      Command,  // Casting an action
      Execute   // Executing an action
    };

    ActionComponent();
    ActionComponent(const ActionComponent&) = default;
    ActionComponent(ActionComponent&&) = default;
    ~ActionComponent() = default;

    ActionComponent&  operator=(const ActionComponent&) = default;
    ActionComponent&  operator=(ActionComponent&&) = default;

    Mode                      mode;   // Current mode
    float                     wait;   // Wait time before action
  };

  class ServerActionComponent : public ActionComponent
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

      virtual void  atWait() = 0;     // Take action after "Wait" time
      virtual void  atCommand() = 0;  // Take action after "Command" time
      virtual void  atExecute() = 0;  // Take action after "Execute" time

      virtual void  interrupt() = 0;  // Request action to stop
    };

    ServerActionComponent();
    ServerActionComponent(const ServerActionComponent&) = default;
    ServerActionComponent(ServerActionComponent&&) = default;
    ~ServerActionComponent() = default;

    ServerActionComponent& operator=(const ServerActionComponent&) = default;
    ServerActionComponent& operator=(ServerActionComponent&&) = default;

    std::unique_ptr<Action> action; // Action to execute, null if no action
    std::unique_ptr<Action> next;   // Next action to execute, null if no action
  };

  class ClientActionComponent : public ActionComponent
  {
  public:
    class Action
    {
    private:
      static std::size_t  IndexGenerator; // Generator of action index

    public:
      RPG::ECS&               ecs;    // Current entity component system
      const RPG::ECS::Entity  self;   // Entity of action
      const std::size_t       index;  // Index of action

      Action() = delete;
      Action(RPG::ECS& ecs, RPG::ECS::Entity self);
      Action(const Action&) = delete;
      Action(Action&&) = delete;
      virtual ~Action() = default;

      virtual float update(float elapsed) = 0;  // Update action, return not consummed elapsed time
    };

    ClientActionComponent();
    ClientActionComponent(const ClientActionComponent&) = default;
    ClientActionComponent(ClientActionComponent&&) = default;
    ~ClientActionComponent() = default;

    ClientActionComponent& operator=(const ClientActionComponent&) = default;
    ClientActionComponent& operator=(ClientActionComponent&&) = default;

    std::list<std::unique_ptr<Action>>  actions;  // Actions to perform
  };

  /*
  class MoveComponent
  {
  public:
    Math::Vector<3, float>  position;   // Target position
    float                   remaining;  // Remaining time

    MoveComponent();
    MoveComponent(const MoveComponent&) = default;
    MoveComponent(MoveComponent&&) = default;
    ~MoveComponent() = default;

    MoveComponent&  operator=(const MoveComponent&) = default;
    MoveComponent&  operator=(MoveComponent&&) = default;
  };

  class CoordinatesComponent
  {
  public:
    Math::Vector<2, int>  coordinates;  // Coordinates of entity in level

    CoordinatesComponent();
    CoordinatesComponent(const CoordinatesComponent&) = default;
    CoordinatesComponent(CoordinatesComponent&&) = default;
    ~CoordinatesComponent() = default;

    CoordinatesComponent& operator=(const CoordinatesComponent&) = default;
    CoordinatesComponent& operator=(CoordinatesComponent&&) = default;
  };
  */
}