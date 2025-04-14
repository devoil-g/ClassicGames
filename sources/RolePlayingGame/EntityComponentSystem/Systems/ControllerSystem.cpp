#include "RolePlayingGame/EntityComponentSystem/Systems/ControllerSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/EntitySystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ModelSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/EntityComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ControllerComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ModelComponent.hpp"
#include "RolePlayingGame/Server.hpp"
#include "System/Window.hpp"

const std::string                   RPG::ControllerSystem::Player::DefaultName("");
const RPG::ControllerSystem::Player RPG::ControllerSystem::Player::ErrorPlayer;

RPG::ControllerSystem::Player::Player() :
  name(DefaultName)
{}

RPG::ControllerSystem::Player::Player(const Game::JSON::Object& json) :
  name(json.contains("name") ? json.get("name").string() : DefaultName)
{}

RPG::ControllerSystem::ControllerSystem(RPG::ECS& ecs)
{}

const RPG::ControllerSystem::Player& RPG::ControllerSystem::getPlayer(std::size_t controller) const
{
  auto iterator = _players.find(controller);

  // No player matching controller
  if (iterator == _players.end())
    return RPG::ControllerSystem::Player::ErrorPlayer;

  // Return matching player
  return iterator->second;
}

Game::JSON::Object  RPG::ControllerSystem::Player::json() const
{
  Game::JSON::Object  json;

  // Serialize player
  if (name != DefaultName)
    json.set("name", name);

  return json;
}

RPG::ServerControllerSystem::ServerControllerSystem(RPG::ECS& ecs) :
  RPG::ControllerSystem(ecs)
{}

void  RPG::ServerControllerSystem::connect(RPG::ECS& ecs, RPG::Server& server, std::size_t id)
{
  assert(id != RPG::ControllerComponent::NoController && "Invalid ID used in RPG::ServerControllerSystem::connect.");
  assert(_players.contains(id) == false && "ID already registered in RPG::ServerControllerSystem::connect.");

  Game::JSON::Object  messageConnect;

  // Update connect to every played
  messageConnect.set("controller", (double)id);
  server.broadcast({ "controller", "connect" }, messageConnect);

  // Send every player infos to new player
  for (const auto& [controller, player] : _players) {
    Game::JSON::Object  messagePlayer = player.json();

    messagePlayer.set("controller", (double)controller);
    server.send(id, { "controller", "player" }, messagePlayer);
  }

  // Add ID to players
  _players.emplace(id, Player());

  // Assign every unused entity to new player
  for (auto entity : entities) {
    auto& networkComponent = ecs.getComponent<RPG::ControllerComponent>(entity);

    // Entity not used
    if (networkComponent.controller == RPG::ControllerComponent::NoController) {
      auto& entityComponent = ecs.getComponent<RPG::EntityComponent>(entity);

      // Assign entity to ID
      networkComponent.controller = id;
      
      Game::JSON::Object  messageAssign;

      // Update control status to every played
      messageAssign.set("id", entityComponent.id);
      messageAssign.set("controller", (double)id);
      server.broadcast({ "controller", "assign" }, messageAssign);
    }
  }

  // TODO: remove this
  std::cerr << "[DEBUG::server] Client #" << id << " connected." << std::endl;
}

void  RPG::ServerControllerSystem::disconnect(RPG::ECS& ecs, RPG::Server& server, std::size_t id)
{
  assert(id != RPG::ControllerComponent::NoController && "Invalid ID used in RPG::ServerControllerSystem::disconnect.");
  assert(_players.contains(id) == true && "ID not registered in RPG::ServerControllerSystem::disconnect.");

  // Find used entity of player
  for (auto entity : entities) {
    auto& networkComponent = ecs.getComponent<RPG::ControllerComponent>(entity);

    // Entity of player
    if (networkComponent.controller == id) {
      auto& entityComponent = ecs.getComponent<RPG::EntityComponent>(entity);

      // Remove entity from player control
      networkComponent.controller = RPG::ControllerComponent::NoController;

      Game::JSON::Object  messageAssign;

      // Update control status to every played
      messageAssign.set("id", entityComponent.id);
      messageAssign.set("controller", (double)RPG::ControllerComponent::NoController);
      server.broadcast({ "controller", "assign" }, messageAssign);
      break;
    }
  }

  // Remove ID from players
  _players.erase(id);

  Game::JSON::Object  messageDisconnect;

  // Update connect to every played
  messageDisconnect.set("controller", (double)id);
  server.broadcast({ "controller", "disconnect" }, messageDisconnect);

  // TODO: remove this
  std::cerr << "[DEBUG::server] Client #" << id << " disconnected." << std::endl;
}

RPG::ClientControllerSystem::ClientControllerSystem(RPG::ECS& ecs) :
  RPG::ControllerSystem(ecs),
  _self(RPG::ControllerComponent::NoController),
  _controlled(RPG::ECS::InvalidEntity),
  _assigned()
{}

void  RPG::ClientControllerSystem::handlePacket(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
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

void  RPG::ClientControllerSystem::handleConnect(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  std::size_t controller = (std::size_t)json.get("controller").number();

  // First connect is player himself
  if (_self == RPG::ControllerComponent::NoController)
    _self = controller;

  // Already registered player
  if (_players.contains(controller) == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Register new player
  _players.emplace(controller, Player());
}

void  RPG::ClientControllerSystem::handleDisconnect(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  std::size_t controller = (std::size_t)json.get("controller").number();

  // Not registered player
  if (_players.contains(controller) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Remove control of entity of player
  for (auto entity : entities) {
    auto& network = ecs.getComponent<RPG::ControllerComponent>(entity);

    if (network.controller == controller)
      network.controller = RPG::ControllerComponent::NoController;
  }

  // Unregister player
  _players.erase(controller);
}

void  RPG::ClientControllerSystem::handleAssign(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  auto entity = ecs.getSystem<ClientEntitySystem>().getEntity(ecs, json.get("id").string());
  
  // No entity with given ID
  if (entity == RPG::ECS::InvalidEntity)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Make entity controllable
  if (ecs.signatureEntity(entity).test(ecs.typeComponent<RPG::ControllerComponent>()) == false)
    ecs.addComponent<RPG::ControllerComponent>(entity);

  auto controller = (std::size_t)json.get("controller").number();
  auto controllerIterator = std::find(_assigned.begin(), _assigned.end(), entity);

  // Add entity to controlled entities list
  if (controllerIterator == _assigned.end() && controller == _self)
    _assigned.push_back(entity);

  // Remove entity from controlled entities list
  if (controllerIterator != _assigned.end() && controller != _self) {
    _assigned.erase(controllerIterator);

    // Entity is currently controlled
    if (controller == _controlled)
      select(ecs, RPG::ECS::InvalidEntity);
  }

  // Assign entity controller
  ecs.getComponent<RPG::ControllerComponent>(entity).controller = controller;
}

void  RPG::ClientControllerSystem::handlePlayer(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  std::size_t controller = (std::size_t)json.get("controller").number();

  // Not registered player
  if (_players.contains(controller) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Reload player info from JSON 
  _players.at(controller) = json;
}

void  RPG::ClientControllerSystem::executeUpdate(RPG::ECS& ecs, float elapsed)
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
      select(ecs, *std::next(_assigned.begin(), index));
}

void  RPG::ClientControllerSystem::executeDraw(RPG::ECS& ecs)
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

void  RPG::ClientControllerSystem::select(RPG::ECS& ecs, RPG::ECS::Entity entity)
{
  // Not controlled entities are considered as invalid
  if (entity != RPG::ECS::InvalidEntity && std::find(_assigned.begin(), _assigned.end(), entity) == _assigned.end())
    entity = RPG::ECS::InvalidEntity;

  // Set given entity as controlled
  _controlled = entity;

  // TODO: remove this
  std::cout << "[DEBUG::ClientControllerSystem]: taking control of entity #" << entity << " (" << (entity == RPG::ECS::InvalidEntity ? "none" : ecs.getComponent<RPG::EntityComponent>(entity).id) << ")." << std::endl;
}