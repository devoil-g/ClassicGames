#include "RolePlayingGame/Components.hpp"

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

const std::size_t RPG::NetworkComponent::NoController(0);

RPG::NetworkComponent::NetworkComponent() :
  controller(NoController)
{}

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

RPG::DisplayComponent::DisplayComponent() :
  position(0.f, 0.f, 0.f),
  direction(RPG::Direction::DirectionNorth),
  color(RPG::Color::White),
  outline(RPG::Color::Transparent),
  model(nullptr),
  animation(nullptr),
  frame(0),
  elapsed(0.f),
  loop(false)
{}

RPG::ParticleComponent::ParticleComponent() :
  physicsSpeed(0.f, 0.f, 0.f),
  physicsGravity(0.f, 0.f, 0.f),
  physicsDrag(0.f),
  physicsFloor(0.f),
  colorStart(RPG::Color::White),
  colorEnd(RPG::Color::White),
  durationFadeIn(0.f),
  durationLife(0.f),
  durationFadeOut(0.f),
  elapsed(0.f)
{}

RPG::ParticleEmitterComponent::ParticleEmitterComponent() :
  size(0.f, 0.f, 0.f),
  position(0.f, 0.f, 0.f),
  frequencyLow(0.f),
  frequencyHigh(0.f),
  particleLow(),
  particleHigh(),
  model(""),
  next(0.f),
  duration(0.f)
{}