#include <SFML/Network/IpAddress.hpp>

#include "RolePlayingGame/ClientScene.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/Components.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/BoardComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/CellComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/EntityComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ModelComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/NetworkComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ParticleComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ParticleEmitterComponent.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/Systems.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/BoardSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/ModelSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/EntitySystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Systems/NetworkSystem.hpp"
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
  _ecs()
{
  // Register ECS components
  _ecs.addComponent<RPG::BoardComponent>();
  _ecs.addComponent<RPG::CellComponent>();
  _ecs.addComponent<RPG::EntityComponent>();
  _ecs.addComponent<RPG::ModelComponent>();
  _ecs.addComponent<RPG::NetworkComponent>();
  _ecs.addComponent<RPG::ParticleComponent>();
  _ecs.addComponent<RPG::ParticleEmitterComponent>();
  _ecs.addComponent<RPG::ClientActionComponent>();

  RPG::ECS::Signature signature;

  // Register ECS systems
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::EntityComponent>());
  signature.set(_ecs.typeComponent<RPG::ModelComponent>());
  _ecs.addSystem<RPG::ClientEntitySystem>(signature);
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::EntityComponent>());
  signature.set(_ecs.typeComponent<RPG::NetworkComponent>());
  signature.set(_ecs.typeComponent<RPG::ModelComponent>());
  _ecs.addSystem<RPG::ClientNetworkSystem>(signature, port, address);
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
  signature.reset();
  signature.set(_ecs.typeComponent<RPG::ClientActionComponent>());
  _ecs.addSystem<RPG::ClientActionSystem>(signature);
}

RPG::ClientScene::~ClientScene()
{
  // Erase action system first
  // NOTE: Actions destructors could call other systems
  _ecs.destroySystem<RPG::ClientActionSystem>();
}

bool  RPG::ClientScene::update(float elapsed)
{
  // Receive pending packets
  _ecs.getSystem<RPG::ClientNetworkSystem>().receive();

  // Update ECS
  _ecs.getSystem<RPG::ClientActionSystem>().execute(elapsed);
  _ecs.getSystem<RPG::ClientNetworkSystem>().executeUpdate(elapsed);
  _ecs.getSystem<RPG::ClientModelSystem>().executeCamera(elapsed);
  _ecs.getSystem<RPG::ParticleSystem>().execute(elapsed);
  _ecs.getSystem<RPG::ParticleEmitterSystem>().execute(elapsed);
  _ecs.getSystem<RPG::ClientModelSystem>().executeAnimation(elapsed);
  _ecs.getSystem<RPG::ClientBoardSystem>().executeCursor(elapsed);
  _ecs.getSystem<RPG::ClientBoardSystem>().executeCell(elapsed);
  _ecs.getSystem<RPG::ClientEntitySystem>().executePosition();

  // TODO
  auto cursorPixel = Game::Window::Instance().mouse().position();
  auto cursorCoords = _ecs.getSystem<RPG::ClientModelSystem>().getCamera().pixelToCoords({ (float)cursorPixel.x(), (float)cursorPixel.y() });
  auto entity = _ecs.getSystem<RPG::ClientEntitySystem>().intersect(cursorCoords);
  auto cell = _ecs.getSystem<RPG::ClientBoardSystem>().intersect(cursorCoords);
  auto controlled = _ecs.getSystem<RPG::ClientNetworkSystem>().selected();

  if (entity != RPG::ECS::InvalidEntity)
    _ecs.getSystem<RPG::ClientBoardSystem>().setCursor(RPG::ECS::InvalidEntity);
  else
    _ecs.getSystem<RPG::ClientBoardSystem>().setCursor(cell);

  if (Game::Window::Instance().mouse().buttonPressed(Game::Window::MouseButton::Left) == true &&
    entity == RPG::ECS::InvalidEntity &&
    cell != RPG::ECS::InvalidEntity &&
    controlled != RPG::ECS::InvalidEntity) {
    Game::JSON::Object  json;
    
    json.set(L"id", _ecs.getComponent<RPG::EntityComponent>(controlled).id);
    json.set(L"target", _ecs.getComponent<RPG::CellComponent>(cell).coordinates.json());
    _ecs.getSystem<RPG::ClientNetworkSystem>().send({ L"action", L"move" }, json);
    _ecs.getSystem<RPG::ClientBoardSystem>().setClick(cell);
  }

  // Send pending packets
  _ecs.getSystem<RPG::ClientNetworkSystem>().send();

  return false;
}

void  RPG::ClientScene::draw()
{
  // Draw entities
  _ecs.getSystem<RPG::ClientNetworkSystem>().executeDraw();
  _ecs.getSystem<RPG::ClientModelSystem>().executeDraw();
}