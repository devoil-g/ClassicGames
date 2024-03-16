#include "RolePlayingGame/Level.hpp"

const RPG::Color  RPG::Level::DefaultColor = RPG::Color::Default;
const std::string RPG::Level::DefaultBackground = "";
const std::string RPG::Level::DefaultForeground = "";

RPG::Level::Level() :
  cells(),
  color(DefaultColor),
  background(DefaultBackground),
  foreground(DefaultForeground)
{}

RPG::Level::Level(const Game::JSON::Object& json) :
  cells(),
  color(json.contains("color") ? json.get("color").object() : DefaultColor),
  background(json.contains("background") ? json.get("background").string() : DefaultBackground),
  foreground(json.contains("foreground") ? json.get("foreground").string() : DefaultForeground)
{
  // Get each cell
  for (const auto& element : json.get("cells").array()._vector)
    cells.emplace(element->object().get("coordinates").array(), element->object());

  // Get each entity
  for (const auto& element : json.get("entities").array()._vector)
    entities.emplace_back(element->object());
}

RPG::Level::Cell& RPG::Level::getCell(const RPG::Coordinates& coordinates)
{
  auto it = cells.find(coordinates);

  // Cell not in map
  if (it == cells.end())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  return it->second;
}

RPG::Level::Cell& RPG::Level::getCell(int x, int y)
{
  // Get cell
  return getCell({ x, y });
}

const RPG::Level::Cell& RPG::Level::getCell(const RPG::Coordinates& coordinates) const
{
  auto it = cells.find(coordinates);

  // Cell not in map
  if (it == cells.end())
    return InvalidCell;

  return it->second;
}

const RPG::Level::Cell& RPG::Level::getCell(int x, int y) const
{
  // Get cell
  return getCell({ x, y });
}

Game::JSON::Object  RPG::Level::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  if (color != DefaultColor)
    json.set("color", color.json());
  if (background != DefaultBackground)
    json.set("background", background);
  if (foreground != DefaultForeground)
    json.set("foreground", foreground);

  // Serialize cells to JSON
  json.set("cells", Game::JSON::Array());

  // Pre-allocate cells array
  json.get("cells").array()._vector.reserve(cells.size());

  // Get each cell
  for (const auto& [coordinates, cell] : cells) {
    Game::JSON::Object value = cell.json();

    value.set("coordinates", coordinates.json());
    json.get("cells").array().push(std::move(value));
  }

  // Serialize entities to JSON
  json.set("entities", Game::JSON::Array());

  // Pre-allocate entities array
  json.get("entities").array()._vector.reserve(entities.size());

  // Get each entity
  for (const auto& entity : entities)
    json.get("entities").array().push(entity.json());

  return json;
}

const bool RPG::Level::Cell::DefaultSelectable = false;
const int  RPG::Level::Cell::DefaultHeight = 0;

RPG::Level::Cell::Cell(const Game::JSON::Object& json) :
  selectable(json.contains("selectable") ? json.get("selectable").boolean() : DefaultSelectable),
  height(json.contains("height") ? (std::int16_t)json.get("height").number() : DefaultHeight),
  tags()
{
  // Extract tags from JSON
  if (json.contains("tags"))
    for (const auto& tag : json.get("tags").array()._vector)
      tags.emplace(tag->string());
}

Game::JSON::Object  RPG::Level::Cell::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  if (selectable != DefaultSelectable)
    json.set("selectable", selectable);
  if (height != DefaultHeight)
    json.set("height", (double)height);
  if (tags.empty() == false) {
    json.set("tags", Game::JSON::Array());
    for (const auto& tag : tags)
      json.get("tags").array().push(tag);
  }

  return json;
}

RPG::Bounds RPG::Level::Cell::bounds() const
{
  // Get cell bounds
  return RPG::Bounds(
    { -(int)RPG::Level::Cell::Width / 2, -(int)RPG::Level::Cell::Height / 2 - height },
    { (int)RPG::Level::Cell::Width, (int)RPG::Level::Cell::Height }
  );
}