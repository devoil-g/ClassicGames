#pragma once

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"

namespace RPG
{
  class ParticleEmitterSystem : public RPG::ECS::System
  {
  public:
    ParticleEmitterSystem() = delete;
    ParticleEmitterSystem(RPG::ECS& ecs);
    ParticleEmitterSystem(const ParticleEmitterSystem&) = delete;
    ParticleEmitterSystem(ParticleEmitterSystem&&) = delete;
    ~ParticleEmitterSystem() = default;

    ParticleEmitterSystem& operator=(const ParticleEmitterSystem&) = delete;
    ParticleEmitterSystem& operator=(ParticleEmitterSystem&&) = delete;

    void  execute(float elapsed);                           // Update existing particle emitters
    void  execute(RPG::ECS::Entity entity, float elapsed);  // Update a single particle emitter
  };
}