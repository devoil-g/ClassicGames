#include <stdexcept>
#include <limits>
#include <string>

#include "RolePlayingGame/EntityComponentSystem/Systems/Systems.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/BoardSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/EntitySystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ModelSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/NetworkSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/Components.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/CellComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/EntityComponent.hpp"

RPG::ServerActionSystem::ServerActionSystem(RPG::ECS& ecs) :
  RPG::ECS::System(ecs)
{}

void  RPG::ServerActionSystem::execute(float elapsed)
{
  // Execute actions in elapsed time
  while (elapsed > 0.f) {
    RPG::ECS::Entity  next = RPG::ECS::InvalidEntity;
    float             timer = elapsed;

    // Find next entity to execute
    for (auto entity : entities) {
      auto& action = ecs.getComponent<RPG::ServerActionComponent>(entity);

      // Get next action
      if (action.action == nullptr)
      {
        // No next action
        if (action.next == nullptr)
          continue;

        // Get next action
        action.action = std::move(action.next);
      }

      // TODO: take entity speed into account for simultaneous actions

      // Earliest action
      if (action.wait < timer) {
        timer = action.wait;
        next = entity;
      }
    }

    // Reduce wait time of entities
    for (auto entity : entities) {
      auto& action = ecs.getComponent<RPG::ServerActionComponent>(entity);

      action.wait = std::max(0.f, action.wait - timer);
    }

    // Consume wait time
    elapsed -= timer;

    // Execute action
    if (next != RPG::ECS::InvalidEntity) {
      auto& action = ecs.getComponent<RPG::ServerActionComponent>(next);
      auto oldMode = action.mode;

      try {
        switch (action.mode) {
        case RPG::ActionComponent::Mode::Wait:
          action.action->atWait();
          break;
        case RPG::ActionComponent::Mode::Command:
          action.action->atCommand();
          break;
        case RPG::ActionComponent::Mode::Execute:
          action.action->atExecute();
          break;
        default:
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
        }
      }
      catch (const std::exception& error) {
        std::cerr << "[RPG::ActionSystem] Warning: exception in action of entity #" << next << " (" << error.what() << ")." << std::endl;

        // Force reset of current action
        action.action.reset();
        action.mode = RPG::ActionComponent::Mode::Wait;
        action.wait = 0.f;
      }

      // Error, no change in action
      if (action.action != nullptr && action.mode == oldMode && action.wait <= 0.f)
      {
        std::cerr << "[RPG::ActionSystem] Warning: no change in action of entity #" << next << "." << std::endl;

        // Force reset of current action
        action.action.reset();
        action.mode = RPG::ActionComponent::Mode::Wait;
        action.wait = 0.f;

        // TODO: broadcast null action to clients
      }
    }
  }
}

void  RPG::ServerActionSystem::handlePacket(std::size_t id, const Game::JSON::Object& json)
{
  // Entity not controlled by this player
  if (id != ecs.getSystem<RPG::ServerNetworkSystem>().getController(ecs.getSystem<RPG::ServerEntitySystem>().getEntity(json.get(L"id").string()))) {
    std::wcerr << "[RPG::ActionSystem]: invalid action request if client #" << id << " for entity #" << ecs.getSystem<RPG::ServerEntitySystem>().getEntity(json.get(L"id").string()) << " (id: '" << json.get(L"id").string() << "')" << std::endl;
    return;
  }

  const auto& type = json.get(L"type").array().get(1).string();

  // Move action
  if (type == L"move")
    handleMove(id, json);
  
  // Invalid action
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ServerActionSystem::handleMove(std::size_t id, const Game::JSON::Object& json)
{
  auto  entity = ecs.getSystem<RPG::ServerEntitySystem>().getEntity(json.get(L"id").string());
  auto& action = ecs.getComponent<RPG::ServerActionComponent>(entity);

  // Add move action to entity
  action.next = std::make_unique<RPG::ServerMoveAction>(ecs, entity, json);

  // Interrupt previous action
  if (action.action != nullptr)
    action.action->interrupt();
}

RPG::ClientActionSystem::ClientActionSystem(RPG::ECS& ecs) :
  RPG::ECS::System(ecs),
  _index(0),
  _blocking()
{}

RPG::ClientActionSystem::~ClientActionSystem()
{
  // Unregister system's actions
  // NOTE: actions destructors could call other destroyed system
  for (auto entity : entities)
    ecs.getComponent<RPG::ClientActionComponent>(entity).action.reset();
}

void  RPG::ClientActionSystem::execute(float elapsed)
{
  std::array<float, RPG::ECS::MaxEntities>  remaining;

  // Save remaining time for each entity
  for (auto entity : entities)
    remaining[entity] = elapsed;

  bool blocked = true;

  // Repeat as long as there is someting to update
  while (blocked == true)
  {
    // Reset flag
    blocked = false;

    // Update each entity
    for (auto entity : entities) {
      auto& action = ecs.getComponent<RPG::ClientActionComponent>(entity);
      auto blocking = _blocking.empty() == true ? std::numeric_limits<std::size_t>().max() : _blocking.front();

      // Force load of an action
      if (action.action == nullptr && action.next.empty() == false) {
        action.action = action.next.front()();
        action.next.pop();
      }

      // Execute actions of entity
      while (remaining[entity] > 0.f && action.action != nullptr)
      {
        // Stop if blocked
        if (action.action->index > blocking) {
          blocked = true;
          break;
        }

        // Update action
        remaining[entity] = action.action->update(remaining[entity]);

        // Action done
        if (remaining[entity] > 0.f)
        {
          // Check if action is blocking
          if (action.action->index == blocking)
            _blocking.pop();

          // Remove action
          action.action.reset();

          // Get next action
          if (action.next.empty() == false) {
            action.action = action.next.front()();
            action.next.pop();
          }
        }
      }

    }
  }
}

void  RPG::ClientActionSystem::handlePacket(const Game::JSON::Object& json)
{
  const auto& type = json.get(L"type").array().get(1).string();

  // Move action
  if (type == L"move")
    handleMove(json);

  // Invalid action
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientActionSystem::handleMove(const Game::JSON::Object& json)
{
  // Add move action to entity
  action<RPG::ClientMoveAction>(json,
    RPG::Coordinates(json.get(L"target").array()),
    RPG::Coordinates(json.get(L"coordinates").array()),
    RPG::Position(json.get(L"position").array()),
    RPG::StringToDirection(json.get(L"direction").string()),
    (float)json.get(L"duration").number()
  );
}

RPG::ServerMoveAction::ServerMoveAction(RPG::ECS& ecs, RPG::ECS::Entity self, const Game::JSON::Object& json) :
  RPG::ServerActionComponent::Action(ecs, self),
  _target(json.get(L"target").array())
{}

void  RPG::ServerMoveAction::atWait()
{
  auto& action = ecs.getComponent<RPG::ServerActionComponent>(self);

  // Switch of execute mode
  action.mode = RPG::ActionComponent::Mode::Execute;
  action.wait = 0.f;

  // Immediatly execute first move
  atExecute();
}

void  RPG::ServerMoveAction::atCommand()
{
  // Never use command mode on a move action
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ServerMoveAction::atExecute()
{
  auto& action = ecs.getComponent<RPG::ServerActionComponent>(self);
  auto& entity = ecs.getComponent<RPG::EntityComponent>(self);
  
  // Target reached
  if (_target == entity.coordinates)
  {
    action.mode = RPG::ActionComponent::Mode::Wait;
    action.wait = 0.f;
  }

  else {
    auto            direction = _target - entity.coordinates;
    RPG::Direction  targetDirection;

    // Find direction
    if (direction.x() > 0 && direction.y() > 0)
      targetDirection = RPG::Direction::DirectionNorth;
    else if (direction.x() < 0 && direction.y() < 0)
      targetDirection = RPG::Direction::DirectionSouth;
    else if (direction.x() > 0)
      targetDirection = RPG::Direction::DirectionNorthEast;
    else if (direction.y() > 0)
      targetDirection = RPG::Direction::DirectionNorthWest;
    else if (direction.x() < 0)
      targetDirection = RPG::Direction::DirectionSouthWest;
    else if (direction.y() < 0)
      targetDirection = RPG::Direction::DirectionSouthEast;
    else
      targetDirection = entity.direction;

    // Move entity
    entity.coordinates += RPG::DirectionCoordinates[targetDirection];
    entity.direction = targetDirection;
    action.mode = RPG::ActionComponent::Mode::Execute;
    action.wait = 0.625f;
  }

  Game::JSON::Object  json;
  
  // Send move to players
  json.set(L"id", entity.id);
  json.set(L"target", _target.json());
  json.set(L"coordinates", entity.coordinates.json());
  json.set(L"position", entity.position.json());
  json.set(L"direction", RPG::DirectionToString(entity.direction));
  json.set(L"mode", RPG::ActionComponent::ModeToString(action.mode));
  json.set(L"duration", (double)action.wait);
  ecs.getSystem<RPG::ServerNetworkSystem>().broadcast({ L"action", L"move" }, json);

  // End action
  if (_target == entity.coordinates) {
    action.action.reset();
    return;
  }
}

void  RPG::ServerMoveAction::interrupt()
{
  auto& entity = ecs.getComponent<RPG::EntityComponent>(self);

  // Move interrupted, stop at current position
  _target = entity.coordinates;
}

RPG::ClientMoveAction::ClientMoveAction(RPG::ECS& ecs, RPG::ECS::Entity self, std::size_t index, RPG::Coordinates target, RPG::Coordinates coordinates, RPG::Position position, RPG::Direction direction, float duration) :
  RPG::ClientActionComponent::Action(ecs, self, index),
  _target(target),
  _coordinates(coordinates),
  _position(position),
  _direction(direction),
  _remaining(duration)
{
  auto& entity = ecs.getComponent<RPG::EntityComponent>(self);

  // Get height of cells
  const auto& boardSystem = ecs.getSystem<RPG::ClientBoardSystem>();
  auto cellOrigin = boardSystem.getCell(entity.coordinates);
  auto heightOrigin = cellOrigin == RPG::ECS::InvalidEntity ? 0.f : ecs.getComponent<RPG::CellComponent>(cellOrigin).height;
  auto cellDestination = boardSystem.getCell(_coordinates);
  auto heightDestination = cellDestination == RPG::ECS::InvalidEntity ? 0.f : ecs.getComponent<RPG::CellComponent>(cellDestination).height;

  // Register new position
  entity.position = {
    entity.position.x() + entity.coordinates.x() - _coordinates.x(),
    entity.position.y() + entity.coordinates.y() - _coordinates.y(),
    entity.position.z() + heightOrigin - heightDestination
  };
  entity.coordinates = _coordinates;
  entity.direction = _direction;

  // Start move animation
  ecs.getSystem<RPG::ClientModelSystem>().setAnimation(self, RPG::Model::Actor::RunAnimation, RPG::Model::Actor::Mode::Loop, +1.f);
}

RPG::ClientMoveAction::~ClientMoveAction()
{
  auto& entity = ecs.getComponent<RPG::EntityComponent>(self);

  // Force entity to destination
  entity.position = _position;

  // Stop run animation if at target destination
  if (entity.coordinates == _target)
    ecs.getSystem<RPG::ClientModelSystem>().setAnimation(self, RPG::Model::Actor::IdleAnimation, RPG::Model::Actor::Mode::Loop, +1.f);
}

float RPG::ClientMoveAction::update(float elapsed)
{
  // End of move
  if (elapsed > _remaining)
    return elapsed - _remaining;

  auto& entity = ecs.getComponent<RPG::EntityComponent>(self);

  // Move entity to new position
  entity.position += (_position - entity.position) * (elapsed / _remaining);

  // Compute remaining time
  _remaining -= elapsed;

  return 0.f;
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