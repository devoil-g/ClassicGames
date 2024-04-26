#include "RolePlayingGame/World.hpp"
#include "RolePlayingGame/Level.hpp"
#include "System/Library/TextureLibrary.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

RPG::ClientLevel::ClientLevel(RPG::ClientWorld& world, const Game::JSON::Object& json) :
  name(json.get("name").string()),
  color(json.contains("color") ? json.get("color").object() : RPG::Level::DefaultColor),
  cells(),
  entities()
{
  // Get each cells
  for (const auto& element : json.get("cells").array()._vector)
    cells.emplace_back(world, *this, element->object());

  // Get each entities
  for (const auto& element : json.get("entities").array()._vector)
    entities.emplace_back(world, *this, element->object());

  // Setup camera
  camera.setPositionDrag(0.03125f);
  camera.setZoomDrag(0.03125f);
}

RPG::ClientCell& RPG::ClientLevel::cell(const RPG::Coordinates& coordinates)
{
  // Find cell in level
  for (auto& cell : cells)
    if (cell.coordinates == coordinates)
      return cell;

  // Cell not in level
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

RPG::ClientCell& RPG::ClientLevel::cell(int x, int y)
{
  // Get cell
  return cell({ x, y });
}

const RPG::ClientCell& RPG::ClientLevel::cell(const RPG::Coordinates& coordinates) const
{
  // Find cell in level
  for (const auto& cell : cells)
    if (cell.coordinates == coordinates)
      return cell;
  
  // Cell not in level
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

const RPG::ClientCell& RPG::ClientLevel::cell(int x, int y) const
{
  // Get cell
  return cell({ x, y });
}

RPG::ClientEntity& RPG::ClientLevel::entity(std::size_t id)
{
  // Find entity in level
  for (auto& entity : entities)
    if (entity.id == id)
      return entity;

  // Entity not in level
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

const RPG::ClientEntity& RPG::ClientLevel::entity(std::size_t id) const
{
  // Find entity in level
  for (const auto& entity : entities)
    if (entity.id == id)
      return entity;

  // Entity not in level
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

Math::Vector<3> RPG::ClientLevel::position(const RPG::ClientEntity& entity) const
{
  // Get entity position in world
  return position(entity.coordinates, entity.position);
}

Math::Vector<3> RPG::ClientLevel::position(const RPG::Coordinates& coordinates, const RPG::Position& position) const
{
  Math::Vector<3> result = position;

  // Add cell coordinates
  result.x() += (coordinates.x() - coordinates.y()) * (int)RPG::Cell::Width;
  result.y() -= (coordinates.x() + coordinates.y()) * ((int)RPG::Cell::Height / 2);

  // Add height of cell if existing
  try {
    result.z() += cell(coordinates).height;
  }
  catch (const std::exception&) {}

  return result;
}

void  RPG::ClientLevel::update(RPG::ClientWorld& world, const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(2).string();

  // Update entity
  if (type == "entity")
    updateEntity(world, json);
  
  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientLevel::updateEntity(RPG::ClientWorld& world, const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(3).string();

  // Update entity
  if (type == "update")
    entity((std::size_t)json.get("id").number()).update(world, *this, json);

  // Create entity
  else if (type == "create")
    // TODO: check that ID is not already used
    entities.emplace_back(world, *this, json);

  // Delete entity
  else if (type == "delete") {
    auto id = (std::size_t)json.get("id").number();

    // Remove entities matching ID
    entities.remove_if([id](const auto& entity) { return entity.id == id; });
  }

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientLevel::update(RPG::ClientWorld& world, float elapsed)
{
  // Update entities
  for (auto& entity : entities)
    entity.update(world, *this, elapsed);
  
  // Update camera
  camera.update(elapsed);

  // Sort cells by depth
  cells.sort([](const auto& a, const auto& b) {
    if (a.coordinates.x() + a.coordinates.y() < b.coordinates.x() + b.coordinates.y())
      return true;
    if (a.coordinates.x() + a.coordinates.y() > b.coordinates.x() + b.coordinates.y())
      return false;
    return a.coordinates.x() < b.coordinates.x();
    });

  // Sort entities by depth
  entities.sort([this](const auto& a, const auto& b) {
    RPG::Position aPosition = position(a);
    RPG::Position bPosition = position(b);

    if (aPosition.y() > bPosition.y())
      return true;
    if (aPosition.y() < bPosition.y())
      return false;
    if (aPosition.z() > bPosition.z())
      return true;
    if (aPosition.z() < bPosition.z())
      return false;
    return aPosition.x() < bPosition.x();
    });
}

void  RPG::ClientLevel::draw(const RPG::ClientWorld& world) const
{
  auto& window = Game::Window::Instance();

  // Clear window
  window.window().clear(sf::Color(color.raw));

  // Set world view
  camera.set();

  // Draw entities
  for (auto entity = entities.rbegin(); entity != entities.rend(); entity++)
    entity->draw(world, *this);

  // TODO: draw cursor

  // Reset to default view
  camera.set();
}

const RPG::ServerCell RPG::ServerLevel::InvalidCell = Game::JSON::Object();

RPG::ServerLevel::ServerLevel(const RPG::ServerWorld& world, const Game::JSON::Object& json) :
  name(json.get("name").string()),
  color(json.contains("color") ? json.get("color").object() : RPG::Level::DefaultColor),
  cells(),
  entities()
{
  // Get each cells
  for (const auto& element : json.get("cells").array()._vector)
    cells.emplace(element->object().get("coordinates").array(), element->object());

  // Get each entities
  for (const auto& element : json.get("entities").array()._vector)
    entities.emplace_back(world, *this, element->object());
}

RPG::ServerCell& RPG::ServerLevel::cell(const RPG::Coordinates& coordinates)
{
  auto it = cells.find(coordinates);

  // Cell not in map
  if (it == cells.end())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  return it->second;
}

RPG::ServerCell& RPG::ServerLevel::cell(int x, int y)
{
  // Get cell
  return cell({ x, y });
}

const RPG::ServerCell& RPG::ServerLevel::cell(const RPG::Coordinates& coordinates) const
{
  auto it = cells.find(coordinates);

  // Cell not in map
  if (it == cells.end())
    return InvalidCell;

  return it->second;
}

const RPG::ServerCell& RPG::ServerLevel::cell(int x, int y) const
{
  // Get cell
  return cell({ x, y });
}

Game::JSON::Object  RPG::ServerLevel::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  json.set("name", name);
  if (color != RPG::Level::DefaultColor) {
    json.set("color", color.json());
    std::cout << "color: " << color.json() << std::endl;
  }

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