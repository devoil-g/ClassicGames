#pragma once

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"

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

    void  execute(float elapsed);
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