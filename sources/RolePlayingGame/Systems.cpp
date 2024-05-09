#include "RolePlayingGame/Systems.hpp"
#include "RolePlayingGame/Components.hpp"
#include "RolePlayingGame/Level.hpp"
#include "RolePlayingGame/World.hpp"

void  RPG::DrawingSystem::draw(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level)
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
    auto  aPosition = level.position(ecs.getComponent<RPG::CoordinatesComponent>(aEntity).coordinates, ecs.getComponent<RPG::PositionComponent>(aEntity).position);
    auto  bPosition = level.position(ecs.getComponent<RPG::CoordinatesComponent>(bEntity).coordinates, ecs.getComponent<RPG::PositionComponent>(bEntity).position);

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
    auto  entity = drawables[index];
    auto& drawable = ecs.getComponent<RPG::DrawableComponent>(entity);
    
    // No model / animation / texture
    if (drawable.model == nullptr || drawable.animation == nullptr || drawable.texture == nullptr)
      continue;

    auto  position = level.position(ecs.getComponent<RPG::CoordinatesComponent>(entity).coordinates, ecs.getComponent<RPG::PositionComponent>(entity).position);
    
    // Draw entity
    drawable.animation->frames[drawable.frame].draw(*drawable.texture, { position.x(), position.y() - position.z() }, drawable.outline);
  }
}