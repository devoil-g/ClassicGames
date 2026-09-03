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
  RPG::ECS::System(ecs),
  _clock(0.f)
{}

void  RPG::ServerActionSystem::execute(float elapsed)
{
  auto& network = ecs.getSystem<RPG::ServerNetworkSystem>();

  // Execute actions in elapsed time
  while (elapsed > 0.f) {
    RPG::ECS::Entity  next = RPG::ECS::InvalidEntity;
    float             timer = elapsed;

    // Find next entity to execute
    for (auto entity : entities()) {
      auto& action = ecs.getComponent<RPG::ServerActionComponent>(entity);

      // Get next action
      if (action.action == nullptr)
      {
        // No next action
        if (action.next.has_value() == false)
          continue;

        // Get next action
        action.action = action.next.value()();
        action.next.reset();
      }

      // Compute remaining time of action
      // TODO: add sub-actions (buff timers for example) here
      auto remaining = (action.speed > 0.f) ?
        ((1.f - action.progress) / action.speed) :
        ((action.progress == 1.f) ?
          (0.f) :
          (std::numeric_limits<float>::infinity()));

      // Earliest action
      if (timer > remaining) {
        timer = remaining;
        next = entity;
      }
    }

    // Progress actions of entities
    for (auto entity : entities()) {
      auto& action = ecs.getComponent<RPG::ServerActionComponent>(entity);

      // TODO: add sub-actions (buff timers for example) here
      action.progress = std::clamp(action.progress + action.speed * timer, 0.f, 1.f);
    }

    // Consume wait time
    elapsed -= timer;
    _clock += timer;

    // Execute action
    // TODO: add sub-actions (buff timers for example) here
    if (next != RPG::ECS::InvalidEntity) {
      auto& action = ecs.getComponent<RPG::ServerActionComponent>(next);

      try {
        action.action->execute();

        // Action finished, wait for next
        if (action.action == nullptr) {
          // TODO: reset to combat action wait time
          action.mode = RPG::ActionMode::Command;
          action.start = 0.f;
          action.end = 1.f;
          action.speed = 1.f;
          action.progress = 0.f;
        }
      }
      catch (const std::exception& error) {
        std::cerr << "[RPG::ServerActionSystem] Warning: exception in action of entity #" << next << " (" << error.what() << ")." << std::endl;
        
        // TODO: reset to combat action wait time
        // Force reset of current action
        action.action.reset();
        action.mode = RPG::ActionMode::Command;
        action.start = 0.f;
        action.end = 1.f;
        action.speed = 1.f;
        action.progress = 0.f;
      }

      auto actionJson = jsonAction(next);

      // Broadcast action JSON to clients
      network.broadcast({ L"action", L"entity" }, actionJson);
    }
  }

  Game::JSON::Object clockJson = jsonClock();

  // Broadcast new clock to clients
  network.broadcast({ L"action", L"clock" }, clockJson);
}

void  RPG::ServerActionSystem::handlePacket(std::size_t id, const Game::JSON::Object& json)
{
  // Entity not controlled by this player
  if (id != ecs.getSystem<RPG::ServerNetworkSystem>().getController(ecs.getSystem<RPG::ServerEntitySystem>().getEntity(json.get(L"id").string()))) {
    std::wcerr << "[RPG::ServerActionSystem] Warning: invalid action request by client #" << id << " for entity #" << ecs.getSystem<RPG::ServerEntitySystem>().getEntity(json.get(L"id").string()) << " (id: '" << json.get(L"id").string() << "')" << std::endl;
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
  
  // Extract data from JSON
  RPG::Coordinates  target = json.get(L"target").array();

  // Add action to entity
  action<RPG::ServerMoveAction>(entity, target);
}

Game::JSON::Array   RPG::ServerActionSystem::jsonActions() const
{
  Game::JSON::Array array;

  // Memory pre-allocation
  array.reserve(entities().size());

  // Serialize each entity
  for (auto entity : entities())
    array.push(jsonAction(entity));

  return array;
}

Game::JSON::Object  RPG::ServerActionSystem::jsonAction(RPG::ECS::Entity entity) const
{
  // Invalid entity
  if (entity == RPG::ECS::InvalidEntity)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  auto& actionComponent = ecs.getComponent<RPG::ServerActionComponent>(entity);
  auto& entityComponent = ecs.getComponent<RPG::EntityComponent>(entity);
  Game::JSON::Object json;

  // Serialize base action data
  json.set(L"id", entityComponent.id);
  json.set(L"clock", (double)_clock);
  json.set(L"mode", RPG::ActionModeToString(actionComponent.mode));
  json.set(L"start", (double)actionComponent.start);
  json.set(L"end", (double)actionComponent.end);
  json.set(L"progress", (double)actionComponent.progress);
  json.set(L"speed", (double)actionComponent.speed);

  // Serialize specific action data
  if (actionComponent.action != nullptr)
    json.set(L"action", actionComponent.action->json());
  else
    json.set(L"action");

  return json;
}

Game::JSON::Object  RPG::ServerActionSystem::jsonClock() const
{
  Game::JSON::Object json;

  // Serialize clock
  json.set(L"clock", (double)_clock);

  return json;
}

RPG::ClientActionSystem::ClientActionSystem(RPG::ECS& ecs) :
  RPG::ECS::System(ecs),
  _clock(0.f),
  _target(0.f),
  _timeout(RPG::ClientActionSystem::DefaultTimeout),
  _bufferingMin(RPG::ClientActionSystem::DefaultBufferingMin),
  _bufferingMax(RPG::ClientActionSystem::DefaultBufferingMax)
{}

RPG::ClientActionSystem::~ClientActionSystem()
{
  // Unregister system's actions
  // NOTE: actions destructors could call other destroyed system
  for (auto entity : entities())
    ecs.getComponent<RPG::ClientActionComponent>(entity).action.reset();
}

void  RPG::ClientActionSystem::execute(float elapsed)
{
  float startClock = _clock;

  // Acceleration
  if (_clock < _target - _bufferingMax * _timeout && elapsed > 0.f) {
    float speed = ((_target - _bufferingMax * _timeout) - _clock) / (2.f * _bufferingMin * _timeout) + 1.f;
    float consumed = std::min((_target - _bufferingMax * _timeout) - _clock, elapsed * speed);

    _clock += consumed;
    elapsed -= consumed / speed;
  }

  // Normal speed
  if (_clock < _target - _bufferingMin * _timeout && elapsed > 0.f) {
    float consumed = std::min((_target - _bufferingMin * _timeout) - _clock, elapsed);

    _clock += consumed;
    elapsed -= consumed;
  }

  // Slowing
  if (elapsed > 0.f) {
    float speed = 1.f - (_clock - (_target - (_bufferingMin * _timeout))) / (2.f * _bufferingMin * _timeout);
    float consumed = std::max(0.f, std::min(_target - _clock, elapsed * speed));

    _clock += consumed;
    elapsed -= consumed / speed;
  }

  // Update each entity
  for (auto entity : entities()) {
    auto& action = ecs.getComponent<RPG::ClientActionComponent>(entity);
    auto entityClock = startClock;

    while (true)
    {
      // Update current action
      if (action.action != nullptr)
        action.action->update(((action.next.empty() == true) ? (_clock) : (std::min(_clock, action.next.front().clock))) - entityClock);

      // Start next action
      if (action.next.empty() == false && action.next.front().clock < _clock) {
        entityClock = action.next.front().clock;
        action.action.reset();
        try {
          action.action = action.next.front().builder();
        }
        catch (const std::exception& error) {
          std::wcerr << "[RPG::ClientActionSystem] Warning: failed to build action of entity '" << ecs.getComponent<RPG::EntityComponent>(entity).id << "' (" << error.what() << ")." << std::endl;
        }
        action.next.pop();
      }

      // No more action, stop
      else
        break;
    }
  }
}

void  RPG::ClientActionSystem::handlePacket(const Game::JSON::Object& json)
{
  const auto& type = json.get(L"type").array().get(1).string();

  // Clock update
  if (type == L"clock")
    handleClock(json);

  // New entity action
  else if (type == L"entity")
    handleEntity(json);

  // Load resources
  else if (type == L"load")
    handleLoad(json);

  // Invalid action
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientActionSystem::handleClock(const Game::JSON::Object& json)
{
  // Update local clock target
  _target = (float)json.get(L"clock").number();
}

void  RPG::ClientActionSystem::handleEntity(const Game::JSON::Object& json)
{
  // Find action to perform
  if (json.get(L"action").null() == false) {
    const auto& actionJson = json.get(L"action").object();
    const auto& type = actionJson.get(L"type").string();

    // Move entity
    if (type == L"move")
      action<RPG::ClientMoveAction>(json,
        RPG::Coordinates(actionJson.get(L"target").array()),
        RPG::Coordinates(actionJson.get(L"coordinates").array()),
        RPG::Position(actionJson.get(L"position").array()),
        RPG::StringToDirection(actionJson.get(L"direction").string())
      );
  }

  // Does nothing
  else {
    action<RPG::ClientNullAction>(json);
  }
}

void  RPG::ClientActionSystem::handleLoad(const Game::JSON::Object& json)
{
  const auto& type = json.get(L"type").array().get(2).string();

  // Clock update
  if (type == L"entities")
    handleLoadEntities(json);

  // Invalid load
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientActionSystem::handleLoadEntities(const Game::JSON::Object& json)
{
  // Load each action
  for (const auto& action : json.get(L"actions").array()) {
    handleEntity(action->object());
  }
}

void  RPG::ClientActionSystem::setTimeout(float timeout)
{
  // Set new timeout
  _timeout = std::max(1.f / 256.f, timeout);
}

float RPG::ClientActionSystem::getTimeout() const
{
  // Get current timeout
  return _timeout;
}

void  RPG::ClientActionSystem::setBuffering(float min, float max)
{
  // Set new buffering settings
  _bufferingMin = std::max(1.f, min);
  _bufferingMax = std::max(_bufferingMin, max);
}

std::pair<float, float> RPG::ClientActionSystem::getBuffering() const
{
  // Get buffering setting
  return { _bufferingMin, _bufferingMax };
}

RPG::ServerMoveAction::ServerMoveAction(RPG::ECS& ecs, RPG::ECS::Entity self, const RPG::Coordinates& target) :
  RPG::ServerActionComponent::Action(ecs, self),
  _target(target)
{
  auto& action = ecs.getComponent<RPG::ServerActionComponent>(self);

  // Execute immediatly
  action.mode = RPG::ActionMode::Execute;
  action.start = 0.f;
  action.end = 1.f;
  action.speed = 1.6f;
  action.progress = 1.f;
}

Game::JSON::Object  RPG::ServerMoveAction::json() const
{
  Game::JSON::Object  actionJson;
  auto&               entity = ecs.getComponent<RPG::EntityComponent>(self);

  // Serialize action to JSON
  actionJson.set(L"type", std::wstring(L"move"));
  actionJson.set(L"target", _target.json());
  actionJson.set(L"coordinates", entity.coordinates.json());
  actionJson.set(L"position", entity.position.json());
  actionJson.set(L"direction", RPG::DirectionToString(entity.direction));

  return actionJson;
}

void  RPG::ServerMoveAction::execute()
{
  auto& action = ecs.getComponent<RPG::ServerActionComponent>(self);
  auto& entity = ecs.getComponent<RPG::EntityComponent>(self);
  
  // Move to target
  if (_target != entity.coordinates) {
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

    // Wait for next move
    action.mode = RPG::ActionMode::Execute;
    action.start = 0.f;
    action.end = 1.f;
    action.progress = 0.f;
    action.speed = 1.6f;
  }

  // Target reached, end action
  else
    action.action.reset();
}

void  RPG::ServerMoveAction::refresh()
{
  // TODO
}

void  RPG::ServerMoveAction::interrupt()
{
  auto& entity = ecs.getComponent<RPG::EntityComponent>(self);

  // Move interrupted, stop at current position
  _target = entity.coordinates;
}


RPG::ClientNullAction::ClientNullAction(RPG::ECS& ecs, RPG::ECS::Entity self) :
  RPG::ClientActionComponent::Action(ecs, self)
{
  // Idle animation
  ecs.getSystem<RPG::ClientModelSystem>().setAnimation(self, RPG::Model::Actor::IdleAnimation, RPG::Model::Actor::Mode::Loop, +1.f);
}

void  RPG::ClientNullAction::update(float elapsed)
{
  // Does nothing
}

RPG::ClientMoveAction::ClientMoveAction(RPG::ECS& ecs, RPG::ECS::Entity self, RPG::Coordinates target, RPG::Coordinates coordinates, RPG::Position position, RPG::Direction direction) :
  RPG::ClientActionComponent::Action(ecs, self),
  _target(target),
  _coordinates(coordinates),
  _position(position),
  _direction(direction),
  _remaining(0.f)
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

  auto& action = ecs.getComponent<RPG::ClientActionComponent>(self);

  // Invalid speed
  if (action.speed <= 0.f)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Compute move duration
  _remaining = (1.f - action.progress) / action.speed;

  // Start move animation
  ecs.getSystem<RPG::ClientModelSystem>().setAnimation(self, RPG::Model::Actor::RunAnimation, RPG::Model::Actor::Mode::Loop, +1.f);
}

RPG::ClientMoveAction::~ClientMoveAction()
{
  // Stop run animation
  ecs.getSystem<RPG::ClientModelSystem>().setAnimation(self, RPG::Model::Actor::IdleAnimation, RPG::Model::Actor::Mode::Loop, +1.f);
}

void  RPG::ClientMoveAction::update(float elapsed)
{
  auto& entity = ecs.getComponent<RPG::EntityComponent>(self);

  // Move entity to new position
  entity.position += (_position - entity.position) * (std::min(elapsed, _remaining) / _remaining);

  // Compute remaining time
  _remaining -= std::min(elapsed, _remaining);
}