#include "RolePlayingGame/Model.hpp"
#include "System/Config.hpp"
#include "System/Library/TextureLibrary.hpp"
#include "System/Window.hpp"

RPG::Model::Model(const Game::JSON::Object& json) :
  spritesheet(json.contains("spritesheet") ? json.get("spritesheet").string() : ""),
  animations()
{
  // Get animations from JSON
  for (const auto& animation : json.get("animations").array()._vector)
    animations.emplace(animation->object().get("name").string(), std::move(animation->object()));
}

Game::JSON::Object  RPG::Model::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  if (spritesheet.empty() == false)
    json.set("spritesheet", spritesheet);

  // Serialize animations to JSON
  json.set("animations", Game::JSON::Array());
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

const Math::Vector<2, unsigned int> RPG::Model::Animation::Frame::DefaultOrigin = { 0u, 0u };
const Math::Vector<2, unsigned int> RPG::Model::Animation::Frame::DefaultSize = { 0u, 0u };
const Math::Vector<2, int>          RPG::Model::Animation::Frame::DefaultOffset = { 0, 0 };
const bool                          RPG::Model::Animation::Frame::DefaultFlipX = false;
const bool                          RPG::Model::Animation::Frame::DefaultFlipY = false;
const RPG::Color                    RPG::Model::Animation::Frame::DefaultColor = RPG::Color::Default;
const double                        RPG::Model::Animation::Frame::DefaultDuration = 0.0;

RPG::Model::Animation::Frame::Frame(const Game::JSON::Object& json) :
  origin(json.contains("origin") ? json.get("origin").array() : DefaultOrigin),
  size(json.contains("size") ? json.get("size").array() : DefaultSize),
  offset(json.contains("offset") ? json.get("offset").array() : DefaultOffset),
  flipX(json.contains("flipX") ? json.get("flipX").boolean() : DefaultFlipX),
  flipY(json.contains("flipY") ? json.get("flipY").boolean() : DefaultFlipY),
  color(json.contains("color") ? json.get("color").object() : DefaultColor),
  duration(json.contains("duration") ? json.get("duration").number() : DefaultDuration)
{}

Game::JSON::Object  RPG::Model::Animation::Frame::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  if (origin != DefaultOrigin)
    json.set("origin", origin.json());
  if (size != DefaultSize)
    json.set("size", size.json());
  if (offset != DefaultOffset)
    json.set("offset", offset.json());
  if (flipX != DefaultFlipX)
    json.set("flipX", flipX);
  if (flipY != DefaultFlipY)
    json.set("flipY", flipY);
  if (color != DefaultColor)
    json.set("color", color.json());
  if (duration != DefaultDuration)
    json.set("duration", duration);

  return json;
}

void  RPG::Model::Animation::Frame::draw(const std::string& spritesheet, const RPG::Coordinates& position) const
{
  // Nothing to draw
  if (spritesheet.empty() == true)
    return;

  sf::Sprite  sprite;

  // Set properties
  sprite.setPosition((float)position.x(), (float)position.y());
  sprite.setTexture(Game::TextureLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "rpg" / spritesheet));
  sprite.setTextureRect(sf::IntRect(origin.x(), origin.y(), size.x(), size.y()));
  sprite.setScale(flipX == true ? -1.f : +1.f, flipY == true ? -1.f : +1.f);
  sprite.setOrigin((float)offset.x(), (float)offset.y());
  sprite.setColor(sf::Color(color.red, color.green, color.blue, color.alpha));
  
  // Draw sprite
  Game::Window::Instance().window().draw(sprite);
}

RPG::Bounds  RPG::Model::Animation::Frame::bounds() const
{
  sf::Sprite  sprite;

  // Set properties
  sprite.setTextureRect(sf::IntRect(origin.x(), origin.y(), size.x(), size.y()));
  sprite.setScale(flipX == true ? -1.f : +1.f, flipY == true ? -1.f : +1.f);
  sprite.setOrigin((float)offset.x(), (float)offset.y());
  sprite.setColor(sf::Color(color.red, color.green, color.blue, color.alpha));

  // Use SFML to compute sprite bounds
  auto bounds = sprite.getGlobalBounds();

  return RPG::Bounds(
    { (int)std::lround(bounds.left), (int)std::lround(bounds.top) },
    { (int)std::lround(bounds.width), (int)std::lround(bounds.height) }
  );
}