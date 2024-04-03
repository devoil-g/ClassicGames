#include <SFML/Network/IpAddress.hpp>

#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Library/TextureLibrary.hpp"
#include "RolePlayingGame/ClientScene.hpp"

RPG::ClientScene::ClientScene(Game::SceneMachine& machine, std::unique_ptr<RPG::Server>&& server) :
  Game::AbstractScene(machine),
  _server(std::move(server)),
  _client(_server->getPort(), sf::IpAddress::LocalHost.toInteger()),
  _world()
{}

RPG::ClientScene::ClientScene(Game::SceneMachine& machine, std::uint16_t port, std::uint32_t address) :
  Game::AbstractScene(machine),
  _server(),
  _client(port, address),
  _world()
{}

void  RPG::ClientScene::send(const std::string& type, Game::JSON::Object& json)
{
  // Add type field
  json.set("type", type);

  // Send packet to server
  _client.send(json);
}

bool  RPG::ClientScene::update(float elapsed)
{
  // Receive available packets
  updatePacketReceive(elapsed);

  // Keyboard and mouse inputs
  updateInputs(elapsed);

  // Send pending packets
  updatePacketSend(elapsed);

  return false;
}

void  RPG::ClientScene::updatePacketReceive(float elapsed)
{
  // Poll pending packets
  while (true)
  {
    Game::JSON::Object  json;

    try {
      json = _client.receive();
    }
    catch (...) {
      std::cout << "[RPG::Client] Invalid socket operation (received)." << std::endl;
      break;
    }

    // No more pending packets
    if (json._map.empty() == true)
      break;

    // Extract packet from client
    try {
      // TODO: remove this
      std::cout << "Client received (tick: " << (std::size_t)json.get("tick").number() << ", type: " << json.get("type").string() << "): " << json << std::endl;

      const auto& type = json.get("type").array().get(0).string();

      // Handle request
      if (type == "load")
        _world.load(json);
      else if (type == "update")
        _world.update(json);
      else
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }
    catch (...) {
      std::cout << "[RPG::Client] Invalid request from server." << std::endl;

    }
  }
}

void  RPG::ClientScene::updatePacketSend(float elapsed)
{
  // Send pending packets
  _client.send();
}

void  RPG::ClientScene::updateInputs(float elapsed)
{}

void  RPG::ClientScene::draw()
{
  // TODO
  _world.draw();
}