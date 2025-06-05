#pragma once

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/Components.hpp"
#include "RolePlayingGame/Types.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class ActionSystem : public RPG::ECS::System
  {
  public:
    ActionSystem() = delete;
    ActionSystem(RPG::ECS& ecs);
    ActionSystem(const ActionSystem&) = delete;
    ActionSystem(ActionSystem&&) = delete;
    ~ActionSystem() = default;

    ActionSystem& operator=(const ActionSystem&) = delete;
    ActionSystem& operator=(ActionSystem&&) = delete;

    void  execute(float elapsed); // Update and execute actions

    void  handlePacket(std::size_t id, const Game::JSON::Object& json); // Handle a packet
    void  handleMove(std::size_t id, const Game::JSON::Object& json);   // Handle a move action
  };

  class MoveAction : public RPG::ActionComponent::IAction
  {
  private:
    RPG::Coordinates  _coordinates; // Target of move

  public:
    MoveAction() = delete;
    MoveAction(RPG::ECS& ecs, RPG::ECS::Entity self, const Game::JSON::Object& json);
    MoveAction(const MoveAction&) = delete;
    MoveAction(MoveAction&&) = delete;
    ~MoveAction();

    MoveAction& operator=(const MoveAction&) = delete;
    MoveAction& operator=(MoveAction&&) = delete;

    virtual void  atWait(RPG::ECS& ecs, RPG::ECS::Entity self) override;    // Take action after "Wait" time
    virtual void  atCommand(RPG::ECS& ecs, RPG::ECS::Entity self) override; // Take action after "Command" time
    virtual void  atExecute(RPG::ECS& ecs, RPG::ECS::Entity self) override; // Take action after "Execute" time
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