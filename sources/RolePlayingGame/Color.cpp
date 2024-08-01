#include "RolePlayingGame/Color.hpp"

const RPG::Color    RPG::Color::Default = RPG::Color(DefaultRed, DefaultGreen, DefaultBlue, DefaultAlpha);
const std::uint8_t  RPG::Color::DefaultRed = 255;
const std::uint8_t  RPG::Color::DefaultGreen = 255;
const std::uint8_t  RPG::Color::DefaultBlue = 255;
const std::uint8_t  RPG::Color::DefaultAlpha = 255;

const RPG::Color    RPG::Color::White = RPG::Color(255, 255, 255);
const RPG::Color    RPG::Color::Black = RPG::Color(0, 0, 0);
const RPG::Color    RPG::Color::Red = RPG::Color(255, 0, 0);
const RPG::Color    RPG::Color::Green = RPG::Color(0, 255, 0);
const RPG::Color    RPG::Color::Blue = RPG::Color(0, 0, 255);
const RPG::Color    RPG::Color::Yellow = RPG::Color(255, 255, 0);
const RPG::Color    RPG::Color::Magenta = RPG::Color(255, 0, 255);
const RPG::Color    RPG::Color::Cyan = RPG::Color(0, 255, 255);
const RPG::Color    RPG::Color::Transparent = RPG::Color(255, 255, 255, 0);

RPG::Color::Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha) :
  raw(((std::uint32_t)red << 24) + ((std::uint32_t)green << 16) + ((std::uint32_t)blue << 8) + ((std::uint32_t)alpha << 0))
{}

RPG::Color::Color(std::uint32_t raw) :
  raw(raw)
{}

RPG::Color::Color(const Game::JSON::Object& json) :
  RPG::Color(
    json.contains("red") ? (std::uint8_t)json.get("red").number() : DefaultRed,
    json.contains("green") ? (std::uint8_t)json.get("green").number() : DefaultGreen,
    json.contains("blue") ? (std::uint8_t)json.get("blue").number() : DefaultBlue,
    json.contains("alpha") ? (std::uint8_t)json.get("alpha").number() : DefaultAlpha)
{}

bool  RPG::Color::operator==(const Color& other) const
{
  // Compare raw color value
  return raw == other.raw;
}

bool  RPG::Color::operator!=(const Color& other) const
{
  // Compare raw color value
  return raw != other.raw;
}

Game::JSON::Object  RPG::Color::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  if (red != DefaultRed)
    json.set("red", (double)red);
  if (green != DefaultGreen)
    json.set("green", (double)green);
  if (blue != DefaultBlue)
    json.set("blue", (double)blue);
  if (alpha != DefaultAlpha)
    json.set("alpha", (double)alpha);

  return json;
}