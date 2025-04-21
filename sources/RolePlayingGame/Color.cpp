#include <algorithm>

#include "RolePlayingGame/Color.hpp"

const RPG::Color  RPG::Color::Default = RPG::Color(DefaultRed, DefaultGreen, DefaultBlue, DefaultAlpha);
const float       RPG::Color::DefaultRed = 1.f;
const float       RPG::Color::DefaultGreen = 1.f;
const float       RPG::Color::DefaultBlue = 1.f;
const float       RPG::Color::DefaultAlpha = 1.f;

const RPG::Color    RPG::Color::White = RPG::Color(1.f, 1.f, 1.f);
const RPG::Color    RPG::Color::Black = RPG::Color(0.f, 0.f, 0.f);
const RPG::Color    RPG::Color::Red = RPG::Color(1.f, 0.f, 0.f);
const RPG::Color    RPG::Color::Green = RPG::Color(0.f, 1.f, 0.f);
const RPG::Color    RPG::Color::Blue = RPG::Color(0.f, 0.f, 1.f);
const RPG::Color    RPG::Color::Yellow = RPG::Color(1.f, 1.f, 0.f);
const RPG::Color    RPG::Color::Magenta = RPG::Color(1.f, 0.f, 1.f);
const RPG::Color    RPG::Color::Cyan = RPG::Color(0.f, 1.f, 1.f);
const RPG::Color    RPG::Color::Transparent = RPG::Color(1.f, 1.f, 1.f, 0.f);

RPG::Color::Color() :
  Color(Default)
{}

RPG::Color::Color(float red, float green, float blue, float alpha) :
  red(red), green(green), blue(blue), alpha(alpha)
{}

RPG::Color::Color(const Game::JSON::Object& json) :
  RPG::Color(
    json.contains(L"red") ? (std::uint8_t)json.get(L"red").number() : DefaultRed,
    json.contains(L"green") ? (std::uint8_t)json.get(L"green").number() : DefaultGreen,
    json.contains(L"blue") ? (std::uint8_t)json.get(L"blue").number() : DefaultBlue,
    json.contains(L"alpha") ? (std::uint8_t)json.get(L"alpha").number() : DefaultAlpha
  )
{}

RPG::Color RPG::Color::operator+(const RPG::Color& other) const { return RPG::Color(red + other.red, green + other.green, blue + other.blue, alpha + other.alpha); }
RPG::Color RPG::Color::operator-(const RPG::Color& other) const { return RPG::Color(red - other.red, green - other.green, blue - other.blue, alpha - other.alpha); }
RPG::Color RPG::Color::operator*(const RPG::Color& other) const { return RPG::Color(red * other.red, green * other.green, blue * other.blue, alpha * other.alpha); }
RPG::Color RPG::Color::operator*(float factor) const { return RPG::Color(red * factor, green * factor, blue * factor, alpha * factor); }
RPG::Color RPG::Color::operator/(const RPG::Color& other) const { return RPG::Color(red / other.red, green / other.green, blue / other.blue, alpha / other.alpha); }
RPG::Color RPG::Color::operator/(float factor) const { return RPG::Color(red / factor, green / factor, blue / factor, alpha / factor); }

RPG::Color& RPG::Color::operator+=(const RPG::Color& other) { red += other.red; green += other.green; blue += other.blue; alpha += other.alpha; return *this; }
RPG::Color& RPG::Color::operator-=(const RPG::Color& other) { red -= other.red; green -= other.green; blue -= other.blue; alpha -= other.alpha; return *this; }
RPG::Color& RPG::Color::operator*=(const RPG::Color& other) { red *= other.red; green *= other.green; blue *= other.blue; alpha *= other.alpha; return *this; }
RPG::Color& RPG::Color::operator*=(float factor) { red *= factor; green *= factor; blue *= factor; alpha *= factor; return *this; }
RPG::Color& RPG::Color::operator/=(const RPG::Color& other) { red /= other.red; green /= other.green; blue /= other.blue; alpha /= other.alpha; return *this; }
RPG::Color& RPG::Color::operator/=(float factor) { red /= factor; green /= factor; blue /= factor; alpha /= factor; return *this; }

Game::JSON::Object  RPG::Color::json() const
{
  Game::JSON::Object  json;

  // Serialize to JSON
  if (red != DefaultRed)
    json.set(L"red", (double)red);
  if (green != DefaultGreen)
    json.set(L"green", (double)green);
  if (blue != DefaultBlue)
    json.set(L"blue", (double)blue);
  if (alpha != DefaultAlpha)
    json.set(L"alpha", (double)alpha);
  sizeof(wchar_t);
  return json;
}

std::uint32_t RPG::Color::uint32() const
{
  // Convert to 32bits integers RGBA
  return (((std::uint32_t)std::clamp(red * 255.f, 0.f, 255.f)) << 24)
    | (((std::uint32_t)std::clamp(green * 255.f, 0.f, 255.f)) << 16)
    | (((std::uint32_t)std::clamp(blue * 255.f, 0.f, 255.f)) << 8)
    | (((std::uint32_t)std::clamp(alpha * 255.f, 0.f, 255.f)) << 0);
}