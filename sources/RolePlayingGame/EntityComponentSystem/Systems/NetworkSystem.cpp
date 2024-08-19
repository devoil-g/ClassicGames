#include "RolePlayingGame/EntityComponentSystem/Systems/EntitySystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/NetworkSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/EntityComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/NetworkComponent.hpp"
#include "RolePlayingGame/Server.hpp"

const std::string                 RPG::NetworkSystem::Player::DefaultName("");
const RPG::NetworkSystem::Player  RPG::NetworkSystem::Player::ErrorPlayer;

RPG::NetworkSystem::Player::Player() :
  name(DefaultName)
{}

RPG::NetworkSystem::Player::Player(const Game::JSON::Object& json) :
  name(json.contains("name") ? json.get("name").string() : DefaultName)
{}

const RPG::NetworkSystem::Player& RPG::NetworkSystem::getPlayer(std::size_t controller) const
{
  auto iterator = _players.find(controller);

  // No player matching controller
  if (iterator == _players.end())
    return RPG::NetworkSystem::Player::ErrorPlayer;

  // Return matching player
  return iterator->second;
}

Game::JSON::Object  RPG::NetworkSystem::Player::json() const
{
  Game::JSON::Object  json;

  // Serialize player
  if (name != DefaultName)
    json.set("name", name);

  return json;
}

void  RPG::ServerNetworkSystem::connect(RPG::ECS& ecs, RPG::Server& server, std::size_t id)
{
  assert(id != RPG::NetworkComponent::NoController && "Invalid ID used in RPG::ServerNetworkSystem::connect.");
  assert(_players.contains(id) == false && "ID already registered in RPG::ServerNetworkSystem::connect.");

  Game::JSON::Object  messageConnect;

  // Update connect to every played
  messageConnect.set("controller", (double)id);
  server.broadcast({ "network", "connect" }, messageConnect);

  // Send every player infos to new player
  for (const auto& [controller, player] : _players) {
    Game::JSON::Object  messagePlayer = player.json();

    messagePlayer.set("controller", (double)controller);
    server.send(id, { "network", "player" }, messagePlayer);
  }

  // Add ID to players
  _players.emplace(id, Player());

  // Find an unused entity for new player
  for (auto entity : entities) {
    auto& networkComponent = ecs.getComponent<RPG::NetworkComponent>(entity);

    // Entity not used
    if (networkComponent.controller == RPG::NetworkComponent::NoController) {
      auto& entityComponent = ecs.getComponent<RPG::EntityComponent>(entity);

      // Assign entity to ID
      networkComponent.controller = id;
      
      Game::JSON::Object  messageAssign;

      // Update control status to every played
      messageAssign.set("id", entityComponent.id);
      messageAssign.set("controller", (double)id);
      server.broadcast({ "network", "assign" }, messageAssign);
      break;
    }
  }

  // TODO: remove this
  std::cerr << "[DEBUG::server] Client #" << id << " connected." << std::endl;
}

void  RPG::ServerNetworkSystem::disconnect(RPG::ECS& ecs, RPG::Server& server, std::size_t id)
{
  assert(id != RPG::NetworkComponent::NoController && "Invalid ID used in RPG::ServerNetworkSystem::disconnect.");
  assert(_players.contains(id) == true && "ID not registered in RPG::ServerNetworkSystem::disconnect.");

  // Find used entity of player
  for (auto entity : entities) {
    auto& networkComponent = ecs.getComponent<RPG::NetworkComponent>(entity);

    // Entity of player
    if (networkComponent.controller == id) {
      auto& entityComponent = ecs.getComponent<RPG::EntityComponent>(entity);

      // Remove entity from player control
      networkComponent.controller = RPG::NetworkComponent::NoController;

      Game::JSON::Object  messageAssign;

      // Update control status to every played
      messageAssign.set("id", entityComponent.id);
      messageAssign.set("controller", (double)RPG::NetworkComponent::NoController);
      server.broadcast({ "network", "assign" }, messageAssign);
      break;
    }
  }

  // Remove ID from players
  _players.erase(id);

  Game::JSON::Object  messageDisconnect;

  // Update connect to every played
  messageDisconnect.set("controller", (double)id);
  server.broadcast({ "network", "disconnect" }, messageDisconnect);

  // TODO: remove this
  std::cerr << "[DEBUG::server] Client #" << id << " disconnected." << std::endl;
}

RPG::ClientNetworkSystem::ClientNetworkSystem() :
  self(RPG::NetworkComponent::NoController)
{}

void  RPG::ClientNetworkSystem::handlePacket(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(1).string();

  // Connecting players
  if (type == "connect")
    handleConnect(ecs, client, json);

  // Disconnecting players
  else if (type == "disconnect")
    handleDisconnect(ecs, client, json);

  // Assign an entity to player
  else if (type == "assign")
    handleAssign(ecs, client, json);

  // Update player info
  else if (type == "player")
    handlePlayer(ecs, client, json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientNetworkSystem::handleConnect(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  std::size_t controller = (std::size_t)json.get("controller").number();

  // First connect is player himself
  if (self == RPG::NetworkComponent::NoController)
    self = controller;

  // Already registered player
  if (_players.contains(controller) == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Register new player
  _players.emplace(controller, Player());
}

void  RPG::ClientNetworkSystem::handleDisconnect(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  std::size_t controller = (std::size_t)json.get("controller").number();

  // Not registered player
  if (_players.contains(controller) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Remove control of entity of player
  for (auto entity : entities) {
    auto& network = ecs.getComponent<RPG::NetworkComponent>(entity);

    if (network.controller == controller)
      network.controller = RPG::NetworkComponent::NoController;
  }

  // Unregister player
  _players.erase(controller);
}

void  RPG::ClientNetworkSystem::handleAssign(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  auto entity = ecs.getSystem<ClientEntitySystem>().getEntity(ecs, json.get("id").string());
  
  // No entity with given ID
  if (entity == RPG::ECS::InvalidEntity)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Make entity controllable
  if (ecs.signatureEntity(entity).test(ecs.typeComponent<RPG::NetworkComponent>()) == false)
    ecs.addComponent<RPG::NetworkComponent>(entity);

  // Assign entity controller
  ecs.getComponent<RPG::NetworkComponent>(entity).controller = (std::size_t)json.get("controller").number();
}

void  RPG::ClientNetworkSystem::handlePlayer(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  std::size_t controller = (std::size_t)json.get("controller").number();

  // Not registered player
  if (_players.contains(controller) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Reload player info from JSON
  _players.at(controller) = json;
}