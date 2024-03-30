#include "RolePlayingGame/Level.hpp"
#include "RolePlayingGame/World.hpp"
#include "RolePlayingGame/Entity.hpp"

RPG::ClientEntity::ClientEntity(const RPG::ClientWorld& world, const RPG::ClientLevel& level, const Game::JSON::Object& json) :
  id((std::uint64_t)json.get("id").number()),
  coordinates(json.get("coordinates").array()),
  position(json.contains("position") ? json.get("position").array() : RPG::Entity::DefaultPosition),
  direction(json.contains("direction") ? (RPG::Direction)json.get("direction").number() : RPG::Entity::DefaultDirection),
  _move{
    .target = coordinates,
    .position = position,
    .remaining = 0.f,
    .speed = 0.f
  },
  _animation{
    .model = nullptr,
    .animation = nullptr,
    .texture = nullptr,
    .frame = 0,
    .elapsed = 0.f,
    .loop = false
  },
  _actions{
    .pickup = json.contains("pickup") ? json.get("pickup").boolean() : false,
    .container = json.contains("container") ? json.get("container").boolean() : false,
    .dialog = json.contains("dialog") ? json.get("dialog").boolean() : false,
    .fight = json.contains("fight") ? json.get("fight").boolean() : false,
    .trigger = json.contains("trigger") ? json.get("trigger").boolean() : false
  }
{
  // Load model
  if (json.contains("model") == true)
    setModel(world, level, json.get("model").string());

  // Start default animation
  setAnimation(world, level, RPG::Model::DefaultAnimation, true);
}

void  RPG::ClientEntity::setModel(const RPG::ClientWorld& world, const RPG::ClientLevel& level, const std::string& name)
{
  // Invalid model name
  if (name.empty() == true)
    return;

  // Reset model
  _animation.model = nullptr;
  _animation.animation = nullptr;
  _animation.texture = nullptr;
  _animation.frame = 0;
  _animation.elapsed = 0.f;
  _animation.loop = false;

  const RPG::Model* model = nullptr;
  const RPG::Model::Animation* animation = nullptr;
  const RPG::Texture* texture = nullptr;

  // Get resources
  try {
    model = &world.model(name);
    animation = &model->animation(RPG::Model::DefaultAnimation);
    texture = &world.texture(model->spritesheet);
  }
  catch (const std::exception&) {
    return;
  }

  // Assign resources
  _animation.model = model;
  _animation.animation = animation;
  _animation.texture = texture;

  // Start default animation
  setAnimation(world, level, RPG::Model::DefaultAnimation, true);
}

void  RPG::ClientEntity::setAnimation(const RPG::ClientWorld& world, const RPG::ClientLevel& level, const std::string& name, bool loop)
{
  // No model
  if (_animation.model == nullptr)
    return;

  const RPG::Model::Animation* animation = nullptr;

  // Get animation
  try {
    // Oriented animation
    animation = &_animation.model->animation(name + "_" + RPG::DirectionNames[direction]);
  }
  catch (const std::exception&) {
    try {
      // Non-oriented animation
      animation = &_animation.model->animation(name);
    }
    catch (const std::exception&) {
      // Try default animation
      if (name != RPG::Model::DefaultAnimation)
        setAnimation(world, level, RPG::Model::DefaultAnimation, true);
      return;
    }
  }

  // Start new animation
  _animation.animation = animation;
  _animation.loop = loop;
  _animation.frame = 0;
  _animation.elapsed = 0.f;
}

void  RPG::ClientEntity::update(const RPG::ClientWorld& world, const RPG::ClientLevel& level, float elapsed)
{
  // Update animation
  updateAnimation(world, level, elapsed);

  // Update move
  updateMove(world, level, elapsed);
}

void  RPG::ClientEntity::updateAnimation(const RPG::ClientWorld& world, const RPG::ClientLevel& level, float elapsed)
{
  // No model or animation
  if (_animation.model == nullptr || _animation.animation == nullptr)
    return;

  // Increment timer
  _animation.elapsed += elapsed;

  // Skip frames
  while (_animation.elapsed > _animation.animation->frames[_animation.frame].duration)
  {
    // Next frame
    _animation.elapsed -= _animation.animation->frames[_animation.frame].duration;
    _animation.frame = (_animation.frame + 1) % _animation.animation->frames.size();

    // Animation over
    if (_animation.loop == false && _animation.frame == 0) {
      setAnimation(world, level, RPG::Model::IdleAnimation, true);
      return;
    }
  }
}

void  RPG::ClientEntity::updateMove(const RPG::ClientWorld& world, const RPG::ClientLevel& level, float elapsed)
{
  // No move
  if (_move.remaining == 0.f)
    return;

  auto oldRemaining = _move.remaining;
  auto newRemainging = std::max(0.f, _move.remaining - _move.speed * elapsed);
  
  // Update position
  position = _move.position + (position - _move.position) * (newRemainging / oldRemaining);
  _move.remaining = newRemainging;

  // Move completed
  if (newRemainging == 0.f)
    setAnimation(world, level, RPG::Model::IdleAnimation, true);
}

void  RPG::ClientEntity::draw(const RPG::ClientWorld& world, const RPG::ClientLevel& level) const
{
  // No model or animation or texture
  if (_animation.model == nullptr || _animation.animation == nullptr || _animation.texture == nullptr)
    return;
  
  // Draw entity
  _animation.animation->frames[_animation.frame].draw(*_animation.texture, { position.x(), position.y() - position.z() });
}

void  RPG::ClientEntity::update(const RPG::ClientWorld& world, const RPG::ClientLevel& level, const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(4).string();

  // Move entity
  if (type == "move")
    updateMove(world, level, json);

  // Play animation
  else if (type == "animation")
    updateAnimation(world, level, json);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientEntity::updateMove(const RPG::ClientWorld& world, const RPG::ClientLevel& level, const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(5).string();

  // Move to cell
  if (type == "run")
    handleMoveRun(world, level,
      json.get("coordinates").array(),
      json.contains("position") ? json.get("position").array() : position,
      json.contains("direction") ? (RPG::Direction)json.get("direction").number() : direction,
      json.contains("target") ? json.get("target").array() : _move.target,
      (float)json.get("speed").number()
    );

  // Teleport
  else if (type == "teleport")
    handleMoveTeleport(world, level,
      json.get("coordinates").array(),
      json.contains("position") ? json.get("position").array() : position,
      json.contains("direction") ? (RPG::Direction)json.get("direction").number() : direction
    );

  // Cancel current move
  else if (type == "cancel")
    handleMoveCancel(world, level);

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientEntity::updateAnimation(const RPG::ClientWorld& world, const RPG::ClientLevel& level, const Game::JSON::Object& json)
{
  const auto& type = json.get("type").array().get(5).string();

  // Start animation
  if (type == "start")
    handleAnimationStart(world, level,
      json.get("name").string(),
      json.contains("loop") ? json.get("loop").boolean() : false
    );

  // Change model
  else if (type == "model")
    handleAnimationModel(world, level, json.get("name").string());

  // Error
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::ClientEntity::handleMoveRun(const RPG::ClientWorld& world, const RPG::ClientLevel& level, RPG::Coordinates coordinates, RPG::Position position, RPG::Direction direction, RPG::Coordinates target, float speed)
{
  // Teleport
  if (speed <= 0.f) {
    handleMoveTeleport(world, level, coordinates, position, direction);
    return;
  }

  // Register move
  this->coordinates = coordinates;
  this->_move.target = target;
  this->_move.position = position;
  this->_move.remaining = 1.f;
  this->_move.speed = speed;
  
  // Start animation
  if (speed > 0.5f)
    setAnimation(world, level, RPG::Model::RunAnimation, true);
  else
    setAnimation(world, level, RPG::Model::WalkAnimation, true);
}

void  RPG::ClientEntity::handleMoveTeleport(const RPG::ClientWorld& world, const RPG::ClientLevel& level, RPG::Coordinates coordinates, RPG::Position position, RPG::Direction direction)
{
  // Register teleport
  this->coordinates = coordinates;
  this->position = position;
  this->_move.target = coordinates;
  this->_move.position = position;
  this->_move.remaining = 0.f;
  this->_move.speed = 1.f;
  
  // Start animation
  setAnimation(world, level, RPG::Model::IdleAnimation, true);
}

void  RPG::ClientEntity::handleMoveCancel(const RPG::ClientWorld& world, const RPG::ClientLevel& level)
{
  // Use current position as target
  _move.target = coordinates;

  // Stop run annimation
  if (_move.remaining == 0.f)
    setAnimation(world, level, RPG::Model::IdleAnimation, true);
}

void  RPG::ClientEntity::handleAnimationStart(const RPG::ClientWorld& world, const RPG::ClientLevel& level, const std::string& name, bool loop)
{
  // Start animation
  setAnimation(world, level, name, loop);
}

void  RPG::ClientEntity::handleAnimationModel(const RPG::ClientWorld& world, const RPG::ClientLevel& level, const std::string& name)
{
  // Change model
  setModel(world, level, name);
}

std::uint64_t RPG::ServerEntity::_maxId = 0;

std::uint64_t RPG::ServerEntity::generateId()
{
  // Just get the next max ID
  return ++_maxId;
}

RPG::ServerEntity::ServerEntity(const Game::JSON::Object& json) :
  id(json.contains("id") ? (std::uint64_t)json.get("id").number() : generateId()),
  coordinates(json.get("coordinates").array()),
  position(json.contains("position") ? json.get("position").array() : RPG::Entity::DefaultPosition),
  direction(json.contains("direction") ? (RPG::Direction)json.get("direction").number() : RPG::Entity::DefaultDirection),
  _move{
    .target = coordinates,
    .completion = 1.f,
    .speed = 0.f
  },
  _animation{
    .model = json.contains("model") ? json.get("model").string() : RPG::Entity::DefaultModel,
  },
  _actions{
    .pickup = json.contains("pickup") ? json.get("pickup").string() : "",
    .container = json.contains("container") ? json.get("container").string() : "",
    .dialog = json.contains("dialog") ? json.get("dialog").string() : "",
    .fight = json.contains("fight") ? json.get("fight").string() : "",
    .trigger = json.contains("trigger") ? json.get("trigger").string() : ""
  }
{
  // Always remember highest ID for generator
  _maxId = std::max(_maxId, id);
}

void  RPG::ServerEntity::update(const RPG::ServerWorld& world, float elapsed)
{
  // Update move
  updateMove(world, elapsed);
}

void  RPG::ServerEntity::updateMove(const RPG::ServerWorld& world, float elapsed)
{
  // Progress move
  _move.completion = std::min(1.f, _move.completion + elapsed * _move.speed);
  
  // Next move
  if (_move.completion == 1.f && coordinates != _move.target) {

  }

}

Game::JSON::Object RPG::ServerEntity::json() const
{
  Game::JSON::Object json;

  // Serialize entity
  json.set("id", (double)id);
  json.set("coordinates", coordinates.json());
  if (position != RPG::Entity::DefaultPosition)
    json.set("position", position.json());
  if (direction != RPG::Entity::DefaultDirection)
    json.set("direction", (double)direction);
  
  // Serialize animation
  if (_animation.model.empty() == false)
    json.set("model", _animation.model);

  // Serialize actions
  if (_actions.pickup.empty() == false)
    json.set("pickup", true);
  if (_actions.container.empty() == false)
    json.set("container", true);
  if (_actions.dialog.empty() == false)
    json.set("dialog", true);
  if (_actions.fight.empty() == false)
    json.set("fight", true);
  if (_actions.trigger.empty() == false)
    json.set("action", true);

  return json;
}
