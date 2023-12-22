#include <array>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>

#include <SFML/Network/Packet.hpp>

#include "RolePlayingGame/GameServer.hpp"
#include "System/Config.hpp"

void  RPG::GameServer::onConnect(std::size_t id)
{
  _clients.emplace_back(id);
}

void  RPG::GameServer::onDisconnect(std::size_t id)
{
  _clients.remove_if([id](const auto& client) { return client.id == id; });
}

void  RPG::GameServer::onReceive(std::size_t id, sf::Packet& packet)
{
  std::string data;
  
  // Extract request type
  packet >> data;
  
  // Remove client in case of invalid request
  if (!packet) {
    std::cerr << "[RPG::GameServer::onReceive] Warning, invalid packet from client #" << id << "." << std::endl;
    kick(id);
  }

  Game::JSON::Object json = Game::JSON::load(data);

  // TODO: here
}

void  RPG::GameServer::onTick()
{

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
  send(id, toSend);
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
  send(id, toSend);
}

RPG::GameServer::Client::Client(std::size_t id) :
  id(id)
{}