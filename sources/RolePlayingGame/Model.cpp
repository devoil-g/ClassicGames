#include <cassert>

#include <SFML/Graphics/Sprite.hpp>

#include "RolePlayingGame/Model.hpp"
#include "System/Window.hpp"

const Math::Vector<2, std::int16_t> RPG::Sprite::DefaultOffset = { (std::int16_t)0, (std::int16_t)0 };
const Math::Vector<2, std::int16_t> RPG::Sprite::DefaultSize = { (std::int16_t)0, (std::int16_t)0 };
const Math::Vector<2, std::int16_t> RPG::Sprite::DefaultOrigin = { (std::int16_t)0, (std::int16_t)0 };
const Math::Vector<2, std::int16_t> RPG::Sprite::DefaultScale = { (std::int16_t)1, (std::int16_t)1 };
const RPG::Color                    RPG::Sprite::DefaultColor = RPG::Color::White;
const std::string                   RPG::Sprite::DefaultTexture = "error.png";

const RPG::Sprite RPG::Sprite::ErrorSprite;

RPG::Sprite::Sprite() :
  offset((std::int16_t)0, (std::int16_t)0),
  size((std::int16_t)8, (std::int16_t)8),
  origin((std::int16_t)4, (std::int16_t)4),
  scale((std::int16_t)1, (std::int16_t)1),
  color(RPG::Color::White),
  path("error.png"),
  texture(nullptr)
{}

RPG::Sprite::Sprite(const Game::JSON::Object& json) :
  offset(json.contains("offset") ? json.get("offset").array() : DefaultOffset),
  size(json.contains("size") ? json.get("size").array() : DefaultSize),
  origin(json.contains("origin") ? json.get("origin").array() : DefaultOrigin),
  scale(json.contains("scale") ? json.get("scale").array() : DefaultScale),
  color(json.contains("color") ? json.get("color").object() : DefaultColor),
  path(json.contains("texture") ? json.get("texture").string() : DefaultTexture),
  texture(nullptr)
{}

Game::JSON::Object  RPG::Sprite::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  if (offset != DefaultOffset)
    json.set("offset", offset.json());
  if (size != DefaultSize)
    json.set("size", size.json());
  if (origin != DefaultOrigin)
    json.set("origin", origin.json());
  if (scale != DefaultScale)
    json.set("scale", scale.json());
  if (color != DefaultColor)
    json.set("color", color.json());
  if (path != DefaultTexture)
    json.set("texture", path);

  return json;
}

void  RPG::Sprite::draw(const Math::Vector<2>& position, RPG::Color color, RPG::Color outline) const
{
  // Sprite not visible
  if (this->color.alpha == 0.f && color.alpha == 0.f)
    return;

  // No texture
  assert(texture != nullptr && "Missing texture.");

  auto&           window = Game::Window::Instance();
  sf::Sprite      sprite;
  Math::Vector<2> rounded(std::round(position.x()), std::round(position.y()));

  // Set properties
  sprite.setTexture(texture->get());
  sprite.setTextureRect(sf::IntRect(offset.x(), offset.y(), size.x(), size.y()));
  sprite.setOrigin((float)origin.x(), (float)origin.y());
  sprite.setScale((float)scale.x(), (float)scale.y());
  sprite.setColor(sf::Color((color * this->color).uint32()));

  // Draw outline
  if (outline.alpha > 0) {
    auto shader = RPG::Sprite::OutlineShader::Get(outline);

    // Draw outline
    sprite.setPosition(rounded.x() - 1.f, rounded.y());
    window.window().draw(sprite, shader);
    sprite.setPosition(rounded.x() + 1.f, rounded.y());
    window.window().draw(sprite, shader);
    sprite.setPosition(rounded.x(), rounded.y() - 1.f);
    window.window().draw(sprite, shader);
    sprite.setPosition(rounded.x(), rounded.y() + 1.f);
    window.window().draw(sprite, shader);
  }

  // Draw sprite
  sprite.setPosition(rounded.x(), rounded.y());
  window.window().draw(sprite);
}

RPG::Bounds RPG::Sprite::bounds(const Math::Vector<2>& position) const
{
  sf::Sprite      sprite;
  Math::Vector<2> rounded(std::round(position.x()), std::round(position.y()));

  // Set properties
  sprite.setPosition(rounded.x(), rounded.y());
  sprite.setTextureRect(sf::IntRect(offset.x(), offset.y(), size.x(), size.y()));
  sprite.setOrigin((float)origin.x(), (float)origin.y());
  sprite.setScale((float)scale.x(), (float)scale.y());
  
  // Use SFML to compute sprite bounds
  auto bounds = sprite.getGlobalBounds();

  return RPG::Bounds(
    { std::round(bounds.left), std::round(bounds.top) },
    { std::round(bounds.width), std::round(bounds.height) }
  );
}

RPG::Sprite::OutlineShader::OutlineShader() :
  _shader()
{
  // Failed to load shader
  if (_shader.loadFromMemory("uniform vec4 color; uniform sampler2D texture; void main() { vec4 pixel = texture2D(texture, gl_TexCoord[0].xy); pixel.xyz = color.xyz; pixel.w = pixel.w * color.w; gl_FragColor = gl_Color * pixel; }", sf::Shader::Type::Fragment) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Bind SFML texture of drawable
  _shader.setParameter("texture", sf::Shader::CurrentTexture);
}

const sf::Shader* RPG::Sprite::OutlineShader::Get(RPG::Color color)
{
  static RPG::Sprite::OutlineShader shader;

  // Set outline color
  shader._shader.setParameter("color", sf::Color(color.uint32()));

  return &shader._shader;
}

void  RPG::Sprite::resolve(const std::function<const RPG::Texture& (const std::string&)> library)
{
  // Reload texture pointer
  texture = &library(path);
}

// --- FRAME ------------------------------------------------------------------------------------------------------------------

const std::array<RPG::Sprite, RPG::Direction::DirectionCount> RPG::Frame::DefaultSprites = { RPG::Sprite(), RPG::Sprite(), RPG::Sprite(), RPG::Sprite(), RPG::Sprite(), RPG::Sprite() };
const float RPG::Frame::DefaultDuration = 0.f;

const RPG::Frame  RPG::Frame::ErrorFrame = RPG::Frame();

RPG::Frame::Frame() :
  sprites{ RPG::Sprite(), RPG::Sprite(), RPG::Sprite(), RPG::Sprite(), RPG::Sprite(), RPG::Sprite() },
  duration(0.f)
{}

RPG::Frame::Frame(const Game::JSON::Object& json) :
  sprites(DefaultSprites),
  duration(json.contains("duration") ? (float)json.get("duration").number() : DefaultDuration)
{
  // Load sprites of frame
  if (json.contains("sprites") == true)
  {
    // Single sprite, applied to every direction
    if (json.get("sprites").type() == Game::JSON::Type::TypeObject)
      sprites.fill(json.get("sprites").object());
    
    // Define sprite for every direction
    else {
      std::array<bool, RPG::Direction::DirectionCount>  done;

      // To check if every direction is filled
      done.fill(false);

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
}

Game::JSON::Object  RPG::Frame::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  if (duration != DefaultDuration)
    json.set("duration", duration);

  bool  same = true;

  std::array<std::string, RPG::Direction::DirectionCount> strings;

  // Check if every frame is identical
  for (auto direction = 1; direction < RPG::Direction::DirectionCount; direction++) {
    if (sprites[0] != sprites[direction]) {
      same = false;
      break;
    }
  }

  // Identical sprites
  if (same == true)
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

void  RPG::Frame::draw(RPG::Direction direction, const Math::Vector<2>& position, RPG::Color color, RPG::Color outline) const
{
  // Draw sprite
  sprites[direction].draw(position, color, outline);
}

RPG::Bounds RPG::Frame::bounds(RPG::Direction direction, const Math::Vector<2>& position) const
{
  // Get sprite bounds
  return sprites[direction].bounds(position);
}

void  RPG::Frame::resolve(const std::function<const RPG::Texture& (const std::string&)> library)
{
  // Resolve texture of each sprite
  for (auto& sprite : sprites)
    sprite.resolve(library);
}

// --- ANIMATION --------------------------------------------------------------------------------------------------------------

const RPG::Animation  RPG::Animation::ErrorAnimation = RPG::Animation();

const std::string RPG::Animation::DefaultAnimation = "idle";
const std::string RPG::Animation::IdleAnimation = "idle";
const std::string RPG::Animation::WalkAnimation = "walk";
const std::string RPG::Animation::RunAnimation = "run";

RPG::Animation::Animation() :
  _frames()
{}

RPG::Animation::Animation(const Game::JSON::Object& json) :
  _frames()
{
  // Get animation frames from JSON
  for (const auto& frame : json.get("frames").array()._vector)
    _frames.emplace_back(frame->object());

  // No frames
  if (_frames.empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

Game::JSON::Object  RPG::Animation::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  json.set("frames", Game::JSON::Array());
  for (const auto& frame : _frames)
    json.get("frames").array().push(frame.json());

  return json;
}

std::size_t RPG::Animation::count() const
{
  // Get the number of frames
  return _frames.size();
}

const RPG::Frame& RPG::Animation::frame(std::size_t index) const
{
  // Out of bound
  if (index >= _frames.size())
    return RPG::Frame::ErrorFrame;

  // Get frame at index
  return _frames[index];
}

void  RPG::Animation::resolve(const std::function<const RPG::Texture& (const std::string&)> library)
{
  // Resolve texture of each frame
  for (auto& frame : _frames)
    frame.resolve(library);
}

// --- MODEL ------------------------------------------------------------------------------------------------------------------

const RPG::Model  RPG::Model::ErrorModel = RPG::Model();

RPG::Model::Model() :
  _animations()
{}

RPG::Model::Model(const Game::JSON::Object& json) :
  _animations()
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

const RPG::Animation& RPG::Model::animation(const std::string& name) const
{
  auto iterator = _animations.find(name);

  // Animation found
  if (iterator != _animations.end())
    return iterator->second;

  // Error
  return RPG::Animation::ErrorAnimation;
}

const RPG::Animation& RPG::Model::random() const
{
  // Error, no animation
  if (_animations.empty() == true)
    return RPG::Animation::ErrorAnimation;

  // Select a random animation
  return std::next(_animations.begin(), std::rand() % _animations.size())->second;
}

void  RPG::Model::resolve(const std::function<const RPG::Texture& (const std::string&)> library)
{
  // Resolve texture of each animation
  for (auto& [_, animation] : _animations)
    animation.resolve(library);
}