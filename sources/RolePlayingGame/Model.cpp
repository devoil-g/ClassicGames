#include "RolePlayingGame/Model.hpp"
#include "System/Utilities.hpp"

RPG::Model  RPG::Model::ErrorModel = RPG::Model();

RPG::Model::Model() :
  _animations()
{}

RPG::Model::Model(const Game::JSON::Object& json) :
  Model()
{
  // Load animations
  load(json);
}

Game::JSON::Object  RPG::Model::json() const
{
  Game::JSON::Object  json;

  // Serialize animations to JSON
  json.set(L"animations", Game::JSON::Array());
  for (const auto& [name, animation] : _animations)
  {
    // Ignore empty animations
    if (animation.frames.empty() == true)
      continue;

    auto value = animation.json();

    value.set(L"name", name);
    json.get(L"animations").array().push(std::move(value));
  }

  return json;
}

void  RPG::Model::load(const Game::JSON::Object& json)
{
  // Get animations from JSON
  for (const auto& animation : json.get(L"animations").array())
    _animations[animation->object().get(L"name").string()].load(animation->object());
}

void  RPG::Model::resolve(const std::function<const RPG::Texture& (const std::wstring&)> library)
{
  // Resolve texture of each animation
  for (auto& [_, animation] : _animations)
    for (auto& frame : animation.frames)
      for (auto& sprite : frame.sprites)
        sprite.pointer = &library(sprite.path);
}

const std::wstring  RPG::Model::Actor::DefaultAnimation = L"idle";
const std::wstring  RPG::Model::Actor::IdleAnimation = L"idle";
const std::wstring  RPG::Model::Actor::WalkAnimation = L"walk";
const std::wstring  RPG::Model::Actor::RunAnimation = L"run";

RPG::Model::Actor::Actor() :
  _model(RPG::Model::ErrorModel),
  _animation(RPG::Model::ErrorModel._animations[DefaultAnimation]),
  _frame(0),
  _cursor(0.f),
  _speed(+1.f),
  _mode(Mode::Normal)
{}

RPG::Model::Actor::Actor(RPG::Model& model) :
  Actor()
{
  // Register model
  setModel(model); 
}

void  RPG::Model::Actor::update(float elapsed)
{
  // Consume animation time
  _cursor += elapsed * _speed;

  // Empty animation
  if (_animation.get().frames.empty() == true)
    return;

  // Handle animation change
  _frame = std::clamp(_frame, (size_t)0, _animation.get().frames.size() - 1);

  // Run animation
  while (true)
  {
    // Previous frame
    if (_cursor < 0.f) {
      switch (_mode) {
      case Mode::Normal:
        if (_frame == 0) {
          _cursor = 0.f;
          return;
        }
        break;
      case Mode::Loop:
        break;
      case Mode::PingPong:
        if (_frame == 0) {
          _cursor = -_cursor;
          _speed = -_speed;
          continue;
        }
        break;
      default:
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      }

      // Get previous frame
      _frame = (_frame + _animation.get().frames.size() - 1) % _animation.get().frames.size();
      _cursor += _animation.get().frames[_frame].duration;
    }

    // Next frame
    else if (_cursor > _animation.get().frames[_frame].duration) {
      switch (_mode) {
      case Mode::Normal:
        if (_frame == _animation.get().frames.size() - 1) {
          _cursor = _animation.get().frames[_frame].duration;
          return;
        }
        break;
      case Mode::Loop:
        break;
      case Mode::PingPong:
        if (_frame == _animation.get().frames.size() - 1) {
          _cursor = 2 * _animation.get().frames[_frame].duration - _cursor;
          _speed = -_speed;
          continue;
        }
        break;
      default:
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      }
      
      // Get next frame
      _cursor -= _animation.get().frames[_frame].duration;
      _frame = (_frame + 1) % _animation.get().frames.size();
    }

    // No frame change
    else
      break;
  }
}

const RPG::Sprite& RPG::Model::Actor::sprite(RPG::Direction direction) const
{
  // No animation
  if (_animation.get().frames.empty() == true)
    return RPG::Sprite::ErrorSprite;

  // Get current frame of animation
  else {
    const auto& sprites = _animation.get().frames[std::clamp(_frame, (std::size_t)0, _animation.get().frames.size() - 1)].sprites;

    return sprites[direction % sprites.size()];
  }
}

const std::wstring& RPG::Model::Actor::icon() const
{
  // Get current model icon
  return _model.get()._icon;
}

void  RPG::Model::Actor::setModel(RPG::Model& model)
{
  // Change model
  _model = model;
}

void  RPG::Model::Actor::setAnimation(const std::wstring& name, Mode mode, float speed)
{
  // Base parameters
  _animation =_model.get()._animations[name];
  _speed = speed;
  _mode = mode;

  // Invalid animation
  if (_animation.get().frames.empty() == true) {
    _frame = 0;
    _cursor = 0.f;
  }

  // Register new animation
  else
  {
    // Playing forward
    if (_speed >= 0.f) {
      _frame = 0;
      _cursor = 0.f;
    }
    // Playing backward
    else {
      _frame = _animation.get().frames.size() - 1;
      _cursor = _animation.get().frames[_frame].duration;
    }
  }
}

void  RPG::Model::Actor::setSpeed(float speed)
{
  // Set animation speed
  _speed = speed;
}

void  RPG::Model::Actor::setMode(Mode mode)
{
  // Set animation mode
  _mode = mode;
}

float RPG::Model::Actor::getSpeed() const
{
  // Get animation speed
  return _speed;
}

RPG::Model::Actor::Mode RPG::Model::Actor::getMode() const
{
  // Get animation mode
  return _mode;
}

bool  RPG::Model::Actor::operator==(const RPG::Model& model) const
{
  // Compare model pointers
  return &_model.get() == &model;
}

bool  RPG::Model::Actor::operator!=(const RPG::Model& model) const
{
  // Inverse comparison
  return !(*this == model);
}

RPG::Model::Animation::Animation() :
  frames(),
  duration(0.f)
{}

RPG::Model::Animation::Animation(const Game::JSON::Object& json) :
  Animation()
{
  // Load animations from JSON
  load(json);
}

Game::JSON::Object  RPG::Model::Animation::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  json.set(L"frames", Game::JSON::Array());
  for (const auto& frame : frames)
    json.get(L"frames").array().push(frame.json());

  return json;
}

void  RPG::Model::Animation::load(const Game::JSON::Object& json)
{
  // Reset frames
  frames.clear();
  duration = 0.f;

  // Get animation frames from JSON
  for (const auto& frame : json.get(L"frames").array())
    frames.emplace_back(frame->object());

  // No frames
  if (frames.empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Compute total duration of animation
  for (const auto& frame : frames)
    duration += frame.duration;
}

const float RPG::Model::Animation::Frame::DefaultDuration = 1.f;

RPG::Model::Animation::Frame::Frame(const Game::JSON::Object& json) :
  sprites(),
  duration(json.contains(L"duration") ? (float)json.get(L"duration").number() : DefaultDuration)
{
  // Single sprite, applied to every direction
  if (json.get(L"sprites").type() == Game::JSON::Type::Object)
    sprites.push_back(json.get(L"sprites").object());

  // Define sprite for every direction
  else {
    std::array<bool, RPG::Direction::DirectionCount>  done;

    // To check if every direction is filled
    done.fill(false);

    // Resize container to fit every direction
    sprites.resize(RPG::Direction::DirectionCount);

    // Get each sprite from JSON
    for (const auto& sprite : json.get(L"sprites").array()) {
      const auto& object = sprite->object();
      auto direction = RPG::StringToDirection(object.get(L"direction").string());

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
    json.set(L"duration", duration);

  // Simple frame
  if (sprites.size() == 1)
    json.set(L"sprites", sprites.front().json());

  // Different sprites
  else {
    auto array = Game::JSON::Array();

    array.reserve(RPG::Direction::DirectionCount);
    for (auto direction = 0; direction < RPG::Direction::DirectionCount; direction++) {
      auto sprite = sprites[direction].json();

      sprite.set(L"direction", RPG::DirectionToString((RPG::Direction)direction));
      array.push(std::move(sprite));
    }

    json.set(L"sprites", std::move(array));
  }

  return json;
}
