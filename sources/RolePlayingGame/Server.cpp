#include <iostream>

#include <SFML/Network/IpAddress.hpp>

#include "RolePlayingGame/EntityComponentSystem/Components/CellComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ModelComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/EntityComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ControllerComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/BoardSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ModelSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/EntitySystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ControllerSystem.hpp"
#include "RolePlayingGame/Server.hpp"
#include "System/Config.hpp"

RPG::Server::Server(const std::filesystem::path& config, std::uint16_t port, std::uint32_t address) :
  RPG::TcpServer(port, address),
  _tick(0),
  _ecs()
{
  // Register ECS components
  _ecs.addComponent<RPG::CellComponent>();
  _ecs.addComponent<RPG::EntityComponent>();
  _ecs.addComponent<RPG::ModelComponent>(); // NOTE: not really used in server, only for model library
  _ecs.addComponent<RPG::ControllerComponent>();

  RPG::ECS::Signature signature;

  // Register ECS systems
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::CellComponent>());
  _ecs.addSystem<RPG::ServerBoardSystem>(signature);
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::EntityComponent>());
  _ecs.addSystem<RPG::ServerEntitySystem>(signature);
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::EntityComponent>());
  signature.set(_ecs.typeComponent<RPG::ControllerComponent>());
  _ecs.addSystem<RPG::ServerControllerSystem>(signature);
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::ModelComponent>());
  _ecs.addSystem<RPG::ServerModelSystem>(signature);

  // Load level
  load(Game::Config::ExecutablePath / "assets" / "rpg" / "world.json");

  // TODO: remove this
  std::cout << "[RPG::Server] Server started: " << sf::IpAddress::getPublicAddress().value_or(sf::IpAddress(0)) << ":" << getPort() << "." << std::endl;
}

RPG::Server::~Server()
{
  // Manually stop server
  stop();
  wait();

  // TODO: save

  // TODO: remove this
  std::cout << "[RPG::Server] Server stopped." << std::endl;
}

void  RPG::Server::send(std::size_t id, const std::vector<std::string>& type, Game::JSON::Object& json)
{
  // Add mandatory data to JSON
  header(json, type);

  // Send to client
  RPG::TcpServer::send(id, json);
}

void  RPG::Server::broadcast(const std::vector<std::string>& type, Game::JSON::Object& json)
{
  // Add mandatory data to JSON
  header(json, type);

  // Broadcast to clients
  RPG::TcpServer::broadcast(json);
}

void  RPG::Server::header(Game::JSON::Object& json, const std::vector<std::string>& type) const
{
  Game::JSON::Array typeArray;

  // Serialize type
  typeArray._vector.reserve(type.size());
  for (const auto& field : type)
    typeArray.push(field);
  json.set("type", std::move(typeArray));

  // Serialize tick
  json.set("tick", (double)_tick);
}

void  RPG::Server::kick(std::size_t id)
{
  // Remove client
  RPG::TcpServer::kick(id);
}

void  RPG::Server::onConnect(std::size_t id)
{
  Game::JSON::Object  json;

  // Send models
  json.clear();
  json.set("models", _ecs.getSystem<RPG::ServerModelSystem>().json(_ecs));
  send(id, { "model", "load", "models" }, json);

  // Send cells
  json.clear();
  json.set("cells", _ecs.getSystem<RPG::ServerBoardSystem>().json(_ecs));
  send(id, { "board", "load", "cells" }, json);

  // Send entities
  json.clear();
  json.set("entities", _ecs.getSystem<RPG::ServerEntitySystem>().json(_ecs));
  send(id, { "entity", "load", "entities" }, json);

  // Add new player
  _ecs.getSystem<RPG::ServerControllerSystem>().connect(_ecs, *this, id);
}

void  RPG::Server::onDisconnect(std::size_t id)
{
  // Remove player
  _ecs.getSystem<RPG::ServerControllerSystem>().disconnect(_ecs, *this, id);
}

void  RPG::Server::onReceive(std::size_t id, const Game::JSON::Object& json)
{
  // TODO: remove this
  std::cout << "[RPG::Server] Received (id: " << id << ", type: " << json.get("type").string() << "): " << json << std::endl;

  // Handle packet
  // TODO
}

void  RPG::Server::onTick()
{
  // New tick
  _tick += 1;

  // Update world
  // TODO
  //_world.update(*this, 1.f / getTickrate());
}

void  RPG::Server::load(const std::filesystem::path& path)
{
  Game::JSON::Object json = Game::JSON::load(path);

  // Load models
  _ecs.getSystem<RPG::ServerModelSystem>().load(_ecs, json.get("models").array());

  // Create board cells
  _ecs.getSystem<RPG::ServerBoardSystem>().load(_ecs, json.get("cells").array());

  // Create game entities
  _ecs.getSystem<RPG::ServerEntitySystem>().load(_ecs, json.get("entities").array());
}