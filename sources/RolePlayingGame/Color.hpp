#pragma once

#include <cstdint>

#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class Color // RGBA color
  {
  public:
    static const Color      Default;            // Default color
    static constexpr float  DefaultRed = 1.f;   // Default red value
    static constexpr float  DefaultGreen = 1.f; // Default green value
    static constexpr float  DefaultBlue = 1.f;  // Default blue value
    static constexpr float  DefaultAlpha = 1.f; // Default alpha value

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