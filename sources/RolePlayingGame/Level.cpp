#include "RolePlayingGame/World.hpp"
#include "RolePlayingGame/Level.hpp"
#include "System/Library/TextureLibrary.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

RPG::ClientLevel::ClientLevel(const RPG::ClientWorld& world, const Game::JSON::Object& json) :
  name(json.get("name").string()),
  color(json.contains("color") ? json.get("color").object() : RPG::Level::DefaultColor),
  background(json.contains("background") ? json.get("background").string() : ""),
  foreground(json.contains("foreground") ? json.get("foreground").string() : ""),
  cells(),
  entities()
{
  // Get each cells
  for (const auto& element : json.get("cells").array()._vector)
    cells.emplace_back(world, *this, element->object());

  // Get each entities
  for (const auto& element : json.get("entities").array()._vector)
    entities.emplace_back(world, *this, element->object());
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

Math::Vector<2, float>  RPG::ClientLevel::computePosition(const RPG::Coordinates& coordinates, const RPG::Position& position) const
{
  Math::Vector<2, float> result;

  // Cell coordinates
  result.x() += (coordinates.x() - coordinates.y()) * (int)RPG::Cell::Width;
  result.y() += (coordinates.x() + coordinates.y()) * ((int)RPG::Cell::Height / 2);

  // Add height of cell if existing
  try {
    result.y() -= cell(coordinates).height;
  }
  catch (const std::exception&) {}

  // Position
  result.x() += position.x();
  result.y() += position.y() - position.z();

  return result;
}

void  RPG::ClientLevel::update(const RPG::ClientWorld& world, const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(2).string();

  // Update entity
  if (type == "entity")
    updateEntity(world, json);
  
  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientLevel::updateEntity(const RPG::ClientWorld& world, const Game::JSON::Object& json)
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

void  RPG::ClientLevel::update(const RPG::ClientWorld& world, float elapsed)
{
  // Update background and foreground
  background.update(world, elapsed);
  foreground.update(world, elapsed);

  // Update entities
  for (auto& entity : entities)
    entity.update(world, *this, elapsed);

  // Sort cells by coordinates for rendering and cursor
  cells.sort([](const auto& a, const auto& b) {
    if (a.coordinates.x() + a.coordinates.y() > b.coordinates.x() + b.coordinates.y())
      return true;
    if (a.coordinates.x() + a.coordinates.y() < b.coordinates.x() + b.coordinates.y())
      return false;
    if (a.coordinates.x() < b.coordinates.x())
      return true;
    if (a.coordinates.x() > b.coordinates.x())
      return false;
    return true;
    });

  // Sort entities by coordinates/position for rendering and cursor
  entities.sort([](const auto& a, const auto& b) {
    if (a.coordinates.x() + a.coordinates.y() > b.coordinates.x() + b.coordinates.y())
      return true;
    if (a.coordinates.x() + a.coordinates.y() < b.coordinates.x() + b.coordinates.y())
      return false;
    if (a.position.y() < b.position.y())
      return true;
    if (a.position.y() > b.position.y())
      return false;
    if (a.position.z() < b.position.z())
      return true;
    if (a.position.z() > b.position.z())
      return false;
    if (a.position.x() < b.position.x())
      return true;
    if (a.position.x() > b.position.x())
      return false;
    return true;
    });

}

void  RPG::ClientLevel::draw(const RPG::ClientWorld& world) const
{
  auto& window = Game::Window::Instance();

  // Clear window
  window.window().clear(sf::Color(color.raw));

  // Draw background
  background.draw(world);

  // Draw entities
  for (const auto& entity : entities)
    entity.draw(world, *this);

  // Draw foreground
  foreground.draw(world);
}

RPG::ClientLevel::Layer::Layer(const std::string& model) :
  model(model),
  elements()
{}

void  RPG::ClientLevel::Layer::update(const RPG::ClientWorld& world, float elapsed)
{
  // No layer
  if (model.empty() == true)
    return;

  auto model_it = world.models.find(model);

  // No model for layer
  if (model_it == world.models.end())
    return;

  // Update each animation
  for (const auto& [name, animation] : model_it->second.animations)
    elements[name].update(animation, elapsed);
}

void  RPG::ClientLevel::Layer::draw(const RPG::ClientWorld& world) const
{
  // No layer
  if (model.empty() == true)
    return;

  auto model_it = world.models.find(model);

  // No model for layer
  if (model_it == world.models.end())
    return;

  // Draw each animation
  for (const auto& [name, animation] : model_it->second.animations) {
    auto element_it = elements.find(name);

    if (element_it != elements.end())
      element_it->second.draw(model_it->second.spritesheet, animation);
  }
}

RPG::ClientLevel::Layer::Element::Element() :
  frame(0),
  elapsed(0.f)
{}

void  RPG::ClientLevel::Layer::Element::update(const RPG::Model::Animation& animation, float elapsed)
{
  // No frames
  if (animation.frames.empty() == true)
    return;

  this->elapsed += elapsed;

  // Skip frames according to elasped time
  while (this->elapsed > animation.frames[frame % animation.frames.size()].duration && animation.frames[frame % animation.frames.size()].duration != 0.f) {
    this->elapsed -= animation.frames[frame % animation.frames.size()].duration;
    frame = (frame + 1) % animation.frames.size();
  }
}

void  RPG::ClientLevel::Layer::Element::draw(const std::string& spritesheet, const RPG::Model::Animation& animation) const
{
  // No frames
  if (animation.frames.empty() == true)
    return;

  // Draw frame
  animation.frames[frame % animation.frames.size()].draw(spritesheet, { 0.f, 0.f });
}

const RPG::ServerCell RPG::ServerLevel::InvalidCell = Game::JSON::Object();

RPG::ServerLevel::ServerLevel(const Game::JSON::Object& json) :
  color(json.contains("color") ? json.get("color").object() : RPG::Level::DefaultColor),
  background(json.contains("background") ? json.get("background").string() : ""),
  foreground(json.contains("foreground") ? json.get("foreground").string() : ""),
  cells(),
  entities()
{
  // Get each cells
  for (const auto& element : json.get("cells").array()._vector)
    cells.emplace(element->object().get("coordinates").array(), element->object());

  // Get each entities
  for (const auto& element : json.get("entities").array()._vector)
    entities.emplace_back(element->object());
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
  if (color != RPG::Level::DefaultColor)
    json.set("color", color.json());
  if (background.empty() == false)
    json.set("background", background);
  if (foreground.empty() == false)
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