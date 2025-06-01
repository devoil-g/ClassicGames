#include <exception>
#include <limits>
#include <string>

#include "RolePlayingGame/EntityComponentSystem/Systems/Systems.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/Components.hpp"

RPG::ActionSystem::ActionSystem(RPG::ECS& ecs) :
  RPG::ECS::System(ecs)
{}

void  RPG::ActionSystem::execute(float elapsed)
{
  // Execute actions in elapsed time
  while (elapsed > 0.f) {
    RPG::ECS::Entity  next = RPG::ECS::InvalidEntity;
    float             timer = elapsed;

    // Find next entity to execute
    for (auto entity : entities) {
      const auto& action = ecs.getComponent<RPG::ActionComponent>(entity);

      // No action
      if (action.action == nullptr)
        continue;

      // TODO: take entity speed into account for simultaneous actions

      // Earliest action
      if (action.wait < timer) {
        timer = action.wait;
        next = entity;
      }
    }

    // Reduce wait time of entities
    for (auto entity : entities) {
      auto& action = ecs.getComponent<RPG::ActionComponent>(entity);

      action.wait = std::max(0.f, action.wait - timer);
    }

    // Consume wait time
    elapsed -= timer;

    // Execute action
    if (next != RPG::ECS::InvalidEntity) {
      auto& action = ecs.getComponent<RPG::ActionComponent>(next);

      switch (action.mode) {
      case RPG::ActionComponent::Mode::Wait:
        action.action->atWait(ecs, next);
        break;
      case RPG::ActionComponent::Mode::Command:
        action.action->atCommand(ecs, next);
        break;
      case RPG::ActionComponent::Mode::Execute:
        action.action->atExecute(ecs, next);
        break;
      default:
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      }
    }
  }
}

void RPG::ActionSystem::wait(RPG::ECS::Entity entity, float wait, std::unique_ptr<RPG::ActionComponent::IAction>&& action)
{
}

void RPG::ActionSystem::command(RPG::ECS::Entity entity, float wait, std::unique_ptr<RPG::ActionComponent::IAction>&& action)
{
}

void RPG::ActionSystem::execute(RPG::ECS::Entity entity, float wait, std::unique_ptr<RPG::ActionComponent::IAction>&& action)
{
}

void RPG::ActionSystem::interrupt(RPG::ECS::Entity entity)
{
}

/*
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
*/