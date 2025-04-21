#include "RolePlayingGame/EntityComponentSystem/Systems/ModelSystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ModelComponent.hpp"
#include "System/Utilities.hpp"
#include "System/Window.hpp"

RPG::ModelSystem::ModelSystem(RPG::ECS& ecs)
{}

const RPG::Model& RPG::ModelSystem::getModel(const std::string& name)
{
  // Get model, or error model if not loaded
  return _models.emplace(name, RPG::Model::ErrorModel).first->second;

  auto iterator = _models.find(name);

  // Handle errors
  if (iterator == _models.end())
    return RPG::Model::ErrorModel;
  else
    return iterator->second;
}

RPG::ServerModelSystem::ServerModelSystem(RPG::ECS& ecs) :
  RPG::ModelSystem(ecs)
{}

void  RPG::ServerModelSystem::load(RPG::ECS& ecs, const Game::JSON::Array& models)
{
  // Load each models
  for (const auto& model : models) {
    _models.emplace(Game::Utilities::Convert(model->object().get(L"name").string()), model->object());
  }
}

Game::JSON::Array RPG::ServerModelSystem::json(RPG::ECS& ecs) const
{
  Game::JSON::Array array;

  // Serialize each model
  array.reserve(_models.size());
  for (const auto& [name, model] : _models) {
    auto json = model.json();

    json.set(L"name", Game::Utilities::Convert(name));
    array.push(std::move(json));
  }

  return array;
}

bool  RPG::ClientModelSystem::CloserEntity(RPG::ECS& ecs, RPG::ECS::Entity aEntity, RPG::ECS::Entity bEntity)
{
  const auto  aPosition = ecs.getComponent<RPG::ModelComponent>(aEntity).position;
  const auto  bPosition = ecs.getComponent<RPG::ModelComponent>(bEntity).position;

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

bool  RPG::ClientModelSystem::FartherEntity(RPG::ECS& ecs, RPG::ECS::Entity aEntity, RPG::ECS::Entity bEntity)
{
  const auto  aPosition = ecs.getComponent<RPG::ModelComponent>(aEntity).position;
  const auto  bPosition = ecs.getComponent<RPG::ModelComponent>(bEntity).position;

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

RPG::ClientModelSystem::ClientModelSystem(RPG::ECS& ecs) :
  RPG::ModelSystem(ecs),
  _camera(),
  _textures()
{
  // TODO
  // Ugly fix for texture binding of error sprites
  ((RPG::Sprite*)&RPG::Sprite::ErrorSprite)->pointer = &getTexture(RPG::Sprite::ErrorSprite.path);

  // Setup camera
  _camera.setPositionDrag(0.03125f);
  _camera.setZoomDrag(0.03125f);
}

void  RPG::ClientModelSystem::executeCamera(RPG::ECS& ecs, float elapsed)
{
  auto&           window = Game::Window::Instance();
  Math::Vector<2> size((float)window.getSize().x(), (float)window.getSize().y());
  Math::Vector<2> mouse((float)window.mouse().position().x(), (float)window.mouse().position().y());
  Math::Vector<2> offset;
  float           zoom = 1.f;

  // Camera position (ZQSD)
  offset += Math::Vector<2>(
    (window.keyboard().keyDown(Game::Window::Key::Q) ? -1.f : 0.f) + (window.keyboard().keyDown(Game::Window::Key::D) ? +1.f : 0.f),
    (window.keyboard().keyDown(Game::Window::Key::Z) ? -1.f : 0.f) + (window.keyboard().keyDown(Game::Window::Key::S) ? +1.f : 0.f)
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

  // Camera zoom
  if (zoom != 1.f)
    _camera.setZoomTarget(_camera.getZoomTarget() * zoom, mouse);

  // Update camera
  _camera.update(elapsed);
}

void  RPG::ClientModelSystem::executeAnimation(RPG::ECS& ecs, float elapsed)
{
  // Update animation
  for (auto entity : entities)
    executeAnimation(ecs, entity, elapsed);
}

void  RPG::ClientModelSystem::executeAnimation(RPG::ECS& ecs, RPG::ECS::Entity entity, float elapsed)
{
  auto& model = ecs.getComponent<RPG::ModelComponent>(entity);
  
  // Update component's actor
  model.actor.update(elapsed);
}

void  RPG::ClientModelSystem::executeDraw(RPG::ECS& ecs)
{
  std::array<RPG::ECS::Entity, RPG::ECS::MaxEntities> drawables;
  std::size_t                                         count = 0;

  // Get every drawables in an array
  for (auto entity : entities) {
    drawables[count] = entity;
    count += 1;
  }

  // Sort drawables by depth
  std::sort(drawables.begin(), drawables.begin() + count, [&ecs](auto aEntity, auto bEntity) {
    const auto& aModel = ecs.getComponent<RPG::ModelComponent>(aEntity);
    const auto& bModel = ecs.getComponent<RPG::ModelComponent>(bEntity);

    if (aModel.layer < bModel.layer)
      return true;
    if (aModel.layer > bModel.layer)
      return false;
    if (aModel.position.x() + aModel.position.y() > bModel.position.x() + bModel.position.y())
      return true;
    if (aModel.position.x() + aModel.position.y() < bModel.position.x() + bModel.position.y())
      return false;
    if (aModel.position.z() < bModel.position.z())
      return true;
    if (aModel.position.z() > bModel.position.z())
      return false;
    return aModel.position.x() - aModel.position.y() > bModel.position.x() - bModel.position.y();
    });

  // Set camera to world view
  _camera.set();

  // Draw entities
  for (int index = 0; index < count; index++)
    executeDraw(ecs, drawables[index]);

  // Reset camera to default
  _camera.reset();
}

void  RPG::ClientModelSystem::executeDraw(RPG::ECS& ecs, RPG::ECS::Entity entity)
{
  const auto&     model = ecs.getComponent<RPG::ModelComponent>(entity);
  Math::Vector<2> position = {
    (model.position.x() - model.position.y()) * +RPG::CellOffset.x(),
    (model.position.x() + model.position.y()) * -RPG::CellOffset.y() + model.position.z() * -RPG::CellOffset.z()
  };

  // Draw frame
  model.actor.sprite(model.direction).draw(position, model.color, model.outline);
}

const RPG::Texture& RPG::ClientModelSystem::getTexture(const std::string& name)
{
  // Load and get texture
  return _textures.emplace(name, name).first->second;
}

void  RPG::ClientModelSystem::setModel(RPG::ECS& ecs, RPG::ECS::Entity entity, const std::string& name)
{
  auto& model = ecs.getComponent<RPG::ModelComponent>(entity);

  // Set component's actor model
  model.actor.setModel(getModel(name));
}

void  RPG::ClientModelSystem::setAnimation(RPG::ECS& ecs, RPG::ECS::Entity entity, const std::string& name, bool loop, float speed)
{
  auto& model = ecs.getComponent<RPG::ModelComponent>(entity);

  // Set component's actor animation
  model.actor.setAnimation(name, loop, speed);
}

const RPG::Camera&  RPG::ClientModelSystem::getCamera() const
{
  // Get current camera
  return _camera;
}

bool  RPG::ClientModelSystem::intersect(RPG::ECS& ecs, RPG::ECS::Entity entity, const Math::Vector<2>& coords) const
{
  const auto& model = ecs.getComponent<RPG::ModelComponent>(entity);
  Math::Vector<2> position = {
    (model.position.x() - model.position.y()) * +RPG::CellOffset.x(),
    (model.position.x() + model.position.y()) * -RPG::CellOffset.y() + model.position.z() * -RPG::CellOffset.z()
  };

  // Get intersection of coords with sprite
  return model.actor.sprite(model.direction).bounds(position).contains(coords);
}

void  RPG::ClientModelSystem::handlePacket(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  const auto& type = json.get(L"type").array().get(1).string();

  // Load resources
  if (type == L"load")
    handleLoad(ecs, client, json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientModelSystem::handleLoad(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  const auto& type = json.get(L"type").array().get(2).string();

  // Load/reload models
  if (type == L"models")
    handleLoadModels(ecs, client, json);

  // Load a new texture
  else if (type == L"texture")
    handleLoadTexture(ecs, client, json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientModelSystem::handleLoadModels(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  // Load/update each models
  for (const auto& model : json.get(L"models").array()) {
    const auto& name = Game::Utilities::Convert(model->object().get(L"name").string());

    // Replace old model
    if (_models.contains(name) == true) {
      const RPG::Model& old = _models.at(name);

      // Replace model
      _models.at(name) = model->object();

      // Reload new model to entity
      for (auto entity : entities)
        if (ecs.getComponent<RPG::ModelComponent>(entity).actor == old)
          setModel(ecs, entity, name);
    }

    // New model
    else
      _models.emplace(name, model->object());

    // Resolve texture of new model
    _models.at(name).resolve([this](const std::string& name) { return std::ref(getTexture(name)); });
  }
}

void  RPG::ClientModelSystem::handleLoadTexture(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json)
{
  // TODO
}