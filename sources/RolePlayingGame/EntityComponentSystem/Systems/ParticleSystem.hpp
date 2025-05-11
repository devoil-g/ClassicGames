#pragma once

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"

namespace RPG
{
  class ParticleSystem : public RPG::ECS::System
  {
  public:
    ParticleSystem() = delete;
    ParticleSystem(RPG::ECS& ecs);
    ParticleSystem(const ParticleSystem&) = delete;
    ParticleSystem(ParticleSystem&&) = delete;
    ~ParticleSystem() = default;

    ParticleSystem& operator=(const ParticleSystem&) = delete;
    ParticleSystem& operator=(ParticleSystem&&) = delete;

    void  execute(float elapsed);                           // Update existing particles
    void  execute(RPG::ECS::Entity entity, float elapsed);  // Update a single particle
  };
}