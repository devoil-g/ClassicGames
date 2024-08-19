#include "System/Window.hpp"
#include "RolePlayingGame/Systems.hpp"
#include "RolePlayingGame/Components.hpp"
#include "RolePlayingGame/Server.hpp"

RPG::ECS::Entity  RPG::EntitySystem::getEntity(RPG::ECS& ecs, const std::string& id) const
{
  // Find entity matching ID
  for (auto entity : entities)
    if (ecs.getComponent<RPG::EntityComponent>(entity).id == id)
      return entity;

  // No match
  return RPG::ECS::InvalidEntity;
}

void  RPG::ServerEntitySystem::load(RPG::ECS& ecs, const Game::JSON::Array& entities)
{
  // Create game entities
  for (const auto& element : entities._vector) {
    auto entity = ecs.createEntity();

    ecs.addComponent<RPG::EntityComponent>(entity, element->object());
    ecs.addComponent<RPG::NetworkComponent>(entity);
  }
}

Game::JSON::Array RPG::ServerEntitySystem::json(RPG::ECS& ecs) const
{
  Game::JSON::Array array;

  // Serialize each model
  array._vector.reserve(entities.size());
  for (auto entity : entities)
    array.push(ecs.getComponent<RPG::EntityComponent>(entity).json());

  return array;
}

RPG::ECS::Entity  RPG::ClientEntitySystem::getEntityAtPixel(RPG::ECS& ecs, const Math::Vector<2>& pixel) const
{
  const auto&                                         displaySystem = ecs.getSystem<RPG::ClientDisplaySystem>();
  auto                                                position = displaySystem.getCamera().pixelToCoords(pixel);
  std::array<RPG::ECS::Entity, RPG::ECS::MaxEntities> interactive;
  std::size_t                                         count = 0;

  // Get every interactive entities in an array
  for (auto entity : entities) {
    // TODO: sort non interactive entity
    interactive[count] = entity;
    count += 1;
  }

  // Sort drawables by depth
  std::sort(interactive.begin(), interactive.begin() + count, [&ecs](auto aEntity, auto bEntity) {
    return RPG::ClientDisplaySystem::CloserEntity(ecs, aEntity, bEntity);
    });

  // Find first matching entity
  for (size_t index = 0; index < count; index++)
    if (displaySystem.intersect(ecs, interactive[index], position) == true)
      return interactive[index];

  // No match
  return RPG::ECS::InvalidEntity;
}

void  RPG::ClientEntitySystem::executePosition(RPG::ECS& ecs)
{
  // Update display position of each entity
  for (auto entity : entities)
    executePosition(ecs, entity);
}

void  RPG::ClientEntitySystem::executePosition(RPG::ECS& ecs, RPG::ECS::Entity entity)
{
  const auto& entityComponent = ecs.getComponent<RPG::EntityComponent>(entity);
  auto& displayComponent = ecs.getComponent<RPG::DisplayComponent>(entity);
  auto& cellSystem = ecs.getSystem<RPG::ClientBoardSystem>();
  auto cellEntity = cellSystem.getCell(entityComponent.coordinates);
  float cellHeight = 0.f;

  // Check if cell exist to take its height
  if (cellEntity != RPG::ECS::InvalidEntity)
    cellHeight = ecs.getComponent<RPG::CellComponent>(cellEntity).height;

  // Compute display position
  displayComponent.position.x() = ((+entityComponent.coordinates.x()) + (-entityComponent.coordinates.y()) + (+entityComponent.position.x())) * RPG::DisplaySystem::CellOffsetX;
  displayComponent.position.y() = ((-entityComponent.coordinates.x()) + (-entityComponent.coordinates.y()) + (+entityComponent.position.y())) * RPG::DisplaySystem::CellOffsetY;
  displayComponent.position.z() = entityComponent.position.z() + cellHeight;
  displayComponent.direction = entityComponent.direction;
}

void  RPG::ClientEntitySystem::handlePacket(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(1).string();

  // Load resources
  if (type == "load")
    handleLoad(ecs, client, json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientEntitySystem::handleLoad(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(2).string();

  // Load resources
  if (type == "entities")
    handleLoadEntities(ecs, client, json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientEntitySystem::handleLoadEntities(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  auto& displaySystem = ecs.getSystem<RPG::ClientDisplaySystem>();
  
  // Load entities
  for (const auto& element : json.get("entities").array()._vector) {
    auto entity = ecs.createEntity();

    ecs.addComponent<RPG::EntityComponent>(entity, element->object());
    ecs.addComponent<RPG::DisplayComponent>(entity);

    // Set model of entity
    displaySystem.setModel(ecs, entity, ecs.getComponent<RPG::EntityComponent>(entity).model);
  }
}

const float RPG::DisplaySystem::CellOffsetX(14.f);
const float RPG::DisplaySystem::CellOffsetY(6.f);

void  RPG::ServerDisplaySystem::load(RPG::ECS& ecs, const Game::JSON::Array& models)
{
  // Load each models
  for (const auto& model : models._vector)
    _models.emplace(model->object().get("name").string(), model->object());
}

Game::JSON::Array RPG::ServerDisplaySystem::json(RPG::ECS& ecs) const
{
  Game::JSON::Array array;

  // Serialize each model
  array._vector.reserve(_models.size());
  for (const auto& [name, model] : _models) {
    auto json = model.json();

    json.set("name", name);
    array.push(std::move(json));
  }

  return array;
}

bool  RPG::ClientDisplaySystem::CloserEntity(RPG::ECS& ecs, RPG::ECS::Entity aEntity, RPG::ECS::Entity bEntity)
{
  const auto  aPosition = ecs.getComponent<RPG::DisplayComponent>(aEntity).position;
  const auto  bPosition = ecs.getComponent<RPG::DisplayComponent>(bEntity).position;

  if (aPosition.y() > bPosition.y())
    return true;
  if (aPosition.y() < bPosition.y())
    return false;
  if (aPosition.z() > bPosition.z())
    return true;
  if (aPosition.z() < bPosition.z())
    return false;
  return aPosition.x() < bPosition.x();
}

bool  RPG::ClientDisplaySystem::FartherEntity(RPG::ECS& ecs, RPG::ECS::Entity aEntity, RPG::ECS::Entity bEntity)
{
  const auto  aPosition = ecs.getComponent<RPG::DisplayComponent>(aEntity).position;
  const auto  bPosition = ecs.getComponent<RPG::DisplayComponent>(bEntity).position;

  if (aPosition.y() < bPosition.y())
    return true;
  if (aPosition.y() > bPosition.y())
    return false;
  if (aPosition.z() < bPosition.z())
    return true;
  if (aPosition.z() > bPosition.z())
    return false;
  return aPosition.x() > bPosition.x();
}

RPG::ClientDisplaySystem::ClientDisplaySystem() :
  _camera(),
  _textures()
{
  // TODO
  // Ugly fix for texture binding of error sprites
  ((RPG::Sprite*)&RPG::Sprite::ErrorSprite)->texture = &getTexture(RPG::Sprite::ErrorSprite.path);
  for (auto& sprite : ((RPG::Frame*)&RPG::Frame::ErrorFrame)->sprites)
    sprite.texture = &getTexture(sprite.path);

  // Setup camera
  _camera.setPositionDrag(0.03125f);
  _camera.setZoomDrag(0.03125f);
}

void  RPG::ClientDisplaySystem::executeCamera(RPG::ECS& ecs, float elapsed)
{
  auto& window = Game::Window::Instance();
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
  _camera.setPositionTarget(_camera.getPositionTarget() + offset * elapsed * 96.f / _camera.getZoom());

  // Camera zoom (mouse wheel)
  _camera.setZoomTarget(_camera.getZoomTarget() * zoom);

  // Update camera
  _camera.update(elapsed);
}

void  RPG::ClientDisplaySystem::executeAnimation(RPG::ECS& ecs, float elapsed)
{
  // Update animation
  for (auto entity : entities)
    executeAnimation(ecs, entity, elapsed);
}

void  RPG::ClientDisplaySystem::executeAnimation(RPG::ECS& ecs, RPG::ECS::Entity entity, float elapsed)
{
  auto& display = ecs.getComponent<RPG::DisplayComponent>(entity);
  
  // No model or animation
  assert(display.model != nullptr && display.animation != nullptr && "No model or animation.");

  // Increment timer
  display.elapsed += elapsed;

  // Skip frames
  while (display.elapsed > display.animation->frame(display.frame).duration)
  {
    // Non-ending frame
    if (display.animation->frame(display.frame).duration == 0.f)
      break;

    // Next frame
    display.elapsed -= display.animation->frame(display.frame).duration;
    display.frame = (display.frame + 1) % display.animation->count();

    // Animation over
    if (display.loop == false && display.frame == 0)
      setAnimation(ecs, entity, RPG::Animation::IdleAnimation, true);
  }
}

void  RPG::ClientDisplaySystem::executeDraw(RPG::ECS& ecs)
{
  std::array<RPG::ECS::Entity, RPG::ECS::MaxEntities> drawables;
  std::size_t                                         count = 0;

  auto view = _camera.view();

  // Get every drawables in an array
  for (auto entity : entities) {
    
    drawables[count] = entity;
    count += 1;
  }

  // Sort drawables by depth
  std::sort(drawables.begin(), drawables.begin() + count, [&ecs](auto aEntity, auto bEntity) {
    return RPG::ClientDisplaySystem::FartherEntity(ecs, aEntity, bEntity);
    });

  // Set camera to world view
  _camera.set();

  // Draw entities
  for (int index = 0; index < count; index++)
    executeDraw(ecs, drawables[index]);

  // Reset camera to default
  _camera.reset();
}

void  RPG::ClientDisplaySystem::executeDraw(RPG::ECS& ecs, RPG::ECS::Entity entity)
{
  const auto& display = ecs.getComponent<RPG::DisplayComponent>(entity);

  // No frame
  assert(display.animation != nullptr && "No animation.");

  display.animation->frame(display.frame).sprites.front().texture->get().getNativeHandle();

  // Draw frame
  display.animation->frame(display.frame).draw(display.direction, { display.position.x(), display.position.y() - display.position.z() }, display.color, display.outline);
}

const RPG::Texture& RPG::ClientDisplaySystem::getTexture(const std::string& name)
{
  // Load and get texture
  return _textures.emplace(name, name).first->second;
}

const RPG::Model& RPG::ClientDisplaySystem::getModel(const std::string& name) const
{
  auto it = _models.find(name);

  // Model not found
  if (it == _models.end())
    return RPG::Model::ErrorModel;

  // Get model from map
  return it->second;
}

void  RPG::ClientDisplaySystem::setModel(RPG::ECS& ecs, RPG::ECS::Entity entity, const std::string& name)
{
  // Set model
  setModel(ecs, entity, getModel(name));
}

void  RPG::ClientDisplaySystem::setModel(RPG::ECS& ecs, RPG::ECS::Entity entity, const RPG::Model& model)
{
  auto& display = ecs.getComponent<RPG::DisplayComponent>(entity);

  // Already used
  if (&model == display.model)
    return;

  // Set new model
  display.model = &model;

  // Reload animation
  setAnimation(ecs, entity, RPG::Animation::DefaultAnimation, true);
}

void  RPG::ClientDisplaySystem::setAnimation(RPG::ECS& ecs, RPG::ECS::Entity entity, const std::string& name, bool loop)
{
  auto& display = ecs.getComponent<RPG::DisplayComponent>(entity);

  // Set animation
  setAnimation(ecs, entity, display.model->animation(name), loop);
}

void  RPG::ClientDisplaySystem::setAnimation(RPG::ECS& ecs, RPG::ECS::Entity entity, const RPG::Animation& animation, bool loop)
{
  auto& display = ecs.getComponent<RPG::DisplayComponent>(entity);

  // Do not restart a looped animation
  if (&animation != display.animation || display.loop == false || loop == false) {
    display.animation = &animation;
    display.elapsed = 0.f;
    display.frame = 0;
    display.loop = loop;
  }
}

void  RPG::ClientDisplaySystem::setRandomAnimation(RPG::ECS& ecs, RPG::ECS::Entity entity, bool loop)
{
  auto& display = ecs.getComponent<RPG::DisplayComponent>(entity);

  // Set random animation
  setAnimation(ecs, entity, display.model->random(), loop);
}

const RPG::Camera&  RPG::ClientDisplaySystem::getCamera() const
{
  // Get current camera
  return _camera;
}

bool  RPG::ClientDisplaySystem::intersect(RPG::ECS& ecs, RPG::ECS::Entity entity, const Math::Vector<2>& coords) const
{
  const auto& display = ecs.getComponent<RPG::DisplayComponent>(entity);

  // No frame
  assert(display.animation != nullptr && "No animation.");

  // Get intersection of coords with sprite
  return display.animation->frame(display.frame).bounds(display.direction, { display.position.x(), display.position.y() - display.position.z() }).contains(coords);
}

void  RPG::ClientDisplaySystem::handlePacket(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(1).string();

  // Load resources
  if (type == "load")
    handleLoad(ecs, client, json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientDisplaySystem::handleLoad(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(2).string();

  // Load/reload models
  if (type == "models")
    handleLoadModels(ecs, client, json);

  // Load a new texture
  else if (type == "texture")
    handleLoadTexture(ecs, client, json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientDisplaySystem::handleLoadModels(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  // Load/update each models
  for (const auto& model : json.get("models").array()._vector) {
    const auto& name = model->object().get("name").string();

    // Replace old model
    if (_models.contains(name) == true) {
      const RPG::Model* old = &_models.at(name);

      // Replace model
      _models.at(name) = model->object();

      // Reload new model to entity
      for (auto entity : entities)
        if (ecs.getComponent<RPG::DisplayComponent>(entity).model == old)
          setModel(ecs, entity, name);
    }

    // New model
    else
      _models.emplace(name, model->object());

    // Resolve texture of new model
    _models.at(name).resolve([this](const std::string& name) { return std::ref(getTexture(name)); });
  }
}

void  RPG::ClientDisplaySystem::handleLoadTexture(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  // TODO
}

RPG::ECS::Entity  RPG::BoardSystem::getCell(RPG::Coordinates coordinates) const
{
  auto it = _map.find(coordinates);

  // No cell found
  if (it == _map.end())
    return RPG::ECS::InvalidEntity;

  // Return entity ID of cell
  return it->second;
}

void  RPG::BoardSystem::registerCell(RPG::ECS& ecs, RPG::ECS::Entity entity)
{
  auto& cell = ecs.getComponent<RPG::CellComponent>(entity);

  assert(_map.contains(cell.coordinates) == false && "Cell already registered.");

  // Register cell in map
  _map.emplace(cell.coordinates, entity);
}

void  RPG::BoardSystem::unregisterCell(RPG::ECS& ecs, RPG::ECS::Entity entity)
{
  auto& cell = ecs.getComponent<RPG::CellComponent>(entity);

  assert(_map.contains(cell.coordinates) == true && "Cell not registered.");

  // Unregister cell
  _map.erase(cell.coordinates);
}

void  RPG::ServerBoardSystem::load(RPG::ECS& ecs, const Game::JSON::Array& cells)
{
  // Create each cells of the board
  for (const auto& element : cells._vector) {
    auto entity = ecs.createEntity();

    ecs.addComponent<RPG::CellComponent>(entity, element->object());
    registerCell(ecs, entity);
  }
}

Game::JSON::Array RPG::ServerBoardSystem::json(RPG::ECS& ecs) const
{
  Game::JSON::Array   array;

  // Serialize game board
  array._vector.reserve(entities.size());
  for (auto entity : entities)
    array.push(ecs.getComponent<RPG::CellComponent>(entity).json());

  return array;
}

void  RPG::ClientBoardSystem::executeCell(RPG::ECS& ecs, float elapsed)
{
  // Update each cell of the board
  for (auto entity : entities)
    executeCell(ecs, entity, elapsed);
}

void  RPG::ClientBoardSystem::executeCell(RPG::ECS& ecs, RPG::ECS::Entity entity, float elapsed)
{
  // TODO
}

void  RPG::ClientBoardSystem::handlePacket(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(1).string();

  // Load/reload board
  if (type == "load")
    handleLoad(ecs, client, json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientBoardSystem::handleLoad(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(2).string();

  // Load/reload cells
  if (type == "cells")
    handleLoadCells(ecs, client, json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientBoardSystem::handleLoadCells(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  auto& displaySystem = ecs.getSystem<RPG::ClientDisplaySystem>();

  // Create each cells of the board
  for (const auto& element : json.get("cells").array()._vector) {
    auto entity = ecs.createEntity();

    ecs.addComponent<RPG::CellComponent>(entity, element->object());
    registerCell(ecs, entity);

    // TODO: remove this
    ecs.addComponent<RPG::ParticleEmitterComponent>(entity);
    ecs.addComponent<RPG::DisplayComponent>(entity);
    
    auto& cell = ecs.getComponent<RPG::CellComponent>(entity);
    auto& emitter = ecs.getComponent<RPG::ParticleEmitterComponent>(entity);
    auto& display = ecs.getComponent<RPG::DisplayComponent>(entity);

    displaySystem.setModel(ecs, entity, "test_tile" + std::to_string((cell.coordinates.x() + cell.coordinates.y()) % 3 + 1));
    display.position = {
      ((+cell.coordinates.x()) + (-cell.coordinates.y())) * RPG::ClientDisplaySystem::CellOffsetX,
      ((-cell.coordinates.x()) + (-cell.coordinates.y())) * RPG::ClientDisplaySystem::CellOffsetY - RPG::ClientDisplaySystem::CellOffsetY,
      cell.height
    };

    emitter.duration = ((cell.coordinates.x() + cell.coordinates.y()) % 3) == 0 ? 1000.f : 0.f;
    emitter.size = { RPG::ClientDisplaySystem::CellOffsetX, RPG::ClientDisplaySystem::CellOffsetY * 2, 0.f };
    emitter.position = {
      ((+cell.coordinates.x()) + (-cell.coordinates.y())) * RPG::ClientDisplaySystem::CellOffsetX,
      ((-cell.coordinates.x()) + (-cell.coordinates.y())) * RPG::ClientDisplaySystem::CellOffsetY,
      cell.height
    };
    emitter.frequencyLow = 4.0f;
    emitter.frequencyHigh = 12.f;
    emitter.model = "error";
    emitter.particleLow.durationLife = emitter.particleHigh.durationLife = 1.f;
    emitter.particleLow.durationFadeIn = emitter.particleHigh.durationFadeIn = 0.2f;
    emitter.particleLow.durationFadeOut = emitter.particleHigh.durationFadeOut = 0.8f;
    emitter.particleLow.physicsGravity.z() = emitter.particleHigh.physicsGravity.z() = 4.f;
    emitter.particleLow.physicsSpeed.z() = emitter.particleHigh.physicsSpeed.z() = 2.f;
    emitter.particleLow.physicsDrag = emitter.particleHigh.physicsDrag = 0.1f;
    emitter.particleLow.physicsFloor = emitter.particleHigh.physicsFloor = cell.height;
    emitter.particleLow.colorStart.alpha = emitter.particleLow.colorEnd.alpha = 0.3f;
    emitter.particleHigh.colorStart.alpha = emitter.particleHigh.colorEnd.alpha = 0.5f;
  }
}

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

void RPG::ParticleSystem::execute(RPG::ECS& ecs, float elapsed)
{
  // Update each particle
  for (auto entity = entities.begin(); entity != entities.end();) {
    executeParticle(ecs, *entity, elapsed);

    const auto& particle = ecs.getComponent<RPG::ParticleComponent>(*entity);

    // Remove particle
    if (particle.elapsed >= particle.durationFadeIn + particle.durationLife + particle.durationFadeOut)
      ecs.destroyEntity(*(entity++));
    else
      entity++;
  }
}

void  RPG::ParticleSystem::executeParticle(RPG::ECS& ecs, RPG::ECS::Entity entity, float elapsed)
{
  auto& particle = ecs.getComponent<RPG::ParticleComponent>(entity);
  auto& display = ecs.getComponent<RPG::DisplayComponent>(entity);
  float duration = particle.durationFadeIn + particle.durationLife + particle.durationFadeOut;

  // Increase timer
  particle.elapsed += elapsed;

  // End particle
  if (particle.elapsed >= duration) {
    display.color.alpha = 0;
    return;
  }

  // Update physics and position
  particle.physicsSpeed += particle.physicsGravity * elapsed;
  particle.physicsSpeed *= std::pow(1.f - particle.physicsDrag, elapsed);
  display.position += particle.physicsSpeed * elapsed;
  display.position.z() = std::max(particle.physicsFloor, display.position.z());

  // Update color
  display.color.red = (particle.colorStart.red + (particle.colorEnd.red - particle.colorStart.red) * (particle.elapsed / duration));
  display.color.green = (particle.colorStart.green + (particle.colorEnd.green - particle.colorStart.green) * (particle.elapsed / duration));
  display.color.blue = (particle.colorStart.blue + (particle.colorEnd.blue - particle.colorStart.blue) * (particle.elapsed / duration));
  display.color.alpha = (particle.colorStart.alpha + (particle.colorEnd.alpha - particle.colorStart.alpha) * (particle.elapsed / duration));

  // Fade in
  if (particle.elapsed < particle.durationFadeIn)
    display.color.alpha = (display.color.alpha * (particle.elapsed / particle.durationFadeIn));

  // Fade out
  else if (particle.elapsed > particle.durationFadeIn + particle.durationFadeOut)
    display.color.alpha = (display.color.alpha * (1.f - ((particle.elapsed - (particle.durationFadeIn + particle.durationLife)) / particle.durationFadeOut)));
}

void  RPG::ParticleEmitterSystem::execute(RPG::ECS& ecs, float elapsed)
{
  // Update each particle emitter
  for (auto entity : entities)
    executeParticleEmitter(ecs, entity, elapsed);
}

void  RPG::ParticleEmitterSystem::executeParticleEmitter(RPG::ECS& ecs, RPG::ECS::Entity entity, float elapsed)
{
  auto& emitter = ecs.getComponent<RPG::ParticleEmitterComponent>(entity);
  auto& particleSystem = ecs.getSystem<RPG::ParticleSystem>();
  auto& displaySystem = ecs.getSystem<RPG::ClientDisplaySystem>();

  // Emit particles
  while (true)
  {
    // No more particle to emit
    if (elapsed < emitter.next || emitter.duration < emitter.next) {
      emitter.duration -= elapsed;
      emitter.next -= elapsed;
      break;
    }

    // Update timers
    emitter.duration -= emitter.next;
    elapsed -= emitter.next;
    emitter.next = 1.f / Math::Random(emitter.frequencyLow, emitter.frequencyHigh);

    auto particle = ecs.createEntity();

    // Generate particle
    ecs.addComponent<RPG::DisplayComponent>(particle);
    ecs.addComponent<RPG::ParticleComponent>(particle);
    
    auto& particleDisplay = ecs.getComponent<RPG::DisplayComponent>(particle);
    auto& particleComponent = ecs.getComponent<RPG::ParticleComponent>(particle);

    // Randomize initial position
    particleDisplay.position =
      emitter.position
      + RPG::Position(Math::Random(emitter.size.x()), Math::Random(emitter.size.y()), Math::Random(emitter.size.z()))
      - (emitter.size / 2.f);

    // Set particle model
    displaySystem.setModel(ecs, particle, emitter.model);
    displaySystem.setRandomAnimation(ecs, particle, true);

    // Randomize particle
    particleComponent.physicsSpeed = Math::Vector<3>(
      Math::Random(emitter.particleLow.physicsSpeed.x(), emitter.particleHigh.physicsSpeed.x()),
      Math::Random(emitter.particleLow.physicsSpeed.y(), emitter.particleHigh.physicsSpeed.y()),
      Math::Random(emitter.particleLow.physicsSpeed.z(), emitter.particleHigh.physicsSpeed.z())
    );
    particleComponent.physicsGravity = Math::Vector<3>(
      Math::Random(emitter.particleLow.physicsGravity.x(), emitter.particleHigh.physicsGravity.x()),
      Math::Random(emitter.particleLow.physicsGravity.y(), emitter.particleHigh.physicsGravity.y()),
      Math::Random(emitter.particleLow.physicsGravity.z(), emitter.particleHigh.physicsGravity.z())
    );
    particleComponent.physicsDrag = Math::Random(emitter.particleLow.physicsDrag, emitter.particleHigh.physicsDrag);
    particleComponent.physicsFloor = Math::Random(emitter.particleLow.physicsFloor, emitter.particleHigh.physicsFloor);
    particleComponent.colorStart = RPG::Color(
      Math::Random(emitter.particleLow.colorStart.red, emitter.particleHigh.colorStart.red),
      Math::Random(emitter.particleLow.colorStart.green, emitter.particleHigh.colorStart.green),
      Math::Random(emitter.particleLow.colorStart.blue, emitter.particleHigh.colorStart.blue),
      Math::Random(emitter.particleLow.colorStart.alpha, emitter.particleHigh.colorStart.alpha)
    );
    particleComponent.colorEnd = RPG::Color(
      Math::Random(emitter.particleLow.colorEnd.red, emitter.particleHigh.colorEnd.red),
      Math::Random(emitter.particleLow.colorEnd.green, emitter.particleHigh.colorEnd.green),
      Math::Random(emitter.particleLow.colorEnd.blue, emitter.particleHigh.colorEnd.blue),
      Math::Random(emitter.particleLow.colorEnd.alpha, emitter.particleHigh.colorEnd.alpha)
    );
    particleComponent.durationFadeIn = Math::Random(emitter.particleLow.durationFadeIn, emitter.particleHigh.durationFadeIn);
    particleComponent.durationLife = Math::Random(emitter.particleLow.durationLife, emitter.particleHigh.durationLife);
    particleComponent.durationFadeOut = Math::Random(emitter.particleLow.durationFadeOut, emitter.particleHigh.durationFadeOut);

    // Update particle timer
    displaySystem.executeAnimation(ecs, particle, elapsed);
    particleSystem.executeParticle(ecs, particle, elapsed);
  }
}

/*
void  RPG::MovingSystem::setMove(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, RPG::ECS::Entity entity, Math::Vector<3, float> position, float duration)
{
  auto& move = ecs.getComponent<RPG::MoveComponent>(entity);

  // Register move
  move.position = position;
  move.remaining = duration;

  auto& animationSystem = ecs.getSystem<RPG::AnimatingSystem>();

  // Start run animation
  if (animationSystem.entities.contains(entity) == true)
    animationSystem.setAnimation(ecs, world, level, entity, RPG::Animation::RunAnimation, true);
}

void  RPG::MovingSystem::execute(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, float elapsed)
{
  // Update every entity
  for (auto entity : entities) {
    auto& move = ecs.getComponent<RPG::MoveComponent>(entity);

    // No move
    if (move.remaining <= 0.f)
      continue;

    auto& position = ecs.getComponent<RPG::PositionComponent>(entity);
    float progress = std::min(elapsed / move.remaining, 1.f);

    // Update entity position
    move.remaining *= 1.f - progress;
    position.position = move.position + ((position.position - move.position) * (1.f - progress));

    // End of move
    if (move.remaining <= 0.f) {
      auto& animationSystem = ecs.getSystem<RPG::AnimatingSystem>();

      // Start ilde animation
      if (animationSystem.entities.contains(entity) == true)
        animationSystem.setAnimation(ecs, world, level, entity, RPG::Animation::IdleAnimation, true);
    }
  }
}
*/