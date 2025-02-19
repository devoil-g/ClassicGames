#include "RolePlayingGame/EntityComponentSystem/Systems/ParticleSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ModelComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ParticleComponent.hpp"

RPG::ParticleSystem::ParticleSystem(RPG::ECS& ecs)
{}

void RPG::ParticleSystem::execute(RPG::ECS& ecs, float elapsed)
{
  // Update each particle
  for (auto entity = entities.begin(); entity != entities.end();) {
    executeParticle(ecs, *entity, elapsed);

    const auto& particle = ecs.getComponent<RPG::ParticleComponent>(*entity);

    // Remove particle
    if (particle.elapsed >= particle.durationFadeIn + particle.durationLife + particle.durationFadeOut)
      ecs.destroyEntity(*(entity++));
    else
      entity++;
  }
}

void  RPG::ParticleSystem::executeParticle(RPG::ECS& ecs, RPG::ECS::Entity entity, float elapsed)
{
  auto& particle = ecs.getComponent<RPG::ParticleComponent>(entity);
  auto& model = ecs.getComponent<RPG::ModelComponent>(entity);
  float duration = particle.durationFadeIn + particle.durationLife + particle.durationFadeOut;

  // Increase timer
  particle.elapsed += elapsed;

  // End particle
  if (particle.elapsed >= duration) {
    model.color.alpha = 0;
    return;
  }

  // Update physics and position
  particle.physicsSpeed += particle.physicsGravity * elapsed;
  particle.physicsSpeed *= std::pow(1.f - particle.physicsDrag, elapsed);
  model.position += particle.physicsSpeed * elapsed;
  model.position.z() = std::max(particle.physicsFloor, model.position.z());

  // Update color
  model.color.red = (particle.colorStart.red + (particle.colorEnd.red - particle.colorStart.red) * (particle.elapsed / duration));
  model.color.green = (particle.colorStart.green + (particle.colorEnd.green - particle.colorStart.green) * (particle.elapsed / duration));
  model.color.blue = (particle.colorStart.blue + (particle.colorEnd.blue - particle.colorStart.blue) * (particle.elapsed / duration));
  model.color.alpha = (particle.colorStart.alpha + (particle.colorEnd.alpha - particle.colorStart.alpha) * (particle.elapsed / duration));

  // Fade in
  if (particle.elapsed < particle.durationFadeIn)
    model.color.alpha = (model.color.alpha * (particle.elapsed / particle.durationFadeIn));

  // Fade out
  else if (particle.elapsed > particle.durationFadeIn + particle.durationFadeOut)
    model.color.alpha = (model.color.alpha * (1.f - ((particle.elapsed - (particle.durationFadeIn + particle.durationLife)) / particle.durationFadeOut)));
}