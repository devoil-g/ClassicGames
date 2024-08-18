#include <SFML/Network/IpAddress.hpp>

#include "RolePlayingGame/ClientScene.hpp"
#include "RolePlayingGame/Components.hpp"
#include "RolePlayingGame/Systems.hpp"

RPG::ClientScene::ClientScene(Game::SceneMachine& machine, std::unique_ptr<RPG::Server>&& server) :
  ClientScene(machine, server->getPort(), sf::IpAddress::LocalHost.toInteger())
{
  // Client hold server instance
  _server = std::move(server);
}

RPG::ClientScene::ClientScene(Game::SceneMachine& machine, std::uint16_t port, std::uint32_t address) :
  Game::AbstractScene(machine),
  _server(),
  _client(port, address),
  _ecs()
{
  // Register ECS components
  _ecs.addComponent<RPG::CellComponent>();
  _ecs.addComponent<RPG::EntityComponent>();
  _ecs.addComponent<RPG::DisplayComponent>();
  _ecs.addComponent<RPG::NetworkComponent>();
  _ecs.addComponent<RPG::ParticleComponent>();
  _ecs.addComponent<RPG::ParticleEmitterComponent>();

  RPG::ECS::Signature signature;

  // Register ECS systems
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::EntityComponent>());
  signature.set(_ecs.typeComponent<RPG::DisplayComponent>());
  _ecs.addSystem<RPG::ClientEntitySystem>(signature);
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::EntityComponent>());
  signature.set(_ecs.typeComponent<RPG::NetworkComponent>());
  signature.set(_ecs.typeComponent<RPG::DisplayComponent>());
  _ecs.addSystem<RPG::ClientNetworkSystem>(signature);
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::CellComponent>());
  signature.set(_ecs.typeComponent<RPG::ParticleEmitterComponent>());
  _ecs.addSystem<RPG::ClientBoardSystem>(signature);
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::DisplayComponent>());
  _ecs.addSystem<RPG::ClientDisplaySystem>(signature);
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::ParticleComponent>());
  signature.set(_ecs.typeComponent<RPG::DisplayComponent>());
  _ecs.addSystem<RPG::ParticleSystem>(signature);
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::ParticleEmitterComponent>());
  _ecs.addSystem<RPG::ParticleEmitterSystem>(signature);
}

void  RPG::ClientScene::send(const std::vector<std::string>& type, Game::JSON::Object& json)
{
  Game::JSON::Array typeArray;

  // Serialize type
  typeArray._vector.reserve(type.size());
  for (const auto& field : type)
    typeArray.push(field);
  json.set("type", std::move(typeArray));

  // Send packet to server
  _client.send(json);
}

bool  RPG::ClientScene::update(float elapsed)
{
  // Receive available packets
  updateReceive(elapsed);

  _ecs.getSystem<RPG::ClientDisplaySystem>().executeCamera(_ecs, elapsed);
  _ecs.getSystem<RPG::ParticleSystem>().execute(_ecs, elapsed);
  _ecs.getSystem<RPG::ParticleEmitterSystem>().execute(_ecs, elapsed);
  _ecs.getSystem<RPG::ClientDisplaySystem>().executeAnimation(_ecs, elapsed);
  _ecs.getSystem<RPG::ClientBoardSystem>().executeCell(_ecs, elapsed);
  _ecs.getSystem<RPG::ClientEntitySystem>().executePosition(_ecs);

  // Update ECS
  // TODO

  // Send pending packets
  updateSend(elapsed);

  return false;
}

void  RPG::ClientScene::updateReceive(float elapsed)
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
      updatePacket(json, elapsed);
    }
    catch (const std::exception& exception) {
      std::cout << "[RPG::Client] Invalid request from server (" << exception.what() << ")." << std::endl;
    }
  }
}

void  RPG::ClientScene::updatePacket(const Game::JSON::Object& json, float elapsed)
{
  // TODO: remove this
  std::cout << "Client received (tick: " << (std::size_t)json.get("tick").number() << ", type: " << json.get("type").array() << ", size: " << json.stringify().length() << "): " << json << std::endl;

  const auto& type = json.get("type").array().get(0).string();

  // Handle request
  if (type == "network")
    _ecs.getSystem<RPG::ClientNetworkSystem>().handlePacket(_ecs, *this, json);
  else if (type == "display")
    _ecs.getSystem<RPG::ClientDisplaySystem>().handlePacket(_ecs, *this, json);
  else if (type == "board")
    _ecs.getSystem<RPG::ClientBoardSystem>().handlePacket(_ecs, *this, json);
  else if (type == "entity")
    _ecs.getSystem<RPG::ClientEntitySystem>().handlePacket(_ecs, *this, json);
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientScene::updateSend(float elapsed)
{
  // Send pending packets
  _client.send();
}

void  RPG::ClientScene::draw()
{
  // Draw entities
  _ecs.getSystem<RPG::ClientDisplaySystem>().executeDraw(_ecs);
}