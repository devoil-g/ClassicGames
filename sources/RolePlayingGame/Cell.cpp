#include "RolePlayingGame/Cell.hpp"
#include "RolePlayingGame/Level.hpp"

RPG::ClientCell::ClientCell(const RPG::ClientWorld& world, const RPG::ClientLevel& level, const Game::JSON::Object& json) :
  coordinates(json.get("coordinates").array()),
  blocked(json.contains("blocked") ? json.get("blocked").boolean() : RPG::Cell::DefaultBlocked),
  height(json.contains("height") ? (std::int16_t)json.get("height").number() : RPG::Cell::DefaultHeight)
{}

RPG::Bounds RPG::ClientCell::bounds(const RPG::ClientWorld& world, const RPG::ClientLevel& level) const
{
  auto position = level.position(coordinates);

  // Get cell bounds
  return RPG::Bounds(
    { 
      std::round((position.x() - (float)RPG::Cell::Width / 2.f)),
      std::round((position.y() - position.z() - (float)RPG::Cell::Height / 2.f))
    },
    {
      (float)RPG::Cell::Width,
      (float)RPG::Cell::Height
    }
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