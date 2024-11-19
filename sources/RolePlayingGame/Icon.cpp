#include "RolePlayingGame/Icon.hpp"

const RPG::Icon RPG::Icon::ErrorIcon;

RPG::Icon::Icon() :
  _animations()
{}

RPG::Icon::Icon(const Game::JSON::Object& json) :
  _animations()
{
  // Get animations from JSON
  for (const auto& animation : json.get("animations").array()._vector)
    _animations.emplace(animation->object().get("name").string(), animation->object());
}

Game::JSON::Object  RPG::Icon::json() const
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

void  RPG::Icon::resolve(const std::function<const RPG::Texture& (const std::string&)> library)
{
  // Resolve texture of each animation
  for (auto& [_, animation] : _animations)
    for (auto& frame : animation.frames)
      frame.sprite.texture = &library(frame.sprite.path);
}

RPG::Icon::Actor::Actor() :
  _icon(nullptr),
  _animation(nullptr),
  _frame(0),
  _duration(0.f),
  _speed(1.f),
  _loop(false)
{}

const std::string RPG::Icon::Actor::DefaultAnimation = "default";

RPG::Icon::Actor::Actor(const RPG::Icon& icon) :
  Actor()
{
  // Start with default animation
  setIcon(icon);
}

void  RPG::Icon::Actor::update(float elapsed)
{
  // No animation
  if (_animation == nullptr)
    return;

  // Consume animation time
  _duration -= elapsed * _speed;

  while (_duration < 0.f && _animation->frames[_frame].duration > 0.f)
  {
    // Get to next frame
    _frame = (_frame + 1) % _animation->frames.size();

    // Animation not looping
    if (_frame == 0 && _loop == false)
      setAnimation(RPG::Icon::Actor::DefaultAnimation, 1.f, true);

    // Increase animation timer with new frame duration
    else {
      if (_animation->frames[_frame].duration > 0.f)
        _duration += _animation->frames[_frame].duration;
      else
        _duration = 0.f;
    }
  }
}

const RPG::Sprite& RPG::Icon::Actor::sprite() const
{
  // No animation
  if (_animation == nullptr)
    return RPG::Sprite::ErrorSprite;

  // Get current frame of animation
  else
    return _animation->frames[_frame].sprite;
}

void  RPG::Icon::Actor::setIcon(const RPG::Icon& icon)
{
  // Change icon
  _icon = &icon;

  // Reset to default animation
  setAnimation(RPG::Icon::Actor::DefaultAnimation, true, 1.f);
}

void  RPG::Icon::Actor::setAnimation(const std::string& name, bool loop, float speed)
{
  // No model
  if (_icon == nullptr)
    return;

  auto iterator = _icon->_animations.find(name);

  // Invalid animation
  if (iterator == _icon->_animations.end()) {
    _animation = nullptr;
    _frame = 0;
    _duration = 0.f;
    _speed = 1.f;
    _loop = false;
  }

  // Register new animation
  else {
    _animation = &iterator->second;
    _frame = 0;
    _duration = std::min(0.f, _duration) + _animation->frames[_frame].duration;
    _speed = speed;
    _loop = loop;
  }
}

void  RPG::Icon::Actor::setAnimationRandom(bool loop, float speed)
{
  // No icon
  if (_icon == nullptr)
    return;

  // Error, no animation
  if (_icon->_animations.empty() == true)
    setAnimation(RPG::Icon::Actor::DefaultAnimation, 1.f, true);

  // Select a random animation
  else
    setAnimation(std::next(_icon->_animations.begin(), std::rand() % _icon->_animations.size())->first, loop, speed);
}

bool  RPG::Icon::Actor::operator==(const RPG::Icon& icon)
{
  // Compare model pointers
  return _icon == &icon;
}

bool  RPG::Icon::Actor::operator!=(const RPG::Icon& icon)
{
  // Inverse comparison
  return !(*this == icon);
}

RPG::Icon::Animation::Animation(const Game::JSON::Object& json) :
  frames()
{
  // Get animation frames from JSON
  for (const auto& frame : json.get("frames").array()._vector)
    frames.emplace_back(frame->object());

  // No frames
  if (frames.empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

Game::JSON::Object  RPG::Icon::Animation::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  json.set("frames", Game::JSON::Array());
  for (const auto& frame : frames)
    json.get("frames").array().push(frame.json());

  return json;
}

const float RPG::Icon::Animation::Frame::DefaultDuration = 1.f;

RPG::Icon::Animation::Frame::Frame(const Game::JSON::Object& json) :
  sprite(json.get("sprite").object()),
  duration(json.contains("duration") ? (float)json.get("duration").number() : DefaultDuration)
{}

Game::JSON::Object  RPG::Icon::Animation::Frame::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  json.set("sprite", sprite.json());
  if (duration != DefaultDuration)
    json.set("duration", duration);
  
  return json;
}
