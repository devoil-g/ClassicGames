#include "RolePlayingGame/Model.hpp"
#include "System/Config.hpp"
#include "System/Library/TextureLibrary.hpp"
#include "System/Window.hpp"

const std::string RPG::Model::DefaultAnimation = RPG::Model::IdleAnimation;
const std::string RPG::Model::IdleAnimation = "idle";
const std::string RPG::Model::WalkAnimation = "walk";
const std::string RPG::Model::RunAnimation = "run";

RPG::Model::Model(const Game::JSON::Object& json) :
  spritesheet(json.get("spritesheet").string()),
  animations()
{
  // Get animations from JSON
  for (const auto& animation : json.get("animations").array()._vector)
    animations.emplace(animation->object().get("name").string(), std::move(animation->object()));
}

RPG::Model::Animation& RPG::Model::animation(const std::string& name)
{
  // Get animation
  return animations.at(name);
}

const RPG::Model::Animation&  RPG::Model::animation(const std::string& name) const
{
  // Get animation
  return animations.at(name);
}

Game::JSON::Object  RPG::Model::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  json.set("spritesheet", spritesheet);

  // Serialize animations to JSON
  json.set("animations", Game::JSON::Array());
  json.get("animations").array()._vector.reserve(animations.size());
  for (const auto& [name, animation] : animations) {
    auto value = animation.json();

    value.set("name", name);
    json.get("animations").array().push(std::move(value));
  }

  return json;
}

RPG::Model::Animation::Animation(const Game::JSON::Object& json) :
  frames()
{
  // Get animation frames from JSON
  for (const auto& frame : json.get("frames").array()._vector)
    frames.emplace_back(frame->object());

  // No frames
  if (frames.empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

Game::JSON::Object  RPG::Model::Animation::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  json.set("frames", Game::JSON::Array());
  json.get("frames").array()._vector.reserve(frames.size());
  for (const auto& frame : frames)
    json.get("frames").array().push(frame.json());

  return json;
}

const Math::Vector<2, int> RPG::Model::Animation::Frame::DefaultOffset = { 0, 0 };
const Math::Vector<2, int> RPG::Model::Animation::Frame::DefaultSize = { 0, 0 };
const Math::Vector<2, int> RPG::Model::Animation::Frame::DefaultOrigin = { 0, 0 };
const bool                 RPG::Model::Animation::Frame::DefaultFlipX = false;
const bool                 RPG::Model::Animation::Frame::DefaultFlipY = false;
const RPG::Color           RPG::Model::Animation::Frame::DefaultColor = RPG::Color::Default;
const float                RPG::Model::Animation::Frame::DefaultDuration = 0.f;

RPG::Model::Animation::Frame::Frame(const Game::JSON::Object& json) :
  offset(json.contains("offset") ? json.get("offset").array() : DefaultOffset),
  size(json.contains("size") ? json.get("size").array() : DefaultSize),
  origin(json.contains("origin") ? json.get("origin").array() : DefaultOrigin),
  flipX(json.contains("flipX") ? json.get("flipX").boolean() : DefaultFlipX),
  flipY(json.contains("flipY") ? json.get("flipY").boolean() : DefaultFlipY),
  color(json.contains("color") ? json.get("color").object() : DefaultColor),
  duration(json.contains("duration") ? (float)json.get("duration").number() : DefaultDuration)
{}

Game::JSON::Object  RPG::Model::Animation::Frame::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  if (offset != DefaultOffset)
    json.set("offset", offset.json());
  if (size != DefaultSize)
    json.set("size", size.json());
  if (origin != DefaultOrigin)
    json.set("origin", origin.json());
  if (flipX != DefaultFlipX)
    json.set("flipX", flipX);
  if (flipY != DefaultFlipY)
    json.set("flipY", flipY);
  if (color != DefaultColor)
    json.set("color", color.json());
  if (duration != DefaultDuration)
    json.set("duration", (double)duration);

  return json;
}



void  RPG::Model::Animation::Frame::draw(const RPG::Texture& texture, const Math::Vector<2>& position, RPG::Color outline) const
{
  sf::Sprite  sprite;
  Math::Vector<2> rounded(std::round(position.x()), std::round(position.y()));

  // Set properties
  sprite.setTexture(texture.get());
  sprite.setTextureRect(sf::IntRect(offset.x(), offset.y(), size.x(), size.y()));
  sprite.setScale(flipX == true ? -1.f : +1.f, flipY == true ? -1.f : +1.f);
  sprite.setOrigin((float)origin.x(), (float)origin.y());
  sprite.setColor(sf::Color(color.red, color.green, color.blue, color.alpha));

  // Draw outline
  if (outline.alpha > 0) {
    auto shader = RPG::Model::OutlineShader::Get(outline);

    // Draw outline
    sprite.setPosition(rounded.x() - 1.f, rounded.y());
    Game::Window::Instance().window().draw(sprite, shader);
    sprite.setPosition(rounded.x() + 1.f, rounded.y());
    Game::Window::Instance().window().draw(sprite, shader);
    sprite.setPosition(rounded.x(), rounded.y() - 1.f);
    Game::Window::Instance().window().draw(sprite, shader);
    sprite.setPosition(rounded.x(), rounded.y() + 1.f);
    Game::Window::Instance().window().draw(sprite, shader);
  }

  // Draw sprite
  sprite.setPosition(rounded.x(), rounded.y());
  Game::Window::Instance().window().draw(sprite);
}

RPG::Bounds  RPG::Model::Animation::Frame::bounds(const Math::Vector<2>& position) const
{
  sf::Sprite  sprite;

  // Set properties
  sprite.setPosition(std::round(position.x()), std::round(position.y()));
  sprite.setTextureRect(sf::IntRect(offset.x(), offset.y(), size.x(), size.y()));
  sprite.setScale(flipX == true ? -1.f : +1.f, flipY == true ? -1.f : +1.f);
  sprite.setOrigin((float)origin.x(), (float)origin.y());

  // Use SFML to compute sprite bounds
  auto bounds = sprite.getGlobalBounds();

  return RPG::Bounds(
    { std::round(bounds.left), std::round(bounds.top) },
    { std::round(bounds.width), std::round(bounds.height) }
  );
}

RPG::Model::OutlineShader::OutlineShader() :
  _shader()
{
  // Failed to load shader
  if (_shader.loadFromMemory("uniform vec4 color; uniform sampler2D texture; void main() { vec4 pixel = texture2D(texture, gl_TexCoord[0].xy); pixel.xyz = color.xyz; pixel.w = pixel.w * color.w; gl_FragColor = gl_Color * pixel; }", sf::Shader::Type::Fragment) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Bind SFML texture of drawable
  _shader.setParameter("texture", sf::Shader::CurrentTexture);
}

const sf::Shader* RPG::Model::OutlineShader::Get(RPG::Color color)
{
  static RPG::Model::OutlineShader shader;

  // Set outline color
  shader._shader.setParameter("color", sf::Color(color.red, color.green, color.blue, color.alpha));

  return &shader._shader;
}