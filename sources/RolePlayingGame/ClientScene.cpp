#include <SFML/Network/IpAddress.hpp>

#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Library/TextureLibrary.hpp"
#include "RolePlayingGame/ClientScene.hpp"

RPG::ClientScene::ClientScene(Game::SceneMachine& machine, std::unique_ptr<RPG::Server>&& server) :
  Game::AbstractScene(machine),
  _server(std::move(server)),
  _client(server->getPort(), sf::IpAddress::LocalHost.toInteger()),
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

  // Update world camera
  _camera.update(elapsed);

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
{
  auto& window = Game::Window::Instance();

  // Mouse wheel, zoom camera
  _camera.setZoomTarget(_camera.getZoomTarget() * (float)std::pow(1.125f, window.mouse().wheel()));

  // Keyboard arrows, move camera
  _camera.setPositionTarget(
    _camera.getPositionTarget()
    + Math::Vector<2>(
      (window.keyboard().keyDown(sf::Keyboard::Q) ? -1.f : 0.f) + (window.keyboard().keyDown(sf::Keyboard::D) ? +1.f : 0.f),
      (window.keyboard().keyDown(sf::Keyboard::Z) ? -1.f : 0.f) + (window.keyboard().keyDown(sf::Keyboard::S) ? +1.f : 0.f)
    ) / _camera.getZoom() * elapsed * 128.f
  );

  auto mouse = Game::Window::Instance().mouse().position();
  auto pos = _camera.pixelToCoords({ (float)mouse.x, (float)mouse.y });

  _cursor = { -1, -1 };

  RPG::Coordinates coord(0, 0);

  // Left side, Y axis
  if (pos.x() < 0)
    coord.y() = -((int)pos.x() - (int)RPG::Level::Cell::Width / 2) / RPG::Level::Cell::Width;

  // Right size, X axis
  else
    coord.x() = +((int)pos.x() + (int)RPG::Level::Cell::Width / 2) / RPG::Level::Cell::Width;

  // Find a matching item
  for (; coord.x() < _level.cellsX && coord.y() < _level.cellsY; coord += { +1, +1 }) {
    const auto& cell = _level.getCell(coord.x(), coord.y());

    // TODO: check entities on cell

    if (cell.selectable == false)
      continue;

    sf::IntRect bounds(
      (coord.x() - coord.y()) * RPG::Level::CellWidth - RPG::Level::CellWidth / 2 + ((coord.x() - coord.y()) < 0 ? 1 : 0),
      -(coord.x() + coord.y()) * RPG::Level::CellHeight / 2 - RPG::Level::CellHeight / 2 - cell.height + (coord.y() > 0 ? 1 : 0),
      RPG::Level::CellWidth,
      RPG::Level::CellHeight
    );
    
    // Cell intersection
    if (bounds.contains((int)pos.x(), (int)pos.y()) == true) {
      _cursor = coord;
      break;
    }
  }

  // Left click, select a cell
  if (_cursor != Math::Vector<2, int>(-1, -1) && window.mouse().buttonPressed(sf::Mouse::Button::Left) == true) {
    _select = _cursor;

    Game::JSON::Object  json;

    json.map["cell"] = std::make_unique<Game::JSON::Object>();
    json.map["cell"]->object().map["x"] = std::make_unique<Game::JSON::Number>((std::size_t)_select.x());
    json.map["cell"]->object().map["y"] = std::make_unique<Game::JSON::Number>((std::size_t)_select.y());

    send("characterMove", json);
  }
    
  // Right click, unselect cell
  if (window.mouse().buttonPressed(sf::Mouse::Button::Right) == true)
    _select = { -1, -1 };
}

void  RPG::ClientScene::draw()
{
  // TODO
  drawWorld();
}

void  RPG::GameScene::drawWorld()
{
  auto& window = Game::Window::Instance();
  
  // Set world camera
  _camera.set();
  
  // Background color
  window.window().clear(sf::Color(_level.color.red, _level.color.green, _level.color.blue, _level.color.alpha));

  // Draw background
  _level.background.draw(0.f, 0.f);

  int cameraX = 0;
  int cameraY = 0;
  int cameraR = 16;

  int cameraStartX = cameraR;
  int cameraStartY = cameraR;

  // Iterate cells from back to front
  for (; cameraStartX >= -cameraR && cameraStartY >= -cameraR; (cameraStartX > -cameraR) ? cameraStartX-- : cameraStartY--)
  {
    int cameraTileX = cameraStartX;
    int cameraTileY = cameraStartY;

    for (; cameraTileX <= +cameraR && cameraTileY >= -cameraR; cameraTileX++, cameraTileY--)
    {
      // Out of camera
      if (-cameraR > cameraTileX - cameraTileY || cameraTileX - cameraTileY > +cameraR)
        continue;

      int cellX = cameraX + cameraTileX;
      int cellY = cameraY + cameraTileY;
      
      const auto& cell = _level.getCell(cellX, cellY);

      // Out of level
      if (cellX < 0 || cellX >= _level.cellsX || cellY < 0 || cellY >= _level.cellsY)
        continue;

      const auto& cell = _level.getCell(cellX, cellY);

      // Compute position of the cell
      float       posX = (float)(cellX - cellY) * (float)RPG::Level::CellWidth;
      float       posY = (float)(cellX + cellY) * -(float)(RPG::Level::CellHeight / 2) - (float)cell.height;

      cell.background.draw(posX, posY);
    }
  }

  // Draw foreground
  _level.foreground.draw(0.f, 0.f);

  // Cursor
  if (_cursor != Math::Vector<2, int>(-1, -1))
  {
    const auto& texture = Game::TextureLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "rpg" / "cursor.png");
    sf::Sprite  select;

    select.setPosition((float)(_cursor.x() - _cursor.y()) * (float)RPG::Level::CellWidth, (float)-(_cursor.x() + _cursor.y()) * (float)RPG::Level::CellHeight / 2.f - (float)_level.getCell(_cursor.x(), _cursor.y()).height);
    select.setTexture(texture, true);
    select.setOrigin(16.f, 16.f);
    window.window().draw(select);
  }

  // Select
  if (_select != Math::Vector<2, int>(-1, -1))
  {
    const auto& texture = Game::TextureLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "rpg" / "select.png");
    sf::Sprite  select;

    select.setPosition((float)(_select.x() - _select.y()) * (float)RPG::Level::CellWidth, (float)-(_select.x() + _select.y()) * (float)RPG::Level::CellHeight / 2.f - (float)_level.getCell(_select.x(), _select.y()).height);
    select.setTexture(texture, true);
    select.setOrigin(16.f, 16.f);
    window.window().draw(select);
  }

  // Reset window view
  _camera.reset();
}
