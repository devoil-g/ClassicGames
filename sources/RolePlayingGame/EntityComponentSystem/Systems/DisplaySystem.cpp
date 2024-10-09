#include "RolePlayingGame/EntityComponentSystem/Systems/DisplaySystem.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/DisplayComponent.hpp"
#include "System/Window.hpp"

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

  float oldZoom = _camera.getZoomTarget();

  // Camera zoom
  _camera.setZoomTarget(_camera.getZoomTarget() * zoom);

  // Camera zoom movement to keep same object under the mouse
  _camera.setPositionTarget(_camera.getPositionTarget() + (_camera.getPositionTarget() - _camera.pixelToCoordsTarget(mouse)) * (1.f - _camera.getZoomTarget() / oldZoom));
  
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