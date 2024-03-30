#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

#include "Math/Vector.hpp"
#include "RolePlayingGame/Color.hpp"
#include "RolePlayingGame/Texture.hpp"
#include "RolePlayingGame/Types.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class Model // Animations for an entity
  {
  public:
    class Animation // Frames of an animation
    {
    public:
      class Frame // A single frame of animation
      {
      public:
        static const Math::Vector<2, unsigned int> DefaultOrigin;
        static const Math::Vector<2, unsigned int> DefaultSize;
        static const Math::Vector<2, int>          DefaultOffset;
        static const bool                          DefaultFlipX;
        static const bool                          DefaultFlipY;
        static const RPG::Color                    DefaultColor;
        static const float                         DefaultDuration;

        Math::Vector<2, unsigned int> origin;       // Origin of texture rectangle
        Math::Vector<2, unsigned int> size;         // Size of texture rectangle (full texture if 0,0)
        Math::Vector<2, int>          offset;       // Offset of the sprite from drawing position
        bool                          flipX, flipY; // Texture horizontal and vertical flip
        RPG::Color                    color;        // Sprite color
        float                         duration;     // Duration of the frame in seconds (0 for infinite, default to 0)

        Frame() = delete;
        Frame(const Game::JSON::Object& json);
        Frame(const Frame&) = default;
        Frame(Frame&&) = default;
        ~Frame() = default;

        Frame& operator=(const Frame&) = default;
        Frame& operator=(Frame&&) = default;

        Game::JSON::Object json() const;                                                                    // Serialize to JSON
        void               draw(const RPG::Texture& texture, const Math::Vector<2, float>& position) const; // Draw frame at position
        RPG::Bounds        bounds() const;                                                                  // Get bounds of frame
      };

      std::vector<Frame> frames; // Frames of the animation

      Animation() = delete;
      Animation(const Game::JSON::Object& json);
      Animation(const Animation&) = default;
      Animation(Animation&&) = default;
      ~Animation() = default;

      Animation& operator=(const Animation&) = default;
      Animation& operator=(Animation&&) = default;

      Game::JSON::Object json() const; // Serialize to JSON
    };

    static const std::string DefaultAnimation;
    static const std::string IdleAnimation;
    static const std::string WalkAnimation;
    static const std::string RunAnimation;

    std::string                                spritesheet; // Path to spritesheet used for animation
    std::unordered_map<std::string, Animation> animations;  // Animations of the entity, by name

    Model() = delete;
    Model(const Game::JSON::Object& json);
    Model(const Model&) = default;
    Model(Model&&) = default;
    ~Model() = default;

    Model& operator=(const Model&) = default;
    Model& operator=(Model&&) = default;

    RPG::Model::Animation&       animation(const std::string& name);
    const RPG::Model::Animation& animation(const std::string& name) const;

    Game::JSON::Object json() const; // Serialize to JSON
  };
}