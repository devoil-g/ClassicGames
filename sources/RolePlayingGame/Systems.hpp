#pragma once

#include <unordered_map>

#include "RolePlayingGame/EntityComponentSystem.hpp"
#include "RolePlayingGame/World.hpp"
#include "RolePlayingGame/Level.hpp"
#include "RolePlayingGame/Model.hpp"
#include "RolePlayingGame/Texture.hpp"

namespace RPG
{
  class DrawingSystem : public RPG::ECS::System
  {
  private:
    std::unordered_map<std::string, RPG::Texture> _textures;  // Loaded textures

    const RPG::Texture& getTexture(const std::string& name);

  public:
    DrawingSystem() = default;
    DrawingSystem(const DrawingSystem&) = delete;
    DrawingSystem(DrawingSystem&&) = delete;
    ~DrawingSystem() = default;

    DrawingSystem&  operator=(const DrawingSystem&) = delete;
    DrawingSystem&  operator=(DrawingSystem&&) = delete;

    void  execute(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level);  // Draw entities
  };

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

  class AnimatingSystem : public RPG::ECS::System
  {
  private:
    std::unordered_map<std::string, RPG::Model> _models;  // Registered models

    const RPG::Model& getModel(const std::string& name) const;

  public:
    AnimatingSystem() = default;
    AnimatingSystem(const AnimatingSystem&) = delete;
    AnimatingSystem(AnimatingSystem&&) = delete;
    ~AnimatingSystem() = default;

    AnimatingSystem&  operator=(const AnimatingSystem&) = delete;
    AnimatingSystem&  operator=(AnimatingSystem&&) = delete;

    void  setModel(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, RPG::ECS::Entity entity, const std::string& name);
    void  setModel(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, RPG::ECS::Entity entity, const RPG::Model& model);
    void  setAnimation(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, RPG::ECS::Entity entity, const std::string& name, bool loop);
    void  setAnimation(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, RPG::ECS::Entity entity, const RPG::Animation& animation, bool loop);
    void  setRandomAnimation(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, RPG::ECS::Entity entity, bool loop);

    void  execute(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, float elapsed); // Update animation of entities
  };

  // Simulate particles
  class ParticleSystem : public RPG::ECS::System
  {
  public:
    ParticleSystem() = default;
    ParticleSystem(const ParticleSystem&) = delete;
    ParticleSystem(ParticleSystem&&) = delete;
    ~ParticleSystem() = default;

    ParticleSystem& operator=(const ParticleSystem&) = delete;
    ParticleSystem& operator=(ParticleSystem&&) = delete;

    void  updateParticle(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, float elapsed, RPG::ECS::Entity entity); // Update a single particle

    void  execute(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, float elapsed);  // Update existing particles
  };

  // Generate particles from emitter
  class ParticleEmitterSystem : public RPG::ECS::System
  {
  public:
    ParticleEmitterSystem() = default;
    ParticleEmitterSystem(const ParticleSystem&) = delete;
    ParticleEmitterSystem(ParticleSystem&&) = delete;
    ~ParticleEmitterSystem() = default;

    ParticleEmitterSystem& operator=(const ParticleEmitterSystem&) = delete;
    ParticleEmitterSystem& operator=(ParticleEmitterSystem&&) = delete;

    void  updateParticleEmitter(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, float elapsed, RPG::ECS::Entity entity); // Update a single particle emitter

    void  execute(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, float elapsed);  // Update existing particle emitters
  };
}