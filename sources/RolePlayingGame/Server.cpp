#include <iostream>

#include <SFML/Network/IpAddress.hpp>

#include "RolePlayingGame/Server.hpp"

RPG::Server::Server(const std::filesystem::path& config, std::uint16_t port, std::uint32_t address) :
  RPG::TcpServer(port, address),
  _tick(0),
  _world(Game::JSON::load(config))
{
  // TODO: remove this
  std::cout << "[RPG::Server] Server started: " << sf::IpAddress::getPublicAddress() << ":" << getPort() << "." << std::endl;
}

RPG::Server::~Server()
{
  // TODO: remove this
  std::cout << "[RPG::Server] Server stopped." << std::endl;

  // TODO: save
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
  // Add new player
  _world.connect(*this, id);
}

void  RPG::Server::onDisconnect(std::size_t id)
{
  // Remove player
  _world.disconnect(*this, id);
}

void  RPG::Server::onReceive(std::size_t id, const Game::JSON::Object& json)
{
  // TODO: remove this
  std::cout << "[RPG::Server] Received (id: " << id << ", type: " << json.get("type").string() << "): " << json << std::endl;

  // Handle packet
  _world.receive(*this, id, json);
}

void  RPG::Server::onTick()
{
  // New tick
  _tick += 1;

  // Update world
  _world.update(*this, 1.f / getTickrate());
}