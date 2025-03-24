#include "RolePlayingGame/EntityComponentSystem/Systems/BoardSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ModelSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/BoardComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/CellComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ModelComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ParticleEmitterComponent.hpp"

RPG::BoardSystem::BoardSystem(RPG::ECS& ecs)
{}

RPG::ECS::Entity  RPG::BoardSystem::getCell(RPG::Coordinates coordinates) const
{
  auto it = _map.find(coordinates);

  // No cell found
  if (it == _map.end())
    return RPG::ECS::InvalidEntity;

  // Return entity ID of cell
  return it->second;
}

void  RPG::BoardSystem::registerCell(RPG::ECS& ecs, RPG::ECS::Entity entity)
{
  auto& cell = ecs.getComponent<RPG::CellComponent>(entity);

  assert(_map.contains(cell.coordinates) == false && "Cell already registered.");

  // Register cell in map
  _map.emplace(cell.coordinates, entity);
}

void  RPG::BoardSystem::unregisterCell(RPG::ECS& ecs, RPG::ECS::Entity entity)
{
  auto& cell = ecs.getComponent<RPG::CellComponent>(entity);

  assert(_map.contains(cell.coordinates) == true && "Cell not registered.");

  // Unregister cell
  _map.erase(cell.coordinates);
}

RPG::ServerBoardSystem::ServerBoardSystem(RPG::ECS& ecs) :
  RPG::BoardSystem(ecs)
{}

void  RPG::ServerBoardSystem::load(RPG::ECS& ecs, const Game::JSON::Array& cells)
{
  // Create each cells of the board
  for (const auto& element : cells._vector) {
    auto entity = ecs.createEntity();

    ecs.addComponent<RPG::CellComponent>(entity, element->object());
    registerCell(ecs, entity);
  }
}

Game::JSON::Array RPG::ServerBoardSystem::json(RPG::ECS& ecs) const
{
  Game::JSON::Array   array;

  // Serialize game board
  array._vector.reserve(entities.size());
  for (auto entity : entities)
    array.push(ecs.getComponent<RPG::CellComponent>(entity).json());

  return array;
}

RPG::ClientBoardSystem::ClientBoardSystem(RPG::ECS& ecs) :
  RPG::BoardSystem(ecs),
  _cursorModel(ecs.createEntity()),
  _cursorCell(RPG::ECS::InvalidEntity)
{
  // Set up cursor model
  ecs.addComponent<RPG::ModelComponent>(_cursorModel);
  ecs.getSystem<RPG::ClientModelSystem>().setModel(ecs, _cursorModel, "cursor");
  ecs.getComponent<RPG::ModelComponent>(_cursorModel).layer = RPG::ModelComponent::Layer::LayerBoard;

  // First update of cursor
  executeCursor(ecs, 0.f);
}

RPG::ECS::Entity  RPG::ClientBoardSystem::getCursor() const
{
  // Get current cursor cell
  return _cursorCell;
}

void  RPG::ClientBoardSystem::setCursor(RPG::ECS& ecs, RPG::ECS::Entity entity)
{
  assert((entity == RPG::ECS::InvalidEntity || entities.contains(entity) == true) && "Entity is not a cell");

  // Cancel previous particle emitter
  if (_cursorCell != RPG::ECS::InvalidEntity)
    ecs.getComponent<RPG::ParticleEmitterComponent>(_cursorCell).duration = 0.f;

  // Set new cell of cursor
  _cursorCell = entity;

  // Trigger new particle emitter
  if (_cursorCell != RPG::ECS::InvalidEntity)
    ecs.getComponent<RPG::ParticleEmitterComponent>(_cursorCell).duration = 999.f;
}

RPG::ECS::Entity  RPG::ClientBoardSystem::intersect(RPG::ECS& ecs, const Math::Vector<2>& coords) const
{
  std::array<RPG::ECS::Entity, RPG::ECS::MaxEntities> cells;
  std::size_t                                         count = 0;

  int column = (long)std::round(coords.x() / RPG::CellOffset.x());

  // Get cells in column
  for (auto entity : entities) {
    const auto& cell = ecs.getComponent<RPG::CellComponent>(entity);

    // Select only cells in column
    if (cell.coordinates.x() - cell.coordinates.y() == column) {
      cells[count] = entity;
      count += 1;
    }
  }

  // Sort cells by depth
  std::sort(cells.begin(), cells.begin() + count, [&ecs](auto aEntity, auto bEntity) {
    const auto& aCell = ecs.getComponent<RPG::CellComponent>(aEntity);
    const auto& bCell = ecs.getComponent<RPG::CellComponent>(bEntity);

    return (aCell.coordinates.x() + aCell.coordinates.y()) < (bCell.coordinates.x() + bCell.coordinates.y());
    });

  // Get first cell matching Y coords
  for (int index = 0; index < count; index++) {
    const auto& cell = ecs.getComponent<RPG::CellComponent>(cells[index]);
    float height = (cell.coordinates.x() + cell.coordinates.y()) * -RPG::CellOffset.y() + cell.height * -RPG::CellOffset.z();

    // Check matching height
    if (coords.y() > height - RPG::CellSize.y() / 2 && coords.y() < height + RPG::CellSize.y() / 2)
      return cells[index];
  }

  // No cell found
  return RPG::ECS::InvalidEntity;
}

void  RPG::ClientBoardSystem::executeCursor(RPG::ECS& ecs, float elapsed)
{
  auto& model = ecs.getComponent<RPG::ModelComponent>(_cursorModel);

  // No cell selected, transparent cursor
  if (_cursorCell == RPG::ECS::InvalidEntity) {
    model.color.alpha = 0.f;
  }
  
  // Cell selected, set visibility and position of cursor
  else {
    const auto& cell = ecs.getComponent<RPG::CellComponent>(_cursorCell);

    model.color.alpha = 1.f;
    model.position.x() = (float)cell.coordinates.x();
    model.position.y() = (float)cell.coordinates.y();
    model.position.z() = cell.height + 0.001f;
  }
}

void  RPG::ClientBoardSystem::executeCell(RPG::ECS& ecs, float elapsed)
{
  // Update each cell of the board
  for (auto entity : entities)
    executeCell(ecs, entity, elapsed);
}

void  RPG::ClientBoardSystem::executeCell(RPG::ECS& ecs, RPG::ECS::Entity entity, float elapsed)
{
  auto& cell = ecs.getComponent<RPG::CellComponent>(entity);
  auto& model = ecs.getComponent<RPG::ModelComponent>(entity);
  auto& particle = ecs.getComponent<RPG::ParticleEmitterComponent>(entity);

  model.position.x() = (float)cell.coordinates.x();
  model.position.y() = (float)cell.coordinates.y();
  model.position.z() = cell.height;
  particle.position = model.position;
}

void  RPG::ClientBoardSystem::handlePacket(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(1).string();

  // Load/reload board
  if (type == "load")
    handleLoad(ecs, client, json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientBoardSystem::handleLoad(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(2).string();

  // Load/reload cells
  if (type == "cells")
    handleLoadCells(ecs, client, json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientBoardSystem::handleLoadCells(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  auto& modelSystem = ecs.getSystem<RPG::ClientModelSystem>();

  // Create each cells of the board
  for (const auto& element : json.get("cells").array()._vector) {
    auto entity = ecs.createEntity();

    // Register component
    ecs.addComponent<RPG::BoardComponent>(entity);
    ecs.addComponent<RPG::CellComponent>(entity, element->object());
    ecs.addComponent<RPG::ModelComponent>(entity);
    ecs.addComponent<RPG::ParticleEmitterComponent>(entity);

    // Register cell in coordinates map
    registerCell(ecs, entity);

    auto& modelComponent = ecs.getComponent<RPG::ModelComponent>(entity);
    auto& modelSystem = ecs.getSystem<RPG::ClientModelSystem>();
    auto& particleComponent = ecs.getComponent<RPG::ParticleEmitterComponent>(entity);
    
    // Initialize cell model
    modelComponent.layer = RPG::ModelComponent::Layer::LayerBoard;
    modelComponent.color.alpha = 0.25f;
    modelSystem.setModel(ecs, entity, "cell");
    modelSystem.setAnimation(ecs, entity, "select");

    particleComponent.animation = "simple";
    particleComponent.frequencyLow = 16.f;
    particleComponent.frequencyHigh = 24.f;
    particleComponent.size = { 0.75f, 0.75f, 0.f };
    particleComponent.duration = 0.f;
    particleComponent.particleLow.colorStart = RPG::Color(1.f, 1.f, 1.f, 0.4f);
    particleComponent.particleLow.colorEnd = RPG::Color(1.f, 1.f, 1.f, 0.4f);
    particleComponent.particleHigh.colorStart = RPG::Color(1.f, 1.f, 1.f, 0.8f);
    particleComponent.particleHigh.colorEnd = RPG::Color(1.f, 1.f, 1.f, 0.8f);
    particleComponent.particleLow.durationFadeIn = 0.2f;
    particleComponent.particleHigh.durationFadeIn = 0.3f;
    particleComponent.particleLow.durationLife = 0.4f;
    particleComponent.particleHigh.durationLife = 0.6f;
    particleComponent.particleLow.durationFadeOut = 0.2f;
    particleComponent.particleHigh.durationFadeOut = 0.3f;
    particleComponent.particleLow.physicsDrag = 0.f;
    particleComponent.particleHigh.physicsDrag = 0.f;
    particleComponent.particleLow.physicsFloor = std::numeric_limits<float>::min();
    particleComponent.particleHigh.physicsFloor = std::numeric_limits<float>::min();
    particleComponent.particleLow.physicsGravity = { 0.f, 0.f, +2.f };
    particleComponent.particleHigh.physicsGravity = { 0.f, 0.f, +2.5f };
    particleComponent.particleLow.physicsSpeed = { 0.f, 0.f, +0.2f };
    particleComponent.particleHigh.physicsSpeed = { 0.f, 0.f, +0.3f };

    executeCell(ecs, entity, 0.f);
  }
}