#include "RolePlayingGame/Systems.hpp"
#include "RolePlayingGame/Components.hpp"
#include "RolePlayingGame/Level.hpp"
#include "RolePlayingGame/World.hpp"

void  RPG::DrawingSystem::execute(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level)
{
  std::array<RPG::ECS::Entity, RPG::ECS::MaxEntities> drawables;
  std::size_t                                         count;

  // Reset drawables array
  count = 0;

  // Get every drawables in an array
  for (auto entity : entities) {
    drawables[count] = entity;
    count += 1;
  }

  // Sort drawables by depth
  std::sort(drawables.begin(), drawables.begin() + count, [&ecs, &level](auto aEntity, auto bEntity) {
    const auto  aPosition = ecs.getComponent<RPG::PositionComponent>(aEntity).position;
    const auto  bPosition = ecs.getComponent<RPG::PositionComponent>(bEntity).position;

    if (aPosition.y() < bPosition.y())
      return true;
    if (aPosition.y() > bPosition.y())
      return false;
    if (aPosition.z() < bPosition.z())
      return true;
    if (aPosition.z() > bPosition.z())
      return false;
    return aPosition.x() > bPosition.x();
    });

  // Draw entities
  for (int index = 0; index < count; index++) {
    const auto  entity = drawables[index];
    const auto& sprite = ecs.getComponent<RPG::SpriteComponent>(entity);
    
    // No sprite to draw
    if (sprite.sprite == nullptr)
      continue;

    const auto  position = ecs.getComponent<RPG::PositionComponent>(entity).position;

    // Draw sprite
    sprite.sprite->draw({ position.x(), position.y() - position.z() }, sprite.outline);
  }
}

const RPG::Texture& RPG::DrawingSystem::getTexture(const std::string& name)
{
  // Load and get texture
  return _textures.emplace(name, name).first->second;
}

void  RPG::MovingSystem::setMove(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, RPG::ECS::Entity entity, Math::Vector<3, float> position, float duration)
{
  auto& move = ecs.getComponent<RPG::MoveComponent>(entity);

  // Register move
  move.position = position;
  move.remaining = duration;

  auto& animationSystem = ecs.getSystem<RPG::AnimatingSystem>();

  // Start run animation
  if (animationSystem.entities.contains(entity) == true)
    animationSystem.setAnimation(ecs, world, level, entity, RPG::Animation::RunAnimation, true);
}

void  RPG::MovingSystem::execute(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, float elapsed)
{
  // Update every entity
  for (auto entity : entities) {
    auto& move = ecs.getComponent<RPG::MoveComponent>(entity);

    // No move
    if (move.remaining <= 0.f)
      continue;

    auto& position = ecs.getComponent<RPG::PositionComponent>(entity);
    float progress = std::min(elapsed / move.remaining, 1.f);

    // Update entity position
    move.remaining *= 1.f - progress;
    position.position = move.position + ((position.position - move.position) * (1.f - progress));

    // End of move
    if (move.remaining <= 0.f) {
      auto& animationSystem = ecs.getSystem<RPG::AnimatingSystem>();

      // Start ilde animation
      if (animationSystem.entities.contains(entity) == true)
        animationSystem.setAnimation(ecs, world, level, entity, RPG::Animation::IdleAnimation, true);
    }
  }
}

void  RPG::AnimatingSystem::setModel(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, RPG::ECS::Entity entity, const std::string& name)
{
  // Set model
  setModel(ecs, world, level, entity, getModel(name));
}

void  RPG::AnimatingSystem::setModel(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, RPG::ECS::Entity entity, const RPG::Model& model)
{
  auto& animation = ecs.getComponent<RPG::AnimationComponent>(entity);
  
  // Already used
  if (&model == animation.model)
    return;

  // Set new model
  animation.model = &model;

  // Reload animation
  setAnimation(ecs, world, level, entity, RPG::Animation::DefaultAnimation, true);
}

void  RPG::AnimatingSystem::setAnimation(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, RPG::ECS::Entity entity, const std::string& name, bool loop)
{
  auto& component = ecs.getComponent<RPG::AnimationComponent>(entity);

  // Set animation
  setAnimation(ecs, world, level, entity, component.model->animation(name), loop);
}

void  RPG::AnimatingSystem::setAnimation(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, RPG::ECS::Entity entity, const RPG::Animation& animation, bool loop)
{
  auto& component = ecs.getComponent<RPG::AnimationComponent>(entity);

  // Do not restart a looped animation
  if (&animation != component.animation || component.loop == false || loop == false) {
    component.animation = &animation;
    component.elapsed = 0.f;
    component.frame = 0;
    component.loop = loop;
  }
}

void  RPG::AnimatingSystem::setRandomAnimation(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, RPG::ECS::Entity entity, bool loop)
{
  auto& component = ecs.getComponent<RPG::AnimationComponent>(entity);

  // Set random animation
  setAnimation(ecs, world, level, entity, component.model->random(), loop);
}

void  RPG::AnimatingSystem::execute(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, float elapsed)
{
  // Update every entity
  for (auto entity : entities) {
    auto& animation = ecs.getComponent<RPG::AnimationComponent>(entity);
    auto& sprite = ecs.getComponent<RPG::SpriteComponent>(entity);

    // No model or animation
    if (animation.model == nullptr || animation.animation == nullptr)
      continue;

    // Increment timer
    animation.elapsed += elapsed;

    // Skip frames
    while (animation.elapsed > animation.animation->frame(animation.frame).duration)
    {
      // Non-ending frame
      if (animation.animation->frame(animation.frame).duration == 0.f)
        break;

      // Next frame
      animation.elapsed -= animation.animation->frame(animation.frame).duration;
      animation.frame = (animation.frame + 1) % animation.animation->count();

      // Animation over
      if (animation.loop == false && animation.frame == 0)
        setAnimation(ecs, world, level, entity, RPG::Animation::IdleAnimation, true);
    }
  }
}

const RPG::Model& RPG::AnimatingSystem::getModel(const std::string& name) const
{
  auto it = _models.find(name);

  // Model not found
  if (it == _models.end())
    return RPG::Model::ErrorModel;

  // Get model from map
  return it->second;
}

void RPG::ParticleSystem::execute(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, float elapsed)
{
  // Update each particle
  for (auto entity = entities.begin(); entity != entities.end();) {
    updateParticle(ecs, world, level, elapsed, *entity);

    const auto& particle = ecs.getComponent<RPG::ParticleComponent>(*entity);

    // Remove particle
    if (particle.elapsed >= particle.durationFadeIn + particle.durationLife + particle.durationFadeOut)
      ecs.destroyEntity(*(entity++));
    else
      entity++;
  }
}

void  RPG::ParticleSystem::updateParticle(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, float elapsed, RPG::ECS::Entity entity)
{
  auto& particle = ecs.getComponent<RPG::ParticleComponent>(entity);
  auto& sprite = ecs.getComponent<RPG::SpriteComponent>(entity);
  float duration = particle.durationFadeIn + particle.durationLife + particle.durationFadeOut;

  // Increase timer
  particle.elapsed += elapsed;

  // End particle
  if (particle.elapsed >= duration) {
    sprite.color.alpha = 0;
    return;
  }

  auto& position = ecs.getComponent<RPG::PositionComponent>(entity);

  // Update physics and position
  particle.physicsSpeed += particle.physicsGravity * elapsed;
  particle.physicsSpeed *= std::pow(1.f - particle.physicsDrag, elapsed);
  position.position += particle.physicsSpeed * elapsed;
  position.position.z() = std::max(particle.physicsFloor, position.position.z());

  // Update color
  sprite.color.red = (std::uint8_t)((float)particle.colorStart.red + ((float)particle.colorEnd.red - (float)particle.colorStart.red) * (particle.elapsed / duration));
  sprite.color.green = (std::uint8_t)((float)particle.colorStart.green + ((float)particle.colorEnd.green - (float)particle.colorStart.green) * (particle.elapsed / duration));
  sprite.color.blue = (std::uint8_t)((float)particle.colorStart.blue + ((float)particle.colorEnd.blue - (float)particle.colorStart.blue) * (particle.elapsed / duration));
  sprite.color.alpha = (std::uint8_t)((float)particle.colorStart.alpha + ((float)particle.colorEnd.alpha - (float)particle.colorStart.alpha) * (particle.elapsed / duration));
  
  // Fade in
  if (particle.elapsed < particle.durationFadeIn)
    sprite.color.alpha = (std::uint8_t)((float)sprite.color.alpha * (particle.elapsed / particle.durationFadeIn));

  // Fade out
  else if (particle.elapsed > particle.durationFadeIn + particle.durationFadeOut)
    sprite.color.alpha = (std::uint8_t)((float)sprite.color.alpha * (1.f - ((particle.elapsed - (particle.durationFadeIn + particle.durationLife)) / particle.durationFadeOut)));
}

void  RPG::ParticleEmitterSystem::execute(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, float elapsed)
{
  // Update each particle emitter
  for (auto entity = entities.begin(); entity != entities.end();) {
    updateParticleEmitter(ecs, world, level, elapsed, *entity);

    const auto& emitter = ecs.getComponent<RPG::ParticleEmitterComponent>(*entity);

    // Remove particle emitter
    if (emitter.duration <= 0.f)
      ecs.destroyEntity(*(entity++));
    else
      entity++;
  }
}

void  RPG::ParticleEmitterSystem::updateParticleEmitter(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, float elapsed, RPG::ECS::Entity entity)
{
  auto& emitter = ecs.getComponent<RPG::ParticleEmitterComponent>(entity);
  const auto& emitterPosition = ecs.getComponent<RPG::PositionComponent>(entity);
  auto& particleSystem = ecs.getSystem<RPG::ParticleSystem>();
  auto& animatingSystem = ecs.getSystem<RPG::AnimatingSystem>();

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
    emitter.next = 1.f / (std::min(emitter.frequencyLow, emitter.frequencyHigh) + (Math::Random() * (std::max(emitter.frequencyLow, emitter.frequencyHigh) - std::min(emitter.frequencyLow, emitter.frequencyHigh))));

    auto particle = ecs.createEntity();
    
    // Generate particle
    ecs.addComponent<RPG::PositionComponent>(particle);
    ecs.addComponent<RPG::SpriteComponent>(particle);
    ecs.addComponent<RPG::AnimationComponent>(particle);

    auto& particlePosition = ecs.getComponent<RPG::PositionComponent>(particle);
    auto& particleComponent = ecs.getComponent<RPG::ParticleComponent>(particle);
    
    // Randomize initial position
    particlePosition.position =
      emitterPosition.position
      + emitter.offset
      + RPG::Position(Math::Random() * emitter.size.x(), Math::Random() * emitter.size.y(), Math::Random() * emitter.size.z())
      - (emitter.size / 2.f);

    // Set particle model
    animatingSystem.setModel(ecs, world, level, particle, emitter.model);
    animatingSystem.setRandomAnimation(ecs, world, level, particle, true);

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
    particleComponent.physicsDrag = Math::Random(emitter.particleLow.physicsFloor, emitter.particleHigh.physicsFloor);
    particleComponent.colorStart = RPG::Color(
      (std::uint8_t)Math::Random(emitter.particleLow.colorStart.red, emitter.particleHigh.colorStart.red),
      (std::uint8_t)Math::Random(emitter.particleLow.colorStart.green, emitter.particleHigh.colorStart.green),
      (std::uint8_t)Math::Random(emitter.particleLow.colorStart.blue, emitter.particleHigh.colorStart.blue),
      (std::uint8_t)Math::Random(emitter.particleLow.colorStart.alpha, emitter.particleHigh.colorStart.alpha)
    );
    particleComponent.colorEnd = RPG::Color(
      (std::uint8_t)Math::Random(emitter.particleLow.colorEnd.red, emitter.particleHigh.colorEnd.red),
      (std::uint8_t)Math::Random(emitter.particleLow.colorEnd.green, emitter.particleHigh.colorEnd.green),
      (std::uint8_t)Math::Random(emitter.particleLow.colorEnd.blue, emitter.particleHigh.colorEnd.blue),
      (std::uint8_t)Math::Random(emitter.particleLow.colorEnd.alpha, emitter.particleHigh.colorEnd.alpha)
    );
    particleComponent.durationFadeIn = Math::Random(emitter.particleLow.durationFadeIn, emitter.particleHigh.durationFadeIn);
    particleComponent.durationLife = Math::Random(emitter.particleLow.durationLife, emitter.particleHigh.durationLife);
    particleComponent.durationFadeOut = Math::Random(emitter.particleLow.durationFadeOut, emitter.particleHigh.durationFadeOut);

    // Update particle timer
    particleSystem.updateParticle(ecs, world, level, elapsed, particle);
  }
}