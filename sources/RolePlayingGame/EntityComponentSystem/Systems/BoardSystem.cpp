#include "RolePlayingGame/EntityComponentSystem/Systems/BoardSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ModelSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/CellComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ModelComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ParticleEmitterComponent.hpp"

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

void  RPG::ClientBoardSystem::executeCell(RPG::ECS& ecs, float elapsed)
{
  // Update each cell of the board
  for (auto entity : entities)
    executeCell(ecs, entity, elapsed);
}

void  RPG::ClientBoardSystem::executeCell(RPG::ECS& ecs, RPG::ECS::Entity entity, float elapsed)
{
  // TODO
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

    ecs.addComponent<RPG::CellComponent>(entity, element->object());
    registerCell(ecs, entity);

    // TODO: remove this
    ecs.addComponent<RPG::ParticleEmitterComponent>(entity);
    ecs.addComponent<RPG::ModelComponent>(entity);
    
    auto& cell = ecs.getComponent<RPG::CellComponent>(entity);
    auto& emitter = ecs.getComponent<RPG::ParticleEmitterComponent>(entity);
    auto& model = ecs.getComponent<RPG::ModelComponent>(entity);

    modelSystem.setModel(ecs, entity, "test_tile" + std::to_string((cell.coordinates.x() + cell.coordinates.y()) % 3 + 1));
    model.position = {
      ((+cell.coordinates.x()) + (-cell.coordinates.y())) * RPG::ClientModelSystem::CellOffsetX,
      ((-cell.coordinates.x()) + (-cell.coordinates.y())) * RPG::ClientModelSystem::CellOffsetY - RPG::ClientModelSystem::CellOffsetY,
      cell.height
    };

    emitter.duration = ((cell.coordinates.x() + cell.coordinates.y()) % 3) == 0 ? 1000.f : 0.f;
    emitter.size = { RPG::ClientModelSystem::CellOffsetX, RPG::ClientModelSystem::CellOffsetY * 2, 0.f };
    emitter.position = {
      ((+cell.coordinates.x()) + (-cell.coordinates.y())) * RPG::ClientModelSystem::CellOffsetX,
      ((-cell.coordinates.x()) + (-cell.coordinates.y())) * RPG::ClientModelSystem::CellOffsetY,
      cell.height
    };
    emitter.frequencyLow = 4.0f;
    emitter.frequencyHigh = 12.f;
    emitter.model = "error";
    emitter.particleLow.durationLife = emitter.particleHigh.durationLife = 1.f;
    emitter.particleLow.durationFadeIn = emitter.particleHigh.durationFadeIn = 0.2f;
    emitter.particleLow.durationFadeOut = emitter.particleHigh.durationFadeOut = 0.8f;
    emitter.particleLow.physicsGravity.z() = emitter.particleHigh.physicsGravity.z() = 4.f;
    emitter.particleLow.physicsSpeed.z() = emitter.particleHigh.physicsSpeed.z() = 2.f;
    emitter.particleLow.physicsDrag = emitter.particleHigh.physicsDrag = 0.1f;
    emitter.particleLow.physicsFloor = emitter.particleHigh.physicsFloor = cell.height;
    emitter.particleLow.colorStart.alpha = emitter.particleLow.colorEnd.alpha = 0.3f;
    emitter.particleHigh.colorStart.alpha = emitter.particleHigh.colorEnd.alpha = 0.5f;
  }
}