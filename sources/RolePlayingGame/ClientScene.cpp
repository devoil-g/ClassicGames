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

  // Update world
  _world.update(*this, elapsed);

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
      std::cout << "Client received (tick: " << (std::size_t)json.get("tick").number() << ", type: " << json.get("type").array() << ", size: " << json.stringify().length() << "): " << json << std::endl;

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
{
  // Camera controls
  updateInputsCamera(elapsed);

  // Cursor on level
  updateInputsLevel(elapsed);
}

void  RPG::ClientScene::updateInputsCamera(float elapsed)
{
  // No level
  if (_world.level == nullptr)
    return;

  auto&           window = Game::Window::Instance();
  Math::Vector<2> size((float)window.window().getSize().x, (float)window.window().getSize().y);
  Math::Vector<2> mouse((float)window.mouse().position().x, (float)window.mouse().position().y);
  Math::Vector<2> offset;
  float           zoom = 1.f;

  // Camera position (ZQSD)
  offset += Math::Vector<2>(
    (window.keyboard().keyDown(sf::Keyboard::Q) ? -1.f : 0.f) + (window.keyboard().keyDown(sf::Keyboard::D) ? +1.f : 0.f),
    (window.keyboard().keyDown(sf::Keyboard::Z) ? -1.f : 0.f) + (window.keyboard().keyDown(sf::Keyboard::S) ? +1.f : 0.f)
  );

  // Mouse inside window
  if (mouse.x() >= 0.f && mouse.x() < size.x() && mouse.y() >= 0.f && mouse.y() < size.y())
  {
    // Camera position (cursor on screen border)
    offset += Math::Vector<2>(
      (mouse.x() < 16.f ? -1.f : 0.f) + (mouse.x() >= size.x() - 16.f ? +1.f : 0.f),
      (mouse.y() < 16.f ? -1.f : 0.f) + (mouse.y() >= size.y() - 16.f ? +1.f : 0.f)
    );

    // Limit move to -1,+1
    offset.x() = std::clamp(offset.x(), -1.f, +1.f);
    offset.y() = std::clamp(offset.y(), -1.f, +1.f);

    // Camera zoom (mouse wheel)
    zoom = (float)std::pow(1.28f, window.mouse().wheel());
  }

  // Camera move
  _world.level->camera.setPositionTarget(_world.level->camera.getPositionTarget() + offset * elapsed * 96.f / _world.level->camera.getZoom());

  // Camera zoom (mouse wheel)
  _world.level->camera.setZoomTarget(_world.level->camera.getZoomTarget() * zoom);
}

void  RPG::ClientScene::updateInputsLevel(float elapsed)
{
  // No level
  if (_world.level == nullptr)
    return;
  
  auto&           window = Game::Window::Instance();
  Math::Vector<2> size((float)window.window().getSize().x, (float)window.window().getSize().y);
  Math::Vector<2> mouseScreen((float)window.mouse().position().x, (float)window.mouse().position().y);
  Math::Vector<2> mouseWorld(_world.level->camera.pixelToCoords(mouseScreen));
  bool            show = window.keyboard().keyDown(sf::Keyboard::LAlt);

  RPG::ClientEntity* selectedEntity = nullptr;
  RPG::ClientCell*   selectedCell = nullptr;

  // Entity outline color
  for (auto& entity : _world.level->entities)
  {
    // Reset outline color
    entity.outline = show == true ? RPG::Color(0, 0, 0, 255) : RPG::Color(0, 0, 0, 0);

    // Selectable entity
    if (selectedEntity == nullptr && entity.isTrigger() == true && entity.bounds(_world, *_world.level).contains(mouseWorld) == true) {
      entity.outline = RPG::Color(255, 255, 255, 255);
      selectedEntity = &entity;
    }
  }

  // Cell cursor
  for (auto& cell : _world.level->cells)
  {
    // Reset color
    cell.hover = RPG::Color(0);
    cell.cursor = RPG::Color(0);

    // Selectable cell
    if (selectedEntity == nullptr && selectedCell == nullptr && cell.blocked == false && cell.bounds(_world, *_world.level).contains(mouseWorld) == true) {
      cell.hover = RPG::Color(255, 255, 255, 255);
      cell.cursor = RPG::Color(0);
    }
  }

  // Mouse outside window
  if (mouseScreen.x() < 0 || mouseScreen.x() >= size.x() || mouseScreen.y() < 0 || mouseScreen.y() >= size.y())
    return;

}

void  RPG::ClientScene::draw()
{
  // TODO
  _world.draw();
}