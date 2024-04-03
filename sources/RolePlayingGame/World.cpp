#include "RolePlayingGame/World.hpp"

RPG::ClientWorld::ClientWorld() :
  levels(),
  models(),
  textures(),
  camera()
{
  // Setup camera
  camera.setPositionDrag(0.015625f);
  camera.setZoomDrag(0.03125f);
}

RPG::Model& RPG::ClientWorld::model(const std::string& name)
{
  // Get model from map
  return models.at(name);
}

const RPG::Model& RPG::ClientWorld::model(const std::string& name) const
{
  // Get model from map
  return models.at(name);
}

RPG::Texture& RPG::ClientWorld::texture(const std::string& name)
{
  // Get texture from map
  return textures.emplace(name, name).first->second;
}

const RPG::Texture& RPG::ClientWorld::texture(const std::string& name) const
{
  // Get texture from map
  return textures.at(name);
}

RPG::ClientEntity& RPG::ClientWorld::entity(std::size_t id)
{
  // Search entity in levels
  for (auto& [_, level] : levels) {
    try {
      return level.entity(id);
    }
    catch (...) {}
  }

  // Entity not found
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

const RPG::ClientEntity& RPG::ClientWorld::entity(std::size_t id) const
{
  // Search entity in levels
  for (const auto& [_, level] : levels) {
    try {
      return level.entity(id);
    }
    catch (...) {}
  }

  // Entity not found
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientWorld::load(const Game::JSON::Object& json)
{
  // Load models
  for (const auto& element : json.get("models").array()._vector)
    models.emplace(element->object().get("name").string(), element->object());

  // Load levels
  for (const auto& element : json.get("levels").array()._vector)
    levels.emplace(std::piecewise_construct,
      std::forward_as_tuple(element->object().get("name").string()),
      std::forward_as_tuple(*this, element->object()));
}

void  RPG::ClientWorld::update(const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(1).string();

  // Update level
  if (type == "level")
    levels.at(json.get("level").string()).update(*this, json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientWorld::update(RPG::ClientScene& client, float elapsed)
{
  // Update levels
  for (auto& [_, level] : levels)
    level.update(*this, elapsed);

  // TODO: update inputs

  // Update camera
  camera.update(elapsed);
}
void  RPG::ClientWorld::draw()
{
  // Set world camera
  camera.set();

  // TODO: draw current level

  // Reset window view
  camera.reset();
}

RPG::ServerWorld::ServerWorld(const Game::JSON::Object& json) :
  levels(),
  models()
{
  // Load models
  for (const auto& element : json.get("models").array()._vector)
    models.emplace(element->object().get("name").string(), element->object());

  // Load levels
  for (const auto& element : json.get("levels").array()._vector)
    levels.emplace(std::piecewise_construct,
      std::forward_as_tuple(element->object().get("name").string()),
      std::forward_as_tuple(*this, element->object()));
}

void  RPG::ServerWorld::connect(RPG::Server& server, std::size_t id)
{
  // TODO
}

void  RPG::ServerWorld::disconnect(RPG::Server& server, std::size_t id)
{
  // TODO
}

void  RPG::ServerWorld::receive(RPG::Server& server, std::size_t id, const Game::JSON::Object& json)
{
  // TODO
}

void  RPG::ServerWorld::update(RPG::Server& server, float elapsed)
{
  // TODO
}

Game::JSON::Object  RPG::ServerWorld::json() const
{
  Game::JSON::Object  json;

  // Serialize models
  json.set("models", Game::JSON::Array());
  for (const auto& [name, model] : models) {
    auto object = model.json();

    // Add name to model
    object.set("name", name);

    // Add model to JSON
    json.get("models").array().push(std::move(object));
  }

  // Serialize models
  json.set("levels", Game::JSON::Array());
  for (const auto& [name, level] : levels) {
    auto object = level.json();

    // Check level name
    if (object.get("name").string() != name)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    // Add model to JSON
    json.get("levels").array().push(std::move(object));
  }

  return json;
}