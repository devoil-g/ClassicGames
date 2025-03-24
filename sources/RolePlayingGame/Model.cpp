#include "RolePlayingGame/Model.hpp"

const std::string RPG::Model::DefaultIcon = "";
const RPG::Model  RPG::Model::ErrorModel;

RPG::Model::Model() :
  _animations(),
  _icon()
{}

RPG::Model::Model(const Game::JSON::Object& json) :
  _animations(),
  _icon(json.contains("icon") ? json.get("icon").string() : DefaultIcon)
{
  // Get animations from JSON
  for (const auto& animation : json.get("animations").array()._vector)
    _animations.emplace(animation->object().get("name").string(), animation->object());
}

Game::JSON::Object  RPG::Model::json() const
{
  Game::JSON::Object  json;

  // Serialize animations to JSON
  json.set("animations", Game::JSON::Array());
  for (const auto& [name, animation] : _animations) {
    auto value = animation.json();

    value.set("name", name);
    json.get("animations").array().push(std::move(value));
  }

  return json;
}

void  RPG::Model::resolve(const std::function<const RPG::Texture& (const std::string&)> library)
{
  // Resolve texture of each animation
  for (auto& [_, animation] : _animations)
    for (auto& frame : animation.frames)
      for (auto& sprite : frame.sprites)
        sprite.pointer = &library(sprite.path);
}

const std::string RPG::Model::Actor::DefaultAnimation = "idle";
const std::string RPG::Model::Actor::IdleAnimation = "idle";
const std::string RPG::Model::Actor::WalkAnimation = "walk";
const std::string RPG::Model::Actor::RunAnimation = "run";

RPG::Model::Actor::Actor() :
  _model(nullptr),
  _animation(nullptr),
  _frame(0),
  _duration(0.f),
  _speed(1.f),
  _elapsed(0.f),
  _loop(false)
{}

RPG::Model::Actor::Actor(const RPG::Model& model) :
  Actor()
{
  // Start with default animation
  setModel(model);
}

void  RPG::Model::Actor::update(float elapsed)
{
  // Consume animation time
  _duration -= elapsed * _speed;
  _elapsed += elapsed * _speed;

  // Run animation
  while (_animation != nullptr && _duration < 0.f && _animation->frames[_frame].duration > 0.f)
  {
    // Get to next frame
    _frame = (_frame + 1) % _animation->frames.size();

    // Animation not looping
    if (_frame == 0 && _loop == false) {
      float save = _duration;

      setAnimation(RPG::Model::Actor::DefaultAnimation, 1.f, true);
      _duration = save;
      _elapsed = -save;
    }

    // Increase animation timer with new frame duration
    else {
      if (_animation->frames[_frame].duration > 0.f)
        _duration += _animation->frames[_frame].duration;
      else
        _duration = 0.f;
    }
  }
}

const RPG::Sprite& RPG::Model::Actor::sprite(RPG::Direction direction) const
{
  // No animation
  if (_animation == nullptr)
    return RPG::Sprite::ErrorSprite;

  // Get current frame of animation
  else {
    const auto& sprites = _animation->frames[_frame].sprites;

    return sprites[direction % sprites.size()];
  }
}

const std::string& RPG::Model::Actor::icon() const
{
  // No model
  if (_model == nullptr)
    return RPG::Model::ErrorModel._icon;

  // Get current model icon
  return _model->_icon;
}

void  RPG::Model::Actor::setModel(const RPG::Model& model)
{
  // Change model
  _model = &model;

  // Reset to default animation
  setAnimation(RPG::Model::Actor::DefaultAnimation, true, 1.f);
}

void  RPG::Model::Actor::setAnimation(const std::string& name, bool loop, float speed)
{
  // No model
  if (_model == nullptr)
    return;

  auto iterator = _model->_animations.find(name);

  // Invalid animation
  if (iterator == _model->_animations.end()) {
    _animation = nullptr;
    _frame = 0;
    _duration = 0.f;
    _speed = 1.f;
    _elapsed = 0.f;
    _loop = false;
  }

  // Register new animation
  else {
    _animation = &iterator->second;
    _frame = 0;
    _duration = std::min(0.f, _duration) + _animation->frames[_frame].duration;
    _speed = speed;
    _elapsed = 0.f;
    _loop = loop;
  }
}

float RPG::Model::Actor::getAnimationElapsed() const
{
  // Get elapsed time in current animation
  return _elapsed;
}

float RPG::Model::Actor::getAnimationDuration() const
{
  // No animation
  if (_animation == nullptr)
    return 0.f;

  // Get duration of current animation
  return _animation->duration;
}

bool  RPG::Model::Actor::operator==(const RPG::Model& model)
{
  // Compare model pointers
  return _model == &model;
}

bool  RPG::Model::Actor::operator!=(const RPG::Model& model)
{
  // Inverse comparison
  return !(*this == model);
}

RPG::Model::Animation::Animation(const Game::JSON::Object& json) :
  frames(),
  duration(0.f)
{
  // Get animation frames from JSON
  for (const auto& frame : json.get("frames").array()._vector)
    frames.emplace_back(frame->object());

  // No frames
  if (frames.empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Compute total duration of animation
  for (const auto& frame : frames)
    duration += frame.duration;
}

Game::JSON::Object  RPG::Model::Animation::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  json.set("frames", Game::JSON::Array());
  for (const auto& frame : frames)
    json.get("frames").array().push(frame.json());

  return json;
}

const float RPG::Model::Animation::Frame::DefaultDuration = 1.f;

RPG::Model::Animation::Frame::Frame(const Game::JSON::Object& json) :
  sprites(),
  duration(json.contains("duration") ? (float)json.get("duration").number() : DefaultDuration)
{
  // Single sprite, applied to every direction
  if (json.get("sprites").type() == Game::JSON::Type::TypeObject)
    sprites.push_back(json.get("sprites").object());

  // Define sprite for every direction
  else {
    std::array<bool, RPG::Direction::DirectionCount>  done;

    // To check if every direction is filled
    done.fill(false);

    // Resize container to fit every direction
    sprites.resize(RPG::Direction::DirectionCount);

    // Get each sprite from JSON
    for (const auto& sprite : json.get("sprites").array()._vector) {
      const auto& object = sprite->object();
      auto direction = RPG::StringToDirection(object.get("direction").string());

      // Already defined
      if (done[direction] == true)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

      done[direction] = true;
      sprites[direction] = object;
    }

    // Check that each direction is defined
    for (auto defined : done)
      if (defined == false)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
}

Game::JSON::Object  RPG::Model::Animation::Frame::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  if (duration != DefaultDuration)
    json.set("duration", duration);

  // Simple frame
  if (sprites.size() == 1)
    json.set("sprites", sprites.front().json());

  // Different sprites
  else {
    auto array = Game::JSON::Array();

    array._vector.reserve(RPG::Direction::DirectionCount);
    for (auto direction = 0; direction < RPG::Direction::DirectionCount; direction++) {
      auto sprite = sprites[direction].json();

      sprite.set("direction", RPG::DirectionToString((RPG::Direction)direction));
      array.push(std::move(sprite));
    }

    json.set("sprites", std::move(array));
  }

  return json;
}
