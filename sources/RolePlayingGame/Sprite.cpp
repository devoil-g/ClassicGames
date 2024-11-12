#include <cassert>

#include <SFML/Graphics/Sprite.hpp>

#include "RolePlayingGame/Sprite.hpp"
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