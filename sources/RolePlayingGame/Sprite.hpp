#pragma once

#include <cstdint>
#include <string>

#include <SFML/Graphics/Shader.hpp>

#include "Math/Vector.hpp"
#include "RolePlayingGame/Color.hpp"
#include "RolePlayingGame/Texture.hpp"
#include "RolePlayingGame/Types.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class Sprite
  {
  private:
    class OutlineShader
    {
    private:
      sf::Shader _shader; // SFML shader

      OutlineShader();
      OutlineShader(const OutlineShader&) = delete;
      OutlineShader(OutlineShader&&) = delete;
      ~OutlineShader() = default;

      OutlineShader& operator=(const OutlineShader&) = delete;
      OutlineShader& operator=(OutlineShader&&) = delete;

    public:
      static const sf::Shader* Get(RPG::Color color = RPG::Color::Black);  // Get SFML shader for outline
    };

  public:
    static const Math::Vector<2, std::int16_t>  DefaultOffset;
    static const Math::Vector<2, std::int16_t>  DefaultSize;
    static const Math::Vector<2, std::int16_t>  DefaultOrigin;
    static const Math::Vector<2, std::int16_t>  DefaultScale;
    static const RPG::Color                     DefaultColor;
    static const std::string                    DefaultTexture;

    static const Sprite ErrorSprite;  // Error sprite

    Math::Vector<2, std::int16_t> offset;       // Offset of texture rectangle
    Math::Vector<2, std::int16_t> size;         // Size of texture rectangle (full texture if 0,0)
    Math::Vector<2, std::int16_t> origin;       // Origin of the sprite from drawing position
    Math::Vector<2, std::int16_t> scale;        // Scaling of sprite
    RPG::Color                    color;        // Sprite color
    std::string                   path;         // Path of the texture
    const RPG::Texture*           texture;      // Pointer to texture

    Sprite();
    Sprite(const Game::JSON::Object& json);
    Sprite(const Sprite&) = default;
    Sprite(Sprite&&) = default;
    ~Sprite() = default;

    Sprite& operator=(const Sprite&) = default;
    Sprite& operator=(Sprite&&) = default;
    bool    operator==(const Sprite&) const = default;

    Game::JSON::Object  json() const; // Serialize to JSON

    void        draw(const Math::Vector<2>& position = { 0.f, 0.f }, RPG::Color color = RPG::Color::White, RPG::Color outline = RPG::Color::Transparent) const; // Draw frame at position
    RPG::Bounds bounds(const Math::Vector<2>& position = { 0.f, 0.f }) const;                                                                                   // Get bounds of frame
  };
}