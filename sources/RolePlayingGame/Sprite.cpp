#include <cassert>

#include <SFML/Graphics/Sprite.hpp>

#include "RolePlayingGame/Sprite.hpp"
#include "System/Window.hpp"

const RPG::Sprite::Bounds           RPG::Sprite::DefaultTexture = { .origin = { (std::int16_t)0, (std::int16_t)0 }, .size = { (std::int16_t)0, (std::int16_t)0 } };
const Math::Vector<2, float>        RPG::Sprite::DefaultOrigin = { 0.f, 0.f };
const Math::Vector<2, std::int16_t> RPG::Sprite::DefaultScale = { (std::int16_t)1, (std::int16_t)1 };
const RPG::Color                    RPG::Sprite::DefaultColor = RPG::Color::White;
const std::string                   RPG::Sprite::DefaultPath = "error.png";

const RPG::Sprite RPG::Sprite::ErrorSprite;

RPG::Sprite::Sprite() :
  texture{ .origin { (std::int16_t)0, (std::int16_t)0 }, .size = { (std::int16_t)8, (std::int16_t)8 } },
  select(texture),
  origin(4.f, 4.f),
  scale((std::int16_t)1, (std::int16_t)1),
  color(RPG::Color::White),
  path("error.png"),
  pointer(nullptr)
{}

RPG::Sprite::Sprite(const Game::JSON::Object& json) :
  texture(json.contains("texture") ? RPG::Sprite::Bounds{.origin{ json.get("texture").object().get("origin").array() }, .size{ json.get("texture").object().get("size").array() } } : DefaultTexture),
  select(json.contains("select") ? RPG::Sprite::Bounds{ .origin{ json.get("select").object().get("origin").array() }, .size{ json.get("select").object().get("size").array() } } : RPG::Sprite::Bounds{ .origin = Math::Vector<2, std::int16_t>((std::int16_t)0, (std::int16_t)0), .size = texture.size }),
  origin(json.contains("origin") ? json.get("origin").array() : DefaultOrigin),
  scale(json.contains("scale") ? json.get("scale").array() : DefaultScale),
  color(json.contains("color") ? json.get("color").object() : DefaultColor),
  path(json.contains("texture") ? json.get("path").string() : DefaultPath),
  pointer(nullptr)
{}

Game::JSON::Object  RPG::Sprite::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  if (texture != DefaultTexture) {
    json.set("texture", Game::JSON::Object());
    json.get("texture").object().set("origin", texture.origin.json());
    json.get("texture").object().set("size", texture.size.json());
  }
  if (select.origin != Math::Vector<2, std::int16_t>((std::int16_t)0, (std::int16_t)0) || select.size != texture.size) {
    json.set("select", Game::JSON::Object());
    json.get("select").object().set("origin", select.origin.json());
    json.get("select").object().set("size", select.size.json());
  }
  if (origin != DefaultOrigin)
    json.set("origin", origin.json());
  if (scale != DefaultScale)
    json.set("scale", scale.json());
  if (color != DefaultColor)
    json.set("color", color.json());
  if (path != DefaultPath)
    json.set("path", path);

  return json;
}

void  RPG::Sprite::draw(const Math::Vector<2>& position, RPG::Color color, RPG::Color outline) const
{
  // Sprite not visible
  if (this->color.alpha == 0.f && color.alpha == 0.f)
    return;

  // No texture
  assert(pointer != nullptr && "Missing texture.");

  auto&           window = Game::Window::Instance();
  Math::Vector<2> originTrunc;
  Math::Vector<2> rounded(std::round(position.x() - std::modf(origin.x(), &originTrunc.x())), std::round(position.y() - std::modf(origin.y(), &originTrunc.y())));
  sf::Sprite      sprite(pointer->get());

  // Set properties
  sprite.setPosition({ rounded.x(), rounded.y() });
  sprite.setTextureRect(sf::IntRect({ texture.origin.x(), texture.origin.y() }, { texture.size.x(), texture.size.y() }));
  sprite.setOrigin({ originTrunc.x(), originTrunc.y() });
  sprite.setScale({ (float)scale.x(), (float)scale.y() });
  sprite.setColor(sf::Color((color * this->color).uint32()));
  
  // Draw sprite
  if (outline.alpha == 0)
    window.draw(sprite);
  else
    window.draw(sprite, RPG::Sprite::OutlineShader::Get(outline));
}

RPG::Bounds RPG::Sprite::bounds(const Math::Vector<2>& position) const
{
  Math::Vector<2> originTrunc;
  Math::Vector<2> rounded(std::round(position.x() - std::modf(origin.x(), &originTrunc.x())), std::round(position.y() - std::modf(origin.y(), &originTrunc.y())));
  sf::Texture     empty;
  sf::Sprite      sprite(empty);

  // Set properties
  sprite.setPosition({ rounded.x(), rounded.y() });
  sprite.setTextureRect(sf::IntRect({ texture.origin.x() + select.origin.x(), texture.origin.y() + select.origin.y() }, { select.size.x(), select.size.y() }));
  sprite.setOrigin({ originTrunc.x() - select.origin.x(), originTrunc.y() - select.origin.y() });
  sprite.setScale({ (float)scale.x(), (float)scale.y() });
  
  // Use SFML to compute sprite bounds
  auto bounds = sprite.getGlobalBounds();

  return RPG::Bounds(
    { std::round(bounds.position.x), std::round(bounds.position.y) },
    { std::round(bounds.size.x), std::round(bounds.size.y) }
  );
}

RPG::Sprite::OutlineShader::OutlineShader() :
  _shader()
{
  // Failed to load shader
  if (_shader.loadFromMemory(
    "uniform vec4 color;"
    "uniform sampler2D texture;"
    "void main() {"
    "  vec2 one = vec2(1, 1) / textureSize(texture, 0);"
    "  vec4 outline = vec4(color.r, color.g, color.b, texture2D(texture, vec2(gl_TexCoord[0].x - one.x, gl_TexCoord[0].y)).a);"
    "  outline.a = max(outline.a, texture2D(texture, vec2(gl_TexCoord[0].x + one.x, gl_TexCoord[0].y)).a);"
    "  outline.a = max(outline.a, texture2D(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y - one.y)).a);"
    "  outline.a = max(outline.a, texture2D(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + one.y)).a);"
    "  outline.a = outline.a * color.a;"
    "  vec4 pixel = texture2D(texture, gl_TexCoord[0].xy) * gl_Color;"
    "  gl_FragColor.a = outline.a + (1 - outline.a) * pixel.a;"
    "  gl_FragColor.rgb = (outline.rgb * outline.a * (1 - pixel.a) + pixel.rgb * pixel.a) / min(gl_FragColor.a, 1);"
    "}", sf::Shader::Type::Fragment) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Bind SFML texture of drawable
  _shader.setUniform("texture", sf::Shader::CurrentTexture);
}

const sf::Shader* RPG::Sprite::OutlineShader::Get(RPG::Color color)
{
  static RPG::Sprite::OutlineShader shader;

  // Set outline color
  shader._shader.setUniform("color", sf::Glsl::Vec4(sf::Color(color.uint32())));

  return &shader._shader;
}