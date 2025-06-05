#include "RolePlayingGame/EntityComponentSystem/Systems/BoardSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ModelSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/EntitySystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/CellComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ModelComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/EntityComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/NetworkComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/Components.hpp"

RPG::EntitySystem::EntitySystem(RPG::ECS& ecs) :
  RPG::ECS::System(ecs)
{}

RPG::ECS::Entity  RPG::EntitySystem::getEntity(const std::wstring& id) const
{
  // Find entity matching ID
  for (auto entity : entities)
    if (ecs.getComponent<RPG::EntityComponent>(entity).id == id)
      return entity;

  // No match
  return RPG::ECS::InvalidEntity;
}

RPG::ServerEntitySystem::ServerEntitySystem(RPG::ECS& ecs) :
  RPG::EntitySystem(ecs)
{}

void  RPG::ServerEntitySystem::load(const Game::JSON::Array& entities)
{
  // Create game entities
  for (const auto& element : entities) {
    auto entity = ecs.createEntity();

    ecs.addComponent<RPG::EntityComponent>(entity, element->object());

    // TODO: do not make every entity controllable
    ecs.addComponent<RPG::NetworkComponent>(entity);
    ecs.addComponent<RPG::ActionComponent>(entity);
  }
}

Game::JSON::Array RPG::ServerEntitySystem::json() const
{
  Game::JSON::Array array;

  // Serialize each model
  array.reserve(entities.size());
  for (auto entity : entities)
    array.push(ecs.getComponent<RPG::EntityComponent>(entity).json());

  return array;
}

RPG::ClientEntitySystem::ClientEntitySystem(RPG::ECS& ecs) :
  RPG::EntitySystem(ecs)
{}

RPG::ECS::Entity  RPG::ClientEntitySystem::intersect(const Math::Vector<2>& coords) const
{
  const auto&                                         modelSystem = ecs.getSystem<RPG::ClientModelSystem>();
  std::array<RPG::ECS::Entity, RPG::ECS::MaxEntities> interactive;
  std::size_t                                         count = 0;

  // Get every interactive entities in an array
  for (auto entity : entities) {
    // TODO: sort non interactive entity
    interactive[count] = entity;
    count += 1;
  }

  // Sort entities by depth
  std::sort(interactive.begin(), interactive.begin() + count, [this](auto aEntity, auto bEntity) {
    const auto& aModel = ecs.getComponent<RPG::ModelComponent>(aEntity);
    const auto& bModel = ecs.getComponent<RPG::ModelComponent>(bEntity);

    if (aModel.position.x() + aModel.position.y() < bModel.position.x() + bModel.position.y())
      return true;
    if (aModel.position.x() + aModel.position.y() > bModel.position.x() + bModel.position.y())
      return false;
    if (aModel.position.z() > bModel.position.z())
      return true;
    if (aModel.position.z() < bModel.position.z())
      return false;
    return aModel.position.x() - aModel.position.y() < bModel.position.x() - bModel.position.y();
    });

  // Find first matching entity
  for (size_t index = 0; index < count; index++)
    if (modelSystem.intersect(interactive[index], coords) == true)
      return interactive[index];

  // No match
  return RPG::ECS::InvalidEntity;
}

bool  RPG::ClientEntitySystem::intersect(RPG::ECS::Entity entity, const Math::Vector<2>& coords) const
{
  // Check collision with model
  return ecs.getSystem<RPG::ClientModelSystem>().intersect(entity, coords);
}

void  RPG::ClientEntitySystem::executePosition()
{
  // Update display position of each entity
  for (auto entity : entities)
    executePosition(entity);
}

void  RPG::ClientEntitySystem::executePosition(RPG::ECS::Entity entity)
{
  const auto& entityComponent = ecs.getComponent<RPG::EntityComponent>(entity);
  auto& displayComponent = ecs.getComponent<RPG::ModelComponent>(entity);
  auto& cellSystem = ecs.getSystem<RPG::ClientBoardSystem>();
  auto cellEntity = cellSystem.getCell(entityComponent.coordinates);
  float cellHeight = 0.f;

  // Check if cell exist to take its height
  if (cellEntity != RPG::ECS::InvalidEntity)
    cellHeight = ecs.getComponent<RPG::CellComponent>(cellEntity).height;

  // Compute display position
  displayComponent.position.x() = entityComponent.coordinates.x() + entityComponent.position.x();
  displayComponent.position.y() = entityComponent.coordinates.y() + entityComponent.position.y();
  displayComponent.position.z() = entityComponent.position.z() + cellHeight;
  displayComponent.direction = entityComponent.direction;
}

void  RPG::ClientEntitySystem::handlePacket(const Game::JSON::Object& json)
{
  const auto& type = json.get(L"type").array().get(1).string();

  // Load resources
  if (type == L"load")
    handleLoad(json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientEntitySystem::handleLoad(const Game::JSON::Object& json)
{
  const auto& type = json.get(L"type").array().get(2).string();

  // Load resources
  if (type == L"entities")
    handleLoadEntities(json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientEntitySystem::handleLoadEntities(const Game::JSON::Object& json)
{
  auto& displaySystem = ecs.getSystem<RPG::ClientModelSystem>();
  
  // Load entities
  for (const auto& element : json.get(L"entities").array()) {
    auto entity = ecs.createEntity();

    ecs.addComponent<RPG::EntityComponent>(entity, element->object());
    ecs.addComponent<RPG::ModelComponent>(entity);

    // Set layer of entity
    ecs.getComponent<RPG::ModelComponent>(entity).layer = RPG::ModelComponent::Layer::LayerEntity;

    // Set model of entity
    displaySystem.setModel(entity, ecs.getComponent<RPG::EntityComponent>(entity).model);
    displaySystem.setAnimation(entity, RPG::Model::Actor::DefaultAnimation);
  }
}