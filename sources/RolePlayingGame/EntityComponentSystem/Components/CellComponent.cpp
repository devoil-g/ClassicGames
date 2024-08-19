#include "RolePlayingGame/EntityComponentSystem/Components/CellComponent.hpp"

const RPG::Coordinates  RPG::CellComponent::DefaultCoordinates(0, 0);
const float             RPG::CellComponent::DefaultHeight(0.f);
const bool              RPG::CellComponent::DefaultBlocked(false);

RPG::CellComponent::CellComponent() :
  coordinates(0, 0),
  height(0.f),
  blocked(false)
{}

RPG::CellComponent::CellComponent(const Game::JSON::Object& json) :
  coordinates(json.contains("coordinates") ? json.get("coordinates").array() : DefaultCoordinates),
  height(json.contains("height") ? (float)json.get("height").number() : DefaultHeight),
  blocked(json.contains("blocked") ? json.get("blocked").boolean() : DefaultBlocked)
{}

Game::JSON::Object  RPG::CellComponent::json() const
{
  Game::JSON::Object  json;

  // Serialize cell
  if (coordinates != DefaultCoordinates)
    json.set("coordinates", coordinates.json());
  if (height != DefaultHeight)
    json.set("height", height);
  if (blocked != DefaultBlocked)
    json.set("blocked", blocked);

  return json;
}