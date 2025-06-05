#include <stdexcept>
#include <limits>
#include <string>

#include "RolePlayingGame/EntityComponentSystem/Systems/Systems.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/EntitySystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/NetworkSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/Components.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/EntityComponent.hpp"

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
      auto& action = ecs.getComponent<RPG::ActionComponent>(entity);

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
      auto& action = ecs.getComponent<RPG::ActionComponent>(entity);

      action.wait = std::max(0.f, action.wait - timer);
    }

    // Consume wait time
    elapsed -= timer;

    // Execute action
    if (next != RPG::ECS::InvalidEntity) {
      auto& action = ecs.getComponent<RPG::ActionComponent>(next);
      auto oldMode = action.mode;

      try {
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

void  RPG::ActionSystem::handlePacket(std::size_t id, const Game::JSON::Object& json)
{
  // Entity not controlled by this player
  if (id != ecs.getSystem<RPG::ServerNetworkSystem>().getController(ecs.getSystem<RPG::ServerEntitySystem>().getEntity(json.get(L"id").string())))
    return;

  const auto& type = json.get(L"type").array().get(1).string();

  // Move action
  if (type == L"move")
    handleMove(id, json);
  
  // Invalid action
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ActionSystem::handleMove(std::size_t id, const Game::JSON::Object& json)
{
  auto entity = ecs.getSystem<RPG::ServerEntitySystem>().getEntity(json.get(L"id").string());

  std::cout << "Start move!" << std::endl;

  // Add move action to entity
  ecs.getComponent<RPG::ActionComponent>(entity).next = std::make_unique<RPG::MoveAction>(ecs, entity, json);
}

RPG::MoveAction::MoveAction(RPG::ECS& ecs, RPG::ECS::Entity self, const Game::JSON::Object& json) :
  _coordinates(json.get(L"coordinates").array())
{}

RPG::MoveAction::~MoveAction()
{}

void  RPG::MoveAction::atWait(RPG::ECS& ecs, RPG::ECS::Entity self)
{
  auto& action = ecs.getComponent<RPG::ActionComponent>(self);

  // Switch of execute mode
  action.mode = RPG::ActionComponent::Mode::Execute;
  action.wait = 0.f;

  // Immediatly execute first move
  atExecute(ecs, self);
}

void  RPG::MoveAction::atCommand(RPG::ECS& ecs, RPG::ECS::Entity self)
{
  // Never use command mode on a move action
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::MoveAction::atExecute(RPG::ECS& ecs, RPG::ECS::Entity self)
{
  auto& action = ecs.getComponent<RPG::ActionComponent>(self);
  auto& entity = ecs.getComponent<RPG::EntityComponent>(self);
  auto direction = _coordinates - entity.coordinates;

  RPG::Direction    targetDirection;

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

  // Coordinates reached
  else
  {
    // End action
    action.action.reset();
    action.mode = RPG::ActionComponent::Mode::Wait;
    action.wait = 0.f;

    Game::JSON::Object  json;

    // Send wait to players
    json.set(L"id", entity.id);
    json.set(L"mode", "wait");
    json.set(L"duration", (double)action.wait);
    ecs.getSystem<RPG::ServerNetworkSystem>().broadcast({ L"action", L"none" }, json);
    return;
  }

  // Move entity
  entity.coordinates += RPG::DirectionCoordinates[targetDirection];
  action.wait = 1.f;

  Game::JSON::Object  json;
  
  // Send move to players
  json.set(L"id", entity.id);
  json.set(L"coordinates", entity.coordinates.json());
  json.set(L"target", _coordinates.json());
  json.set(L"mode", "execute");
  json.set(L"duration", (double)action.wait);
  ecs.getSystem<RPG::ServerNetworkSystem>().broadcast({ L"action", L"move" }, json);
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