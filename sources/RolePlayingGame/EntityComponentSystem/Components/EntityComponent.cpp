#include "RolePlayingGame/EntityComponentSystem/Components/EntityComponent.hpp"

const RPG::Coordinates  RPG::EntityComponent::DefaultCoordinates(0, 0);
const RPG::Direction    RPG::EntityComponent::DefaultDirection(RPG::Direction::DirectionNorth);
const RPG::Position     RPG::EntityComponent::DefaultPosition(0.f, 0.f, 0.f);
const std::string       RPG::EntityComponent::DefaultModel("error");

RPG::EntityComponent::EntityComponent() :
  id(),
  coordinates(DefaultCoordinates),
  direction(DefaultDirection),
  position(DefaultPosition),
  model(DefaultModel),
  attack(10),
  defense(8)
{}

RPG::EntityComponent::EntityComponent(const Game::JSON::Object& json) :
  id(json.get("id").string()),
  coordinates(json.contains("coordinates") ? json.get("coordinates").array() : DefaultCoordinates),
  direction(json.contains("direction") ? RPG::StringToDirection(json.get("direction").string()) : DefaultDirection),
  position(json.contains("position") ? json.get("position").array() : DefaultPosition),
  model(json.contains("model") ? json.get("model").string() : DefaultModel),
  attack(10),
  defense(8)
{}

Game::JSON::Object  RPG::EntityComponent::json() const
{
  Game::JSON::Object  json;

  // Serialize entity
  json.set("id", id);
  if (coordinates != DefaultCoordinates)
    json.set("coordinates", coordinates.json());
  if (direction != DefaultDirection)
    json.set("direction", RPG::DirectionToString(direction));
  if (position != DefaultPosition)
    json.set("position", position.json());
  if (model != DefaultModel)
    json.set("model", model);

  return json;
}