#pragma once

#include <cstdint>

#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class Color // RGBA color
  {
  public:
    static const Color        Default;      // Default color
    static const std::uint8_t DefaultRed;   // Default red value
    static const std::uint8_t DefaultGreen; // Default green value
    static const std::uint8_t DefaultBlue;  // Default blue value
    static const std::uint8_t DefaultAlpha; // Default alpha value

    union {
      std::uint8_t  red, green, blue, alpha; // Color components
      std::uint32_t raw;                     // Raw color
    };
    
    Color(std::uint8_t red = DefaultRed, std::uint8_t green = DefaultGreen, std::uint8_t blue = DefaultBlue, std::uint8_t alpha = DefaultAlpha);
    Color(const Game::JSON::Object& json);
    Color(const Color&) = default;
    Color(Color&&) = default;
    ~Color() = default;

    Color& operator=(const Color&) = default;
    Color& operator=(Color&&) = default;
    bool operator==(const Color& other) const;
    bool operator!=(const Color& other) const;

    Game::JSON::Object  json() const; // Serialize to JSON
  };
}