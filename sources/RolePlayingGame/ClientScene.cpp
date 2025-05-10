#include <SFML/Network/IpAddress.hpp>

#include "RolePlayingGame/ClientScene.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/BoardComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/CellComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/EntityComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ModelComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ControllerComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ParticleComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ParticleEmitterComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/BoardSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ModelSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/EntitySystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ControllerSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ParticleSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ParticleEmitterSystem.hpp"
#include "System/Utilities.hpp"
#include "System/Window.hpp"

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
  _ecs.addComponent<RPG::BoardComponent>();
  _ecs.addComponent<RPG::CellComponent>();
  _ecs.addComponent<RPG::EntityComponent>();
  _ecs.addComponent<RPG::ModelComponent>();
  _ecs.addComponent<RPG::ControllerComponent>();
  _ecs.addComponent<RPG::ParticleComponent>();
  _ecs.addComponent<RPG::ParticleEmitterComponent>();

  RPG::ECS::Signature signature;

  // Register ECS systems
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::EntityComponent>());
  signature.set(_ecs.typeComponent<RPG::ModelComponent>());
  _ecs.addSystem<RPG::ClientEntitySystem>(signature);
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::EntityComponent>());
  signature.set(_ecs.typeComponent<RPG::ControllerComponent>());
  signature.set(_ecs.typeComponent<RPG::ModelComponent>());
  _ecs.addSystem<RPG::ClientControllerSystem>(signature);
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::ModelComponent>());
  _ecs.addSystem<RPG::ClientModelSystem>(signature);
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::BoardComponent>());
  signature.set(_ecs.typeComponent<RPG::CellComponent>());
  signature.set(_ecs.typeComponent<RPG::ModelComponent>());
  signature.set(_ecs.typeComponent<RPG::ParticleEmitterComponent>());
  _ecs.addSystem<RPG::ClientBoardSystem>(signature);
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::ParticleComponent>());
  signature.set(_ecs.typeComponent<RPG::ModelComponent>());
  _ecs.addSystem<RPG::ParticleSystem>(signature);
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::ParticleEmitterComponent>());
  _ecs.addSystem<RPG::ParticleEmitterSystem>(signature);
}

void  RPG::ClientScene::send(const std::vector<std::wstring>& type, Game::JSON::Object& json)
{
  Game::JSON::Array typeArray;

  // Serialize type
  typeArray.reserve(type.size());
  for (const auto& field : type)
    typeArray.push(field);
  json.set(L"type", std::move(typeArray));

  // Send packet to server
  _client.send(json);
}

bool  RPG::ClientScene::update(float elapsed)
{
  // Receive available packets
  updateReceive(elapsed);

  // Update ECS
  _ecs.getSystem<RPG::ClientControllerSystem>().executeUpdate(_ecs, elapsed);
  _ecs.getSystem<RPG::ClientModelSystem>().executeCamera(_ecs, elapsed);
  _ecs.getSystem<RPG::ParticleSystem>().execute(_ecs, elapsed);
  _ecs.getSystem<RPG::ParticleEmitterSystem>().execute(_ecs, elapsed);
  _ecs.getSystem<RPG::ClientModelSystem>().executeAnimation(_ecs, elapsed);
  _ecs.getSystem<RPG::ClientBoardSystem>().executeCursor(_ecs, elapsed);
  _ecs.getSystem<RPG::ClientBoardSystem>().executeCell(_ecs, elapsed);
  _ecs.getSystem<RPG::ClientEntitySystem>().executePosition(_ecs);

  // TODO
  auto cursorPixel = Game::Window::Instance().mouse().position();
  auto cursorCoords = _ecs.getSystem<RPG::ClientModelSystem>().getCamera().pixelToCoords({ (float)cursorPixel.x(), (float)cursorPixel.y() });
  auto entity = _ecs.getSystem<RPG::ClientEntitySystem>().intersect(_ecs, cursorCoords);
  auto cell = _ecs.getSystem<RPG::ClientBoardSystem>().intersect(_ecs, cursorCoords);
  auto controlled = _ecs.getSystem<RPG::ClientControllerSystem>().selected();

  if (entity != RPG::ECS::InvalidEntity)
    _ecs.getSystem<RPG::ClientBoardSystem>().setCursor(_ecs, RPG::ECS::InvalidEntity);
  else
    _ecs.getSystem<RPG::ClientBoardSystem>().setCursor(_ecs, cell);

  if (Game::Window::Instance().mouse().buttonPressed(Game::Window::MouseButton::Left) == true &&
    entity == RPG::ECS::InvalidEntity &&
    cell != RPG::ECS::InvalidEntity &&
    controlled != RPG::ECS::InvalidEntity) {
    Game::JSON::Object  json;
    
    json.set(L"id", _ecs.getComponent<RPG::EntityComponent>(controlled).id);
    json.set(L"coordinates", _ecs.getComponent<RPG::CellComponent>(cell).coordinates.json());
    send({ L"action", L"move" }, json);
  }

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
    if (json.empty() == true)
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
  auto jsonStr = json.stringify();
  std::wcout << "Client received (tick: " << (std::size_t)json.get(L"tick").number() << ", type: " << json.get(L"type").array() << ", size: " << jsonStr.length() << "): ";
  std::cout << Game::Utilities::Convert(jsonStr) << std::endl;

  const auto& type = json.get(L"type").array().get(0).string();

  // Handle request
  if (type == L"controller")
    _ecs.getSystem<RPG::ClientControllerSystem>().handlePacket(_ecs, *this, json);
  else if (type == L"model")
    _ecs.getSystem<RPG::ClientModelSystem>().handlePacket(_ecs, *this, json);
  else if (type == L"board")
    _ecs.getSystem<RPG::ClientBoardSystem>().handlePacket(_ecs, *this, json);
  else if (type == L"entity")
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
  _ecs.getSystem<RPG::ClientControllerSystem>().executeDraw(_ecs);
  _ecs.getSystem<RPG::ClientModelSystem>().executeDraw(_ecs);
}