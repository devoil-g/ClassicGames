#include "RolePlayingGame/EntityComponentSystem/Systems/BoardSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/DisplaySystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/EntitySystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/CellComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/DisplayComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/EntityComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/NetworkComponent.hpp"

RPG::ECS::Entity  RPG::EntitySystem::getEntity(RPG::ECS& ecs, const std::string& id) const
{
  // Find entity matching ID
  for (auto entity : entities)
    if (ecs.getComponent<RPG::EntityComponent>(entity).id == id)
      return entity;

  // No match
  return RPG::ECS::InvalidEntity;
}

void  RPG::ServerEntitySystem::load(RPG::ECS& ecs, const Game::JSON::Array& entities)
{
  // Create game entities
  for (const auto& element : entities._vector) {
    auto entity = ecs.createEntity();

    ecs.addComponent<RPG::EntityComponent>(entity, element->object());
    ecs.addComponent<RPG::NetworkComponent>(entity);
  }
}

Game::JSON::Array RPG::ServerEntitySystem::json(RPG::ECS& ecs) const
{
  Game::JSON::Array array;

  // Serialize each model
  array._vector.reserve(entities.size());
  for (auto entity : entities)
    array.push(ecs.getComponent<RPG::EntityComponent>(entity).json());

  return array;
}

RPG::ECS::Entity  RPG::ClientEntitySystem::getEntityAtPixel(RPG::ECS& ecs, const Math::Vector<2>& pixel) const
{
  const auto&                                         displaySystem = ecs.getSystem<RPG::ClientDisplaySystem>();
  auto                                                position = displaySystem.getCamera().pixelToCoords(pixel);
  std::array<RPG::ECS::Entity, RPG::ECS::MaxEntities> interactive;
  std::size_t                                         count = 0;

  // Get every interactive entities in an array
  for (auto entity : entities) {
    // TODO: sort non interactive entity
    interactive[count] = entity;
    count += 1;
  }

  // Sort drawables by depth
  std::sort(interactive.begin(), interactive.begin() + count, [&ecs](auto aEntity, auto bEntity) {
    return RPG::ClientDisplaySystem::CloserEntity(ecs, aEntity, bEntity);
    });

  // Find first matching entity
  for (size_t index = 0; index < count; index++)
    if (displaySystem.intersect(ecs, interactive[index], position) == true)
      return interactive[index];

  // No match
  return RPG::ECS::InvalidEntity;
}

void  RPG::ClientEntitySystem::executePosition(RPG::ECS& ecs)
{
  // Update display position of each entity
  for (auto entity : entities)
    executePosition(ecs, entity);
}

void  RPG::ClientEntitySystem::executePosition(RPG::ECS& ecs, RPG::ECS::Entity entity)
{
  const auto& entityComponent = ecs.getComponent<RPG::EntityComponent>(entity);
  auto& displayComponent = ecs.getComponent<RPG::DisplayComponent>(entity);
  auto& cellSystem = ecs.getSystem<RPG::ClientBoardSystem>();
  auto cellEntity = cellSystem.getCell(entityComponent.coordinates);
  float cellHeight = 0.f;

  // Check if cell exist to take its height
  if (cellEntity != RPG::ECS::InvalidEntity)
    cellHeight = ecs.getComponent<RPG::CellComponent>(cellEntity).height;

  // Compute display position
  displayComponent.position.x() = ((+entityComponent.coordinates.x()) + (-entityComponent.coordinates.y()) + (+entityComponent.position.x())) * RPG::DisplaySystem::CellOffsetX;
  displayComponent.position.y() = ((-entityComponent.coordinates.x()) + (-entityComponent.coordinates.y()) + (+entityComponent.position.y())) * RPG::DisplaySystem::CellOffsetY;
  displayComponent.position.z() = entityComponent.position.z() + cellHeight;
  displayComponent.direction = entityComponent.direction;
}

void  RPG::ClientEntitySystem::handlePacket(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(1).string();

  // Load resources
  if (type == "load")
    handleLoad(ecs, client, json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientEntitySystem::handleLoad(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(2).string();

  // Load resources
  if (type == "entities")
    handleLoadEntities(ecs, client, json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientEntitySystem::handleLoadEntities(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  auto& displaySystem = ecs.getSystem<RPG::ClientDisplaySystem>();
  
  // Load entities
  for (const auto& element : json.get("entities").array()._vector) {
    auto entity = ecs.createEntity();

    ecs.addComponent<RPG::EntityComponent>(entity, element->object());
    ecs.addComponent<RPG::DisplayComponent>(entity);

    // Set model of entity
    displaySystem.setModel(ecs, entity, ecs.getComponent<RPG::EntityComponent>(entity).model);
  }
}