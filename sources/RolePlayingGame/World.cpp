#include "RolePlayingGame/Server.hpp"
#include "RolePlayingGame/World.hpp"

RPG::ClientWorld::ClientWorld() :
  level(),
  models(),
  textures()
{}

const RPG::Model& RPG::ClientWorld::model(const std::string& name) const
{
  auto it = models.find(name);

  // Model not found
  if (it == models.end())
    return RPG::Model::ErrorModel;

  // Get model from map
  return it->second;
}

const RPG::Texture& RPG::ClientWorld::texture(const std::string& name) const
{
  auto it = textures.find(name);

  // Texture not found
  if (it == textures.end())
    return RPG::Texture::ErrorTexture;

  // Get texture from map
  return it->second;
}

void  RPG::ClientWorld::load(const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(1).string();

  // Load levels
  if (type == "level")
    loadLevel(json);

  // Load models
  else if (type == "models")
    loadModels(json);

  // Load textures
  else if (type == "textures")
    loadTextures(json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientWorld::loadLevel(const Game::JSON::Object& json)
{
  // Load level
  level = std::make_unique<RPG::ClientLevel>(*this, json.get("level").object());
}

void  RPG::ClientWorld::loadModels(const Game::JSON::Object& json)
{
  // Load models
  for (const auto& element : json.get("models").array()._vector)
    models.emplace(element->object().get("name").string(), element->object());
}

void  RPG::ClientWorld::loadTextures(const Game::JSON::Object& json)
{
  // TODO
}

void  RPG::ClientWorld::update(const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(1).string();

  // Update level
  if (type == "level" && level != nullptr)
    level->update(*this, json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientWorld::update(RPG::ClientScene& client, float elapsed)
{
  // Update level
  if (level != nullptr)
    level->update(*this, elapsed);

  // TODO: update inputs
}
void  RPG::ClientWorld::draw()
{
  // Draw level
  if (level != nullptr)
    level->draw(*this);

  // TODO: HUD
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
  Game::JSON::Object  jsonModels;

  // Serialize models
  jsonModels.set("models", Game::JSON::Array());
  for (const auto& [name, model] : models) {
    auto object = model.json();

    // Add name to model
    object.set("name", name);

    // Add model to JSON
    jsonModels.get("models").array().push(std::move(object));
  }

  // Send models
  server.send(id, { "load", "models" }, jsonModels);

  Game::JSON::Object  jsonLevel;

  // Send level
  jsonLevel.set("level", levels.begin()->second.json());
  server.send(id, { "load", "level" }, jsonLevel);
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

  // TODO: select level of the player

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