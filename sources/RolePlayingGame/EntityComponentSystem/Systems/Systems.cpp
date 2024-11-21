#include "RolePlayingGame/EntityComponentSystem/Systems/Systems.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/Components.hpp"

const RPG::Icon& RPG::IconSystem::getIcon(const std::string& name) const
{
  auto iterator = _icons.find(name);

  // Handle errors
  if (iterator == _icons.end())
    return RPG::Icon::ErrorIcon;
  else
    return iterator->second;
}

void  RPG::ServerIconSystem::load(RPG::ECS& ecs, const Game::JSON::Array& models)
{
  // Load each icons
  for (const auto& model : models._vector)
    _icons.emplace(model->object().get("name").string(), model->object());
}

Game::JSON::Array RPG::ServerIconSystem::json(RPG::ECS& ecs) const
{
  Game::JSON::Array array;

  // Serialize each icon
  array._vector.reserve(_icons.size());
  for (const auto& [name, icon] : _icons) {
    auto json = icon.json();

    json.set("name", name);
    array.push(std::move(json));
  }

  return array;
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