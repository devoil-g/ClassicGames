#include "RolePlayingGame/EntityComponentSystem/Systems/DisplaySystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ParticleEmitterSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ParticleSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/DisplayComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ParticleComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ParticleEmitterComponent.hpp"

void  RPG::ParticleEmitterSystem::execute(RPG::ECS& ecs, float elapsed)
{
  // Update each particle emitter
  for (auto entity : entities)
    executeParticleEmitter(ecs, entity, elapsed);
}

void  RPG::ParticleEmitterSystem::executeParticleEmitter(RPG::ECS& ecs, RPG::ECS::Entity entity, float elapsed)
{
  auto& emitter = ecs.getComponent<RPG::ParticleEmitterComponent>(entity);
  auto& particleSystem = ecs.getSystem<RPG::ParticleSystem>();
  auto& displaySystem = ecs.getSystem<RPG::ClientDisplaySystem>();

  // Emit particles
  while (true)
  {
    // No more particle to emit
    if (elapsed < emitter.next || emitter.duration < emitter.next) {
      emitter.duration -= elapsed;
      emitter.next -= elapsed;
      break;
    }

    // Update timers
    emitter.duration -= emitter.next;
    elapsed -= emitter.next;
    emitter.next = 1.f / Math::Random(emitter.frequencyLow, emitter.frequencyHigh);

    auto particle = ecs.createEntity();

    // Generate particle
    ecs.addComponent<RPG::DisplayComponent>(particle);
    ecs.addComponent<RPG::ParticleComponent>(particle);
    
    auto& particleDisplay = ecs.getComponent<RPG::DisplayComponent>(particle);
    auto& particleComponent = ecs.getComponent<RPG::ParticleComponent>(particle);

    // Randomize initial position
    particleDisplay.position =
      emitter.position
      + RPG::Position(Math::Random(emitter.size.x()), Math::Random(emitter.size.y()), Math::Random(emitter.size.z()))
      - (emitter.size / 2.f);

    // Set particle model
    displaySystem.setModel(ecs, particle, emitter.model);
    displaySystem.setRandomAnimation(ecs, particle, true);

    // Randomize particle
    particleComponent.physicsSpeed = Math::Vector<3>(
      Math::Random(emitter.particleLow.physicsSpeed.x(), emitter.particleHigh.physicsSpeed.x()),
      Math::Random(emitter.particleLow.physicsSpeed.y(), emitter.particleHigh.physicsSpeed.y()),
      Math::Random(emitter.particleLow.physicsSpeed.z(), emitter.particleHigh.physicsSpeed.z())
    );
    particleComponent.physicsGravity = Math::Vector<3>(
      Math::Random(emitter.particleLow.physicsGravity.x(), emitter.particleHigh.physicsGravity.x()),
      Math::Random(emitter.particleLow.physicsGravity.y(), emitter.particleHigh.physicsGravity.y()),
      Math::Random(emitter.particleLow.physicsGravity.z(), emitter.particleHigh.physicsGravity.z())
    );
    particleComponent.physicsDrag = Math::Random(emitter.particleLow.physicsDrag, emitter.particleHigh.physicsDrag);
    particleComponent.physicsFloor = Math::Random(emitter.particleLow.physicsFloor, emitter.particleHigh.physicsFloor);
    particleComponent.colorStart = RPG::Color(
      Math::Random(emitter.particleLow.colorStart.red, emitter.particleHigh.colorStart.red),
      Math::Random(emitter.particleLow.colorStart.green, emitter.particleHigh.colorStart.green),
      Math::Random(emitter.particleLow.colorStart.blue, emitter.particleHigh.colorStart.blue),
      Math::Random(emitter.particleLow.colorStart.alpha, emitter.particleHigh.colorStart.alpha)
    );
    particleComponent.colorEnd = RPG::Color(
      Math::Random(emitter.particleLow.colorEnd.red, emitter.particleHigh.colorEnd.red),
      Math::Random(emitter.particleLow.colorEnd.green, emitter.particleHigh.colorEnd.green),
      Math::Random(emitter.particleLow.colorEnd.blue, emitter.particleHigh.colorEnd.blue),
      Math::Random(emitter.particleLow.colorEnd.alpha, emitter.particleHigh.colorEnd.alpha)
    );
    particleComponent.durationFadeIn = Math::Random(emitter.particleLow.durationFadeIn, emitter.particleHigh.durationFadeIn);
    particleComponent.durationLife = Math::Random(emitter.particleLow.durationLife, emitter.particleHigh.durationLife);
    particleComponent.durationFadeOut = Math::Random(emitter.particleLow.durationFadeOut, emitter.particleHigh.durationFadeOut);

    // Update particle timer
    displaySystem.executeAnimation(ecs, particle, elapsed);
    particleSystem.executeParticle(ecs, particle, elapsed);
  }
}