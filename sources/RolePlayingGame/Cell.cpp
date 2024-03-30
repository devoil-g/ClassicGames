#include "RolePlayingGame/Cell.hpp"

RPG::ClientCell::ClientCell(const RPG::ClientWorld& world, const RPG::ClientLevel& level, const Game::JSON::Object& json) :
  coordinates(json.get("coordinates").array()),
  blocked(json.contains("blocked") ? json.get("blocked").boolean() : RPG::Cell::DefaultBlocked),
  height(json.contains("height") ? (std::int16_t)json.get("height").number() : RPG::Cell::DefaultHeight)
{}

Math::Vector<2, int>  RPG::ClientCell::position() const
{
  // Compute cell position in world
  return {
    (coordinates.x() - coordinates.y()) * (int)RPG::Cell::Width,
    (coordinates.x() + coordinates.y()) * ((int)RPG::Cell::Height / 2) - height
  };
}

RPG::Bounds RPG::ClientCell::bounds() const
{
  // Get cell bounds
  return RPG::Bounds(
    { -(int)RPG::Cell::Width / 2, -(int)RPG::Cell::Height / 2 - height },
    { (int)RPG::Cell::Width, (int)RPG::Cell::Height }
  );
}

RPG::ServerCell::ServerCell(const Game::JSON::Object& json) :
  blocked(json.contains("blocked") ? json.get("blocked").boolean() : RPG::Cell::DefaultBlocked),
  height(json.contains("height") ? (std::int16_t)json.get("height").number() : RPG::Cell::DefaultHeight)
{}

Game::JSON::Object  RPG::ServerCell::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  if (blocked != RPG::Cell::DefaultBlocked)
    json.set("blocked", blocked);
  if (height != RPG::Cell::DefaultHeight)
    json.set("height", (double)height);

  return json;
}