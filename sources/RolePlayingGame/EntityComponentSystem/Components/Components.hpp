#pragma once

#include <memory>

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"

namespace RPG
{
  class ActionComponent
  {
  public:
    class IAction
    {
    public:
      virtual ~IAction() = default;

      virtual void  atWait(RPG::ECS& ecs, RPG::ECS::Entity self) = 0;     // Take action after "Wait" time
      virtual void  atCommand(RPG::ECS& ecs, RPG::ECS::Entity self) = 0;  // Take action after "Command" time
      virtual void  atExecute(RPG::ECS& ecs, RPG::ECS::Entity self) = 0;  // Take action after "Execute" time
    };

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

    std::unique_ptr<IAction>  action; // Action to execute, null if no action
    std::unique_ptr<IAction>  next;   // Next action to execute, null if no action
    Mode                      mode;   // Current mode
    float                     wait;   // Wait time before action
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