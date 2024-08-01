#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

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
  public:
    static const Sprite ErrorSprite;  // Error sprite

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
      static const sf::Shader* Get(RPG::Color color = RPG::Color(0, 0, 0, 255));  // Get SFML shader for outline
    };

    Sprite();

  public:
    static const Math::Vector<2, std::int16_t>  DefaultOffset;
    static const Math::Vector<2, std::int16_t>  DefaultSize;
    static const Math::Vector<2, std::int16_t>  DefaultOrigin;
    static const bool                           DefaultFlipX;
    static const bool                           DefaultFlipY;
    static const RPG::Color                     DefaultColor;
    static const std::string                    DefaultTexture;

    Math::Vector<2, std::int16_t> offset;       // Offset of texture rectangle
    Math::Vector<2, std::int16_t> size;         // Size of texture rectangle (full texture if 0,0)
    Math::Vector<2, std::int16_t> origin;       // Origin of the sprite from drawing position
    bool                          flipX, flipY; // Texture horizontal and vertical flip
    RPG::Color                    color;        // Sprite color
    std::string                   path;         // Path of the texture
    const RPG::Texture*           texture;      // Pointer to texture

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

  class Frame
  {
  public:
    static const Frame  ErrorFrame; // Error frame

  private:
    Frame();

  public:
    static const std::array<RPG::Sprite, RPG::Direction::DirectionCount>  DefaultSprites;
    static const float                                                    DefaultDuration;

    std::array<RPG::Sprite, RPG::Direction::DirectionCount> sprites;  // Sprites of frame, per direction
    float                                                   duration; // Duration of the frame in seconds (0 for infinite, default to 0)

    Frame(const Game::JSON::Object& json);
    Frame(const Frame&) = default;
    Frame(Frame&&) = default;
    ~Frame() = default;

    Frame& operator=(const Frame&) = default;
    Frame& operator=(Frame&&) = default;

    Game::JSON::Object  json() const; // Serialize to JSON

    void        draw(RPG::Direction direction = RPG::Direction::DirectionNorth, const Math::Vector<2>& position = { 0.f, 0.f }, RPG::Color outline = RPG::Color(0, 0, 0, 0)) const; // Draw frame at position
    RPG::Bounds bounds(RPG::Direction direction = RPG::Direction::DirectionNorth, const Math::Vector<2>& position = { 0.f, 0.f }) const;                                            // Get bounds of frame
  };

  class Animation
  {
  public:
    static const Animation  ErrorAnimation; // Error animation

  private:
    std::vector<Frame>  _frames;  // Frames of the animation

    Animation();

  public:
    static const std::string DefaultAnimation;
    static const std::string IdleAnimation;
    static const std::string WalkAnimation;
    static const std::string RunAnimation;

    Animation(const Game::JSON::Object& json);
    Animation(const Animation&) = default;
    Animation(Animation&&) = default;
    ~Animation() = default;

    Animation& operator=(const Animation&) = default;
    Animation& operator=(Animation&&) = default;

    Game::JSON::Object  json() const; // Serialize to JSON

    std::size_t       count() const;                  // Get the number of frames in animation
    const RPG::Frame& frame(std::size_t index) const; // Get frame of animation
  };

  class Model
  {
  public:
    static const Model  ErrorModel; // Error model

  private:
    std::unordered_map<std::string, RPG::Animation> _animations;  // Animations stored by direction/name

    Model();

  public:
    Model(const Game::JSON::Object& json);
    Model(const Model&) = default;
    Model(Model&&) = default;
    ~Model() = default;

    Model& operator=(const Model&) = default;
    Model& operator=(Model&&) = default;

    Game::JSON::Object  json() const; // Serialize to JSON

    const RPG::Animation& animation(const std::string& name) const; // Get an animation of model
    const RPG::Animation& random() const;                           // Get a random animation of model
  };
}