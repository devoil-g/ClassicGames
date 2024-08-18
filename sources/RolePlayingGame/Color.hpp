#pragma once

#include <cstdint>

#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class Color // RGBA color
  {
  public:
    static const Color  Default;      // Default color
    static const float  DefaultRed;   // Default red value
    static const float  DefaultGreen; // Default green value
    static const float  DefaultBlue;  // Default blue value
    static const float  DefaultAlpha; // Default alpha value

    static const Color  White, Black, Red, Green, Blue, Yellow, Magenta, Cyan, Transparent; // Default color template

    float red, green, blue, alpha;  // Color components

    Color();
    Color(float red, float green, float blue, float alpha = DefaultAlpha);
    Color(const Game::JSON::Object& json);
    Color(const Color&) = default;
    Color(Color&&) = default;
    ~Color() = default;

    // Comparison operators
    Color&  operator=(const Color&) = default;
    Color&  operator=(Color&&) = default;
    bool    operator==(const Color& other) const = default;
    bool    operator!=(const Color& other) const = default;

    // Arithmetic operators
    Color operator+(const Color& other) const;
    Color operator-(const Color& other) const;
    Color operator*(const Color& other) const;
    Color operator*(float factor) const;
    Color operator/(const Color& other) const;
    Color operator/(float factor) const;
    
    // Assignment/arithmetic operators
    Color&  operator+=(const Color& other);
    Color&  operator-=(const Color& other);
    Color&  operator*=(const Color& other);
    Color&  operator*=(float factor);
    Color&  operator/=(const Color& other);
    Color&  operator/=(float factor);

    Game::JSON::Object  json() const; // Serialize to JSON
    std::uint32_t       uint32() const;
  };
}