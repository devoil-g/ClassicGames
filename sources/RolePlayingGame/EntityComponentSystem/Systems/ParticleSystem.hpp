#pragma once

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"

namespace RPG
{
  class ParticleSystem : public RPG::ECS::System
  {
  public:
    ParticleSystem() = default;
    ParticleSystem(const ParticleSystem&) = delete;
    ParticleSystem(ParticleSystem&&) = delete;
    ~ParticleSystem() = default;

    ParticleSystem& operator=(const ParticleSystem&) = delete;
    ParticleSystem& operator=(ParticleSystem&&) = delete;

    void  execute(RPG::ECS& ecs, float elapsed);                                  // Update existing particles
    void  executeParticle(RPG::ECS& ecs, RPG::ECS::Entity entity, float elapsed); // Update a single particle
  };
}