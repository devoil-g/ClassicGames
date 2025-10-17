#include "RolePlayingGame/EntityComponentSystem/Systems/ModelSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ParticleEmitterSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ParticleSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ModelComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ParticleComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ParticleEmitterComponent.hpp"

RPG::ParticleEmitterSystem::ParticleEmitterSystem(RPG::ECS& ecs) :
  RPG::ECS::System(ecs)
{}

void  RPG::ParticleEmitterSystem::execute(float elapsed)
{
  // Update each particle emitter
  for (auto entity : entities)
    execute(entity, elapsed);
}

void  RPG::ParticleEmitterSystem::execute(RPG::ECS::Entity entity, float elapsed)
{
  auto& emitter = ecs.getComponent<RPG::ParticleEmitterComponent>(entity);
  auto& particleSystem = ecs.getSystem<RPG::ParticleSystem>();
  auto& modelSystem = ecs.getSystem<RPG::ClientModelSystem>();

  // Emit particles
  while (true)
  {
    // No more particle to emit
    if (elapsed <= emitter.next || emitter.duration <= emitter.next) {
      emitter.duration = std::max(emitter.duration - elapsed, 0.f);
      emitter.next = std::max(emitter.next - elapsed, 0.f);
      break;
    }

    // Update timers
    emitter.duration -= emitter.next;
    elapsed -= emitter.next;
    emitter.next = 1.f / Math::Random(emitter.frequencyLow, emitter.frequencyHigh);

    auto particle = ecs.createEntity();

    // Generate particle
    ecs.addComponent<RPG::ModelComponent>(particle);
    ecs.addComponent<RPG::ParticleComponent>(particle);
    
    auto& particleModel = ecs.getComponent<RPG::ModelComponent>(particle);
    auto& particleComponent = ecs.getComponent<RPG::ParticleComponent>(particle);

    // Generate random position
    auto direction = Math::Vector<3>(
      Math::Random() - 0.5f, Math::Random() - 0.5f, Math::Random() - 0.5f
    );

    auto length = direction.length();

    // Normalize random direction
    if (length != 0.f)
      direction *= std::cbrt(Math::Random()) / length;

    // Randomize initial position
    particleModel.position = emitter.position + direction * emitter.size / 2;
    particleModel.layer = RPG::ModelComponent::Layer::LayerEntity;

    // Set particle model
    modelSystem.setModel(particle, L"particles");
    modelSystem.setAnimation(particle, emitter.animation, RPG::Model::Actor::Mode::Loop);

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
    modelSystem.executeAnimation(particle, elapsed);
    particleSystem.execute(particle, elapsed);
  }
}