#include "RolePlayingGame/EntityComponentSystem/Components/EntityComponent.hpp"
#include "System/Utilities.hpp"

const RPG::Coordinates  RPG::EntityComponent::DefaultCoordinates(0, 0);
const RPG::Direction    RPG::EntityComponent::DefaultDirection(RPG::Direction::DirectionNorth);
const RPG::Position     RPG::EntityComponent::DefaultPosition(0.f, 0.f, 0.f);
const std::wstring      RPG::EntityComponent::DefaultModel(L"error");

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
  id(json.get(L"id").string()),
  coordinates(json.contains(L"coordinates") ? json.get(L"coordinates").array() : DefaultCoordinates),
  direction(json.contains(L"direction") ? RPG::StringToDirection(json.get(L"direction").string()) : DefaultDirection),
  position(json.contains(L"position") ? json.get(L"position").array() : DefaultPosition),
  model(json.contains(L"model") ? json.get(L"model").string() : DefaultModel),
  attack(10),
  defense(8)
{}

Game::JSON::Object  RPG::EntityComponent::json() const
{
  Game::JSON::Object  json;

  // Serialize entity
  json.set(L"id", id);
  if (coordinates != DefaultCoordinates)
    json.set(L"coordinates", coordinates.json());
  if (direction != DefaultDirection)
    json.set(L"direction", RPG::DirectionToString(direction));
  if (position != DefaultPosition)
    json.set(L"position", position.json());
  if (model != DefaultModel)
    json.set(L"model", model);

  return json;
}