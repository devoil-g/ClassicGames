#pragma once

#include "RolePlayingGame/EntityComponentSystem.hpp"
#include "RolePlayingGame/World.hpp"
#include "RolePlayingGame/Level.hpp"

namespace RPG
{
  class DrawingSystem : public RPG::ECS::System
  {
  private:
    
  public:
    DrawingSystem() = default;
    DrawingSystem(const DrawingSystem&) = delete;
    DrawingSystem(DrawingSystem&&) = delete;
    ~DrawingSystem() = default;

    DrawingSystem&  operator=(const DrawingSystem&) = delete;
    DrawingSystem&  operator=(DrawingSystem&&) = delete;

    void  draw(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level); // Draw every entity
  };
}