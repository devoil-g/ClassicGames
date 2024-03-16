#include <array>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>

#include <SFML/Network/Packet.hpp>

#include "RolePlayingGame/GameServer.hpp"
#include "System/Config.hpp"

RPG::GameServer::GameServer(const std::filesystem::path& config, std::uint16_t port, std::uint32_t address) :
  RPG::TcpServer(port, address),
  _clients(),
  _tick(0),
  _level(Game::JSON::load(config))
{
  std::cout << "[RPG::GameServer] Server started: " << sf::IpAddress::getPublicAddress() << ":" << getPort() << "." << std::endl;
}

RPG::GameServer::~GameServer()
{
  std::cout << "[RPG::GameServer] Server stopped." << std::endl;
}

void  RPG::GameServer::send(std::size_t id, const std::string& type, Game::JSON::Object& json)
{
  // Add mandatory data to JSON
  json.map["type"] = std::make_unique<Game::JSON::String>(type);
  json.map["tick"] = std::make_unique<Game::JSON::Number>((double)_tick);

  // Send to client
  TcpServer::send(id, json);
}

void  RPG::GameServer::broadcast(const std::string& type, Game::JSON::Object& json)
{
  // Add mandatory data to JSON
  json.map["type"] = std::make_unique<Game::JSON::String>(type);
  json.map["tick"] = std::make_unique<Game::JSON::Number>((double)_tick);

  // Broadcast to clients
  TcpServer::broadcast(json);
}

void  RPG::GameServer::kick(std::size_t id)
{
  // Remove client
  TcpServer::kick(id);
}

void  RPG::GameServer::onConnect(std::size_t id)
{
  _clients.emplace_back(id);

  Game::JSON::Object  json = _level.json();

  send(id, "LevelLoad", json);
}

void  RPG::GameServer::onDisconnect(std::size_t id)
{
  _clients.remove_if([id](const auto& client) { return client.id == id; });
}

void  RPG::GameServer::onReceive(std::size_t id, const Game::JSON::Object& json)
{
  // TODO: handle packet
  std::cout << "Server received (id: " << id << ", type: " << json.map.at("type")->string() << "): " << json << std::endl;

}

void  RPG::GameServer::onTick()
{
  // New tick
  _tick += 1;

  Game::JSON::Object  json;

  broadcast("LevelUpdate", json);

}

void  RPG::GameServer::handleFileRequest(std::size_t id, sf::Packet& packet)
{
  std::string filename;

  // Extract filename from packet
  packet >> filename;

  // Invalid packet
  if (!packet || packet.endOfPacket() == false) {
    std::cerr << "[RPG::GameServer::handleFileRequest] Warning, invalid request from client #" << id << "." << std::endl;
    kick(id);
    return;
  }

  std::ifstream file(Game::Config::ExecutablePath / "assets" / "rpg" / filename, std::iostream::binary);
  sf::Packet    toSend;

  // Valid file
  if (file) {
    std::string   data;
    std::uint8_t  byte;

    // Read file to vector
    data.reserve(1024 * 1024);
    while (file >> byte)
      data.push_back(byte);

    // Create packet to send
    toSend << std::uint32_t(RPG::RFC::ServerToClient::StC_FileSend) << std::size_t(data.size());
    for (auto byte : data)
      toSend << byte;
  }

  // Unknown file
  else
    toSend << RPG::RFC::ServerToClient::StC_FileUnknown << filename;

  // Send answer to client
  //send(id, toSend);
}

void  RPG::GameServer::handleFileInfo(std::size_t id, sf::Packet& packet)
{
  std::string filename;

  // Extract filename from packet
  packet >> filename;

  // Invalid packet
  if (!packet || packet.endOfPacket() == false) {
    std::cerr << "[RPG::GameServer::handleFileInfo] Warning, invalid request from client #" << id << "." << std::endl;
    kick(id);
    return;
  }

  std::ifstream file(Game::Config::ExecutablePath / "assets" / "rpg" / filename, std::iostream::binary);
  sf::Packet    toSend;

  // Valid file
  if (file) {
    std::string   data;
    std::uint8_t  byte;

    // Read file to string
    data.reserve(1024 * 1024);
    while (file >> byte)
      data.push_back(byte);

    // Create packet to send
    toSend << std::uint32_t(RPG::RFC::ServerToClient::StC_FileInfo) << std::size_t(data.size()) << std::size_t(std::hash<std::string>{}(data));
  }

  // Unknown file
  else
    toSend << RPG::RFC::ServerToClient::StC_FileUnknown << filename;

  // Send answer to client
  //send(id, toSend);
}

RPG::GameServer::Client::Client(std::size_t id) :
  id(id)
{}