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
  coordinates(json.contains(L"coordinates") ? json.get(L"coordinates").array() : DefaultCoordinates),
  height(json.contains(L"height") ? (float)json.get(L"height").number() : DefaultHeight),
  blocked(json.contains(L"blocked") ? json.get(L"blocked").boolean() : DefaultBlocked)
{}

Game::JSON::Object  RPG::CellComponent::json() const
{
  Game::JSON::Object  json;

  // Serialize cell
  if (coordinates != DefaultCoordinates)
    json.set(L"coordinates", coordinates.json());
  if (height != DefaultHeight)
    json.set(L"height", height);
  if (blocked != DefaultBlocked)
    json.set(L"blocked", blocked);

  return json;
}