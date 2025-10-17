#include <iostream>

#include <SFML/Network/IpAddress.hpp>

#include "RolePlayingGame/EntityComponentSystem/Components/CellComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ModelComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/EntityComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/NetworkComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/Components.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/BoardSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ModelSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/EntitySystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/NetworkSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/Systems.hpp"
#include "RolePlayingGame/Server.hpp"
#include "System/Config.hpp"
#include "System/Utilities.hpp"

RPG::Server::Server(const std::filesystem::path& config, std::uint16_t port, std::uint32_t address) :
  _ecs()
{
  // Register ECS components
  _ecs.addComponent<RPG::CellComponent>();
  _ecs.addComponent<RPG::EntityComponent>();
  _ecs.addComponent<RPG::ModelComponent>(); // NOTE: not really used in server, only for model library
  _ecs.addComponent<RPG::NetworkComponent>();
  _ecs.addComponent<RPG::ServerActionComponent>();

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
  signature.set(_ecs.typeComponent<RPG::NetworkComponent>());
  _ecs.addSystem<RPG::ServerNetworkSystem>(signature, port, address);
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::ModelComponent>());
  _ecs.addSystem<RPG::ServerModelSystem>(signature);
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::ServerActionComponent>());
  _ecs.addSystem<RPG::ServerActionSystem>(signature);

  // Load level
  Game::JSON::Object json(config);

  // Load models
  _ecs.getSystem<RPG::ServerModelSystem>().load(json.get(L"models").array());

  // Create board cells
  _ecs.getSystem<RPG::ServerBoardSystem>().load(json.get(L"cells").array());

  // Create game entities
  _ecs.getSystem<RPG::ServerEntitySystem>().load(json.get(L"entities").array());

  // TODO: remove this
  sf::IpAddress net_address(0);

  try {
    net_address = sf::IpAddress::getPublicAddress().value_or(sf::IpAddress(0));
  }
  catch (const std::exception&) {}
  std::cout << "[RPG::Server] Server started: " << net_address << ":" << _ecs.getSystem<RPG::ServerNetworkSystem>().getPort() << "." << std::endl;

  // Run server
  _ecs.getSystem<RPG::ServerNetworkSystem>().run();
}

RPG::Server::~Server()
{
  // Manually stop server
  _ecs.getSystem<RPG::ServerNetworkSystem>().stop();
  _ecs.getSystem<RPG::ServerNetworkSystem>().wait();

  // TODO: save

  // TODO: remove this
  std::cout << "[RPG::Server] Server stopped." << std::endl;
}

std::uint16_t RPG::Server::getPort()
{
  // Get port of server
  return _ecs.getSystem<RPG::ServerNetworkSystem>().getPort();
}

std::uint32_t RPG::Server::getAddress()
{
  // Get address of server
  return _ecs.getSystem<RPG::ServerNetworkSystem>().getAddress();
}