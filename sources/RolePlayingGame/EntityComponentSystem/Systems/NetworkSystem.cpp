#include <algorithm>

#include "RolePlayingGame/EntityComponentSystem/Systems/Systems.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/BoardSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/NetworkSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/EntitySystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ModelSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/EntityComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/NetworkComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ModelComponent.hpp"
#include "RolePlayingGame/Server.hpp"
#include "System/Utilities.hpp"
#include "System/Window.hpp"

const std::wstring                RPG::NetworkSystem::Player::DefaultName(L"");
const RPG::NetworkSystem::Player  RPG::NetworkSystem::Player::ErrorPlayer;

RPG::NetworkSystem::Player::Player() :
  name(DefaultName)
{}

RPG::NetworkSystem::Player::Player(const Game::JSON::Object& json) :
  name(json.contains(L"name") ? json.get(L"name").string() : DefaultName)
{}

RPG::NetworkSystem::NetworkSystem(RPG::ECS& ecs) :
  RPG::ECS::System(ecs),
  _players()
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

std::size_t RPG::NetworkSystem::getController(RPG::ECS::Entity entity) const
{
  // Entity not registered
  if (entities.contains(entity) == false)
    return RPG::NetworkComponent::NoController;

  // Get entity controller
  return ecs.getComponent<RPG::NetworkComponent>(entity).controller;
}

Game::JSON::Object  RPG::NetworkSystem::Player::json() const
{
  Game::JSON::Object  json;

  // Serialize player
  if (name != DefaultName)
    json.set(L"name", name);

  return json;
}

RPG::ServerNetworkSystem::ServerNetworkSystem(RPG::ECS& ecs, std::uint16_t port, std::uint32_t address) :
  RPG::NetworkSystem(ecs),
  RPG::TcpServer(port, address),
  _tick(0)
{}

void  RPG::ServerNetworkSystem::onConnect(std::size_t id)
{
  assert(id != RPG::NetworkComponent::NoController && "Invalid ID used in RPG::ServerNetworkSystem::onConnect.");
  assert(_players.contains(id) == false && "ID already registered in RPG::ServerNetworkSystem::onConnect.");

  Game::JSON::Object  json;

  // Send models
  json.clear();
  json.set(L"models", ecs.getSystem<RPG::ServerModelSystem>().json());
  send(id, { L"model", L"load", L"models" }, json);

  // Send cells
  json.clear();
  json.set(L"cells", ecs.getSystem<RPG::ServerBoardSystem>().json());
  send(id, { L"board", L"load", L"cells" }, json);

  // Send entities
  json.clear();
  json.set(L"entities", ecs.getSystem<RPG::ServerEntitySystem>().json());
  send(id, { L"entity", L"load", L"entities" }, json);

  Game::JSON::Object  messageConnect;

  // Update connect to every played
  messageConnect.set(L"controller", (double)id);
  broadcast({ L"network", L"connect" }, messageConnect);

  // Send every player infos to new player
  for (const auto& [controller, player] : _players) {
    Game::JSON::Object  messagePlayer = player.json();

    messagePlayer.set(L"controller", (double)controller);
    send(id, { L"network", L"player" }, messagePlayer);
  }

  // Add ID to players
  _players.emplace(id, Player());

  // Assign every unused entity to new player
  for (auto entity : entities) {
    auto& networkComponent = ecs.getComponent<RPG::NetworkComponent>(entity);

    // Entity not used
    if (networkComponent.controller == RPG::NetworkComponent::NoController) {
      auto& entityComponent = ecs.getComponent<RPG::EntityComponent>(entity);

      // Assign entity to ID
      networkComponent.controller = id;

      Game::JSON::Object  messageAssign;

      // Update control status to every played
      messageAssign.set(L"id", entityComponent.id);
      messageAssign.set(L"controller", (double)id);
      broadcast({ L"network", L"assign" }, messageAssign);
    }
  }

  // TODO: remove this
  std::cerr << "[DEBUG::Server] Client #" << id << " connected." << std::endl;
}

void  RPG::ServerNetworkSystem::onDisconnect(std::size_t id)
{
  assert(id != RPG::NetworkComponent::NoController && "Invalid ID used in RPG::ServerNetworkSystem::onDisconnect.");
  assert(_players.contains(id) == true && "ID not registered in RPG::ServerNetworkSystem::onDisconnect.");

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
      messageAssign.set(L"id", entityComponent.id);
      messageAssign.set(L"controller", (double)RPG::NetworkComponent::NoController);
      broadcast({ L"network", L"assign" }, messageAssign);
      break;
    }
  }

  // Remove ID from players
  _players.erase(id);

  Game::JSON::Object  messageDisconnect;

  // Update connect to every played
  messageDisconnect.set(L"controller", (double)id);
  broadcast({ L"network", L"disconnect" }, messageDisconnect);

  // TODO: remove this
  std::cerr << "[DEBUG::Server] Client #" << id << " disconnected." << std::endl;
}

void  RPG::ServerNetworkSystem::onReceive(std::size_t id, const Game::JSON::Object& json)
{
  // TODO
  std::wcout << "[DEBUG::Server] Received (id: " << id << ", type: " << json.get(L"type").array() << ", size: " << json.stringify().size() << "): " << json << std::endl;

  const auto& type = json.get(L"type").array().get(0).string();

  // Player action
  if (type == L"action")
    ecs.getSystem<RPG::ServerActionSystem>().handlePacket(id, json);

  // Invalid action
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ServerNetworkSystem::onTick()
{
  // Execute actions
  ecs.getSystem<RPG::ServerActionSystem>().execute(1.f / getTickrate());
}

void  RPG::ServerNetworkSystem::header(Game::JSON::Object& json, const std::vector<std::wstring>& type) const
{
  Game::JSON::Array typeArray;

  // Serialize type
  typeArray.reserve(type.size());
  for (const auto& field : type)
    typeArray.push(field);
  json.set(L"type", std::move(typeArray));

  // Serialize tick
  json.set(L"tick", (double)_tick);
}

void  RPG::ServerNetworkSystem::send(std::size_t id, const std::vector<std::wstring>& type, Game::JSON::Object& json)
{
  // Add mandatory data to JSON
  header(json, type);

  // Send to client
  RPG::TcpServer::send(id, json);
}

void  RPG::ServerNetworkSystem::broadcast(const std::vector<std::wstring>& type, Game::JSON::Object& json)
{
  // Add mandatory data to JSON
  header(json, type);

  // Broadcast to clients
  RPG::TcpServer::broadcast(json);
}

void  RPG::ServerNetworkSystem::kick(std::size_t id)
{
  // Remove client
  RPG::TcpServer::kick(id);
}

RPG::ClientNetworkSystem::ClientNetworkSystem(RPG::ECS& ecs, std::uint16_t port, std::uint32_t address) :
  RPG::NetworkSystem(ecs),
  RPG::TcpClient(port, address),
  _self(RPG::NetworkComponent::NoController),
  _controlled(RPG::ECS::InvalidEntity),
  _assigned()
{}

void  RPG::ClientNetworkSystem::receive()
{
  // Poll pending packets
  while (true)
  {
    Game::JSON::Object  json;

    try {
      json = RPG::TcpClient::receive();
    }
    catch (...) {
      std::cout << "[RPG::Client] Invalid socket operation (received)." << std::endl;
      break;
    }

    // No more pending packets
    if (json.empty() == true)
      break;

    // Extract packet from client
    try {
      // TODO: remove this
      auto jsonStr = json.stringify();
      std::wcout << "Client received (tick: " << (std::size_t)json.get(L"tick").number() << ", type: " << json.get(L"type").array() << ", size: " << jsonStr.length() << "): ";
      std::cout << Game::Utilities::Convert(jsonStr) << std::endl;

      const auto& type = json.get(L"type").array().get(0).string();

      if (type == L"network")
        handlePacket(json);
      else if (type == L"model")
        ecs.getSystem<RPG::ClientModelSystem>().handlePacket(json);
      else if (type == L"board")
        ecs.getSystem<RPG::ClientBoardSystem>().handlePacket(json);
      else if (type == L"entity")
        ecs.getSystem<RPG::ClientEntitySystem>().handlePacket(json);
      else if (type == L"action")
        ecs.getSystem<RPG::ClientActionSystem>().handlePacket(json);
      else
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }
    catch (const std::exception& exception) {
      std::cout << "[RPG::Client] Invalid request from server (" << exception.what() << ")." << std::endl;
    }
  } 
}

void  RPG::ClientNetworkSystem::send(const std::vector<std::wstring>& type, Game::JSON::Object& json)
{
  Game::JSON::Array typeArray;

  // Serialize type
  typeArray.reserve(type.size());
  for (const auto& field : type)
    typeArray.push(field);
  json.set(L"type", std::move(typeArray));

  // Send pending packets
  RPG::TcpClient::send(json);
}

void  RPG::ClientNetworkSystem::send()
{
  // Send pending packets
  RPG::TcpClient::send();
}

void  RPG::ClientNetworkSystem::handlePacket(const Game::JSON::Object& json)
{
  const auto& type = json.get(L"type").array().get(1).string();

  // Connecting players
  if (type == L"connect")
    handleConnect(json);

  // Disconnecting players
  else if (type == L"disconnect")
    handleDisconnect(json);

  // Assign an entity to player
  else if (type == L"assign")
    handleAssign(json);

  // Update player info
  else if (type == L"player")
    handlePlayer(json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientNetworkSystem::handleConnect(const Game::JSON::Object& json)
{
  std::size_t controller = (std::size_t)json.get(L"controller").number();

  // First connect is player himself
  if (_self == RPG::NetworkComponent::NoController)
    _self = controller;

  // Already registered player
  if (_players.contains(controller) == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Register new player
  _players.emplace(controller, Player());
}

void  RPG::ClientNetworkSystem::handleDisconnect(const Game::JSON::Object& json)
{
  std::size_t controller = (std::size_t)json.get(L"controller").number();

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

void  RPG::ClientNetworkSystem::handleAssign(const Game::JSON::Object& json)
{
  auto entity = ecs.getSystem<ClientEntitySystem>().getEntity(json.get(L"id").string());
  
  // No entity with given ID
  if (entity == RPG::ECS::InvalidEntity)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Make entity controllable
  if (ecs.signatureEntity(entity).test(ecs.typeComponent<RPG::NetworkComponent>()) == false)
    ecs.addComponent<RPG::NetworkComponent>(entity);

  auto controller = (std::size_t)json.get(L"controller").number();
  auto controllerIterator = std::find(_assigned.begin(), _assigned.end(), entity);

  // Add entity to controlled entities list
  if (controllerIterator == _assigned.end() && controller == _self)
    _assigned.push_back(entity);

  // Remove entity from controlled entities list
  if (controllerIterator != _assigned.end() && controller != _self) {
    _assigned.erase(controllerIterator);

    // Entity is currently controlled
    if (controller == _controlled)
      select(RPG::ECS::InvalidEntity);
  }

  // Assign entity controller
  ecs.getComponent<RPG::NetworkComponent>(entity).controller = controller;
}

void  RPG::ClientNetworkSystem::handlePlayer(const Game::JSON::Object& json)
{
  std::size_t controller = (std::size_t)json.get(L"controller").number();

  // Not registered player
  if (_players.contains(controller) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Reload player info from JSON 
  _players.at(controller) = json;
}

void  RPG::ClientNetworkSystem::executeUpdate(float elapsed)
{
  Game::Window& window = Game::Window::Instance();

  // Key binding to controlled entity
  constexpr std::array<Game::Window::Key, 12> shortcuts = {
    Game::Window::Key::F1,
    Game::Window::Key::F2,
    Game::Window::Key::F3,
    Game::Window::Key::F4,
    Game::Window::Key::F5,
    Game::Window::Key::F6,
    Game::Window::Key::F7,
    Game::Window::Key::F8,
    Game::Window::Key::F9,
    Game::Window::Key::F10,
    Game::Window::Key::F11,
    Game::Window::Key::F12,
  };

  // Select entity with shortcut keys
  for (auto index = 0; index < shortcuts.size() && index < _assigned.size(); index++)
    if (window.keyboard().keyPressed(shortcuts[index]) == true)
      select(*std::next(_assigned.begin(), index));
}

void  RPG::ClientNetworkSystem::executeDraw()
{
  for (auto entity : entities)
  {
    auto& model = ecs.getComponent<RPG::ModelComponent>(entity);

    // Reset outline
    model.outline = RPG::Color::Transparent;

    // Assigned characters have black outline
    if (std::find(_assigned.begin(), _assigned.end(), entity) != _assigned.end()) {
      model.outline = RPG::Color::Black;

      // Controller entity have white outline
      if (entity == _controlled)
        model.outline = RPG::Color::White;
    }
  }
}

void  RPG::ClientNetworkSystem::select(RPG::ECS::Entity entity)
{
  // Not controlled entities are considered as invalid
  if (entity != RPG::ECS::InvalidEntity && std::find(_assigned.begin(), _assigned.end(), entity) == _assigned.end())
    entity = RPG::ECS::InvalidEntity;

  // Set given entity as controlled
  _controlled = entity;

  // TODO: remove this
  std::wcout << "[DEBUG::ClientControllerSystem]: taking control of entity #" << entity << " (" << (entity == RPG::ECS::InvalidEntity ? L"none" : ecs.getComponent<RPG::EntityComponent>(entity).id) << ")." << std::endl;
}

RPG::ECS::Entity  RPG::ClientNetworkSystem::selected() const
{
  // Return controlled entity
  return _controlled;
}

std::list<RPG::ECS::Entity> RPG::ClientNetworkSystem::assigned() const
{
  // Get full list of assigned entities
  return _assigned;
}

bool  RPG::ClientNetworkSystem::assigned(RPG::ECS::Entity entity) const
{
  // Check that the given entity is in assigned list
  return entity != RPG::ECS::InvalidEntity && std::find(_assigned.begin(), _assigned.end(), entity) != _assigned.end();
}