#include "RolePlayingGame/Entity.hpp"

std::uint64_t RPG::Entity::_maxId = 0;

std::uint64_t RPG::Entity::generateId()
{
  // Just get the next max ID
  return ++_maxId;
}

const RPG::Coordinates RPG::Entity::DefaultCoordinates = { 0, 0 };
const RPG::Position    RPG::Entity::DefaultPosition = { 0.f, 0.f, 0.f };
const RPG::Direction   RPG::Entity::DefaultDirection = RPG::Direction::DirectionNorth;
const std::string      RPG::Entity::DefaultModel = "";

RPG::Entity::Entity(const Game::JSON::Object& json) :
  id(json.contains("id") ? (std::uint64_t)json.get("id").number() : generateId()),
  coordinates(json.contains("coordinates") ? json.get("coordinates").array() : DefaultCoordinates),
  position(json.contains("position") ? json.get("position").array() : DefaultPosition),
  direction(json.contains("direction") ? (RPG::Direction)json.get("direction").number() : DefaultDirection),
  model(json.contains("model") ? json.get("model").string() : DefaultModel)
{
  // Always remember highest ID for generator
  _maxId = std::max(_maxId, id);

  // Entity doesn't move
  move.target = coordinates;
}

Game::JSON::Object RPG::Entity::json() const
{
  Game::JSON::Object json;

  // Serialize to JSON
  json.set("id", (double)id);
  if (coordinates != DefaultCoordinates)
    json.set("coordinates", coordinates.json());
  if (position != DefaultPosition)
    json.set("position", position.json());
  if (direction != DefaultDirection)
    json.set("direction", (double)direction);
  if (model != DefaultModel)
    json.set("model", model);

  return json;
}

void  RPG::Entity::updateClientMove(float elapsed)
{
  // Update completion of move
  move.completion = std::min(1.f, move.completion + move.speed * elapsed);


}

void  RPG::Entity::updateClient(float elapsed)
{
  // Update completion of move
  move.completion = std::min(1.f, move.completion + move.speed * elapsed);
}

void  RPG::Entity::updateServer(float elapsed)
{

}

RPG::Entity::Move::Move() :
  speed(0.f),
  completion(1.f),
  position(0.f, 0.f, 0.f),
  target(0, 0)
{}