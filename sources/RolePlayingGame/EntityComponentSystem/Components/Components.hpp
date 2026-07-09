#pragma once

#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <queue>
#include <string>

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"

namespace RPG
{
  class ActionComponent
  {
  public:
    enum class Mode {
      Command,  // Start an action
      Execute   // Executing an action
    };

    static constexpr std::array<std::wstring_view, 3> ModeNames = {
      L"command",
      L"action"
    };

    static Mode         StringToMode(const std::wstring& string);
    static std::wstring ModeToString(Mode mode);

    ActionComponent();
    ActionComponent(const ActionComponent&) = default;
    ActionComponent(ActionComponent&&) = default;
    ~ActionComponent() = default;

    ActionComponent&  operator=(const ActionComponent&) = default;
    ActionComponent&  operator=(ActionComponent&&) = default;

    Mode  mode;     // Current mode
    float active;   // Wait time (shown in action bar, decrease last)
    float passive;  // Wait time (not shown in action bar, decrease first)
  };

  class ServerActionComponent : public ActionComponent
  {
  public:
    class Action
    {
    public:
      RPG::ECS& ecs;  // Current entity component system
      const RPG::ECS::Entity  self; // Entity of action

      Action() = delete;
      Action(RPG::ECS& ecs, RPG::ECS::Entity self);
      Action(const Action&) = delete;
      Action(Action&&) = delete;
      virtual ~Action() = default;

      virtual void  atCommand() = 0;  // Take action after "Command" time
      virtual void  atAction() = 0;   // Take action after "Action" time

      virtual void  interrupt() = 0;  // Request action to stop
    };

    ServerActionComponent();
    ServerActionComponent(const ServerActionComponent&) = default;
    ServerActionComponent(ServerActionComponent&&) = default;
    ~ServerActionComponent() = default;

    ServerActionComponent& operator=(const ServerActionComponent&) = default;
    ServerActionComponent& operator=(ServerActionComponent&&) = default;

    std::unique_ptr<Action>                                 action; // Action to execute, null if no action
    std::optional<std::function<std::unique_ptr<Action>()>> next;   // Builder of next action
  };

  class ClientActionComponent : public ActionComponent
  {
  public:
    class Action
    {
    public:
      RPG::ECS&               ecs;    // Current entity component system
      const RPG::ECS::Entity  self;   // Entity of action
      const std::size_t       index;  // Index of action

      Action() = delete;
      Action(RPG::ECS& ecs, RPG::ECS::Entity self, std::size_t index);
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

    std::unique_ptr<Action>                               action; // Current action
    std::queue<std::function<std::unique_ptr<Action>()>>  next;   // Builders of next actions
  };
}