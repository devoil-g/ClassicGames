#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

#include "RolePlayingGame/Sprite.hpp"
#include "RolePlayingGame/Texture.hpp"
#include "RolePlayingGame/Types.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class Icon
  {
  private:
    class Animation
    {
    public:
      class Frame
      {
      public:
        static const float  DefaultDuration;

        RPG::Sprite sprite;   // Sprites of frame
        float       duration; // Duration of the frame in seconds (0 for infinite, default to 0)

        Frame(const Game::JSON::Object& json);
        Frame(const Frame&) = default;
        Frame(Frame&&) = default;
        ~Frame() = default;

        Frame& operator=(const Frame&) = delete;
        Frame& operator=(Frame&&) = delete;

        Game::JSON::Object  json() const; // Serialize to JSON
      };

      std::vector<Frame>  frames;  // Frames of the animation

      Animation(const Game::JSON::Object& json);
      Animation(const Animation&) = default;
      Animation(Animation&&) = default;
      ~Animation() = default;

      Animation& operator=(const Animation&) = delete;
      Animation& operator=(Animation&&) = delete;

      Game::JSON::Object  json() const; // Serialize to JSON
    };

  public:
    class Actor
    {
    public:
      static const std::string DefaultAnimation;

    private:
      const RPG::Icon*            _icon;      // Actor's icon
      const RPG::Icon::Animation* _animation; // Current animation
      std::size_t                 _frame;     // Current frame index
      float                       _duration;  // Remaining time of current frame
      float                       _speed;     // Speed of animation
      bool                        _loop;      // Animation looping flag

    public:
      Actor();
      Actor(const RPG::Icon& icon);
      Actor(const Actor&) = default;
      Actor(Actor&&) = default;
      ~Actor() = default;

      Actor& operator=(const Actor&) = default;
      Actor& operator=(Actor&&) = default;

      bool  operator==(const RPG::Icon& icon);  // Check if given model is used
      bool  operator!=(const RPG::Icon& icon);  // Check if given model is not used

      void                update(float elapsed);  // Update actor animation
      const RPG::Sprite&  sprite() const;         // Get current sprite

      void  setIcon(const RPG::Icon& model);                                              // Reset actor's model
      void  setAnimation(const std::string& name, bool loop = false, float speed = 1.f);  // Start an animation
      void  setAnimationRandom(bool loop = false, float speed = 1.f);                     // Start a random animation (used for particles)
    };

    friend  RPG::Icon::Actor;

  private:
    std::unordered_map<std::string, RPG::Icon::Animation> _animations;  // Animations stored by direction/name

    Icon();

  public:
    static const Icon ErrorIcon; // Error model

    Icon(const Game::JSON::Object& json);
    Icon(const Icon&) = default;
    Icon(Icon&&) = default;
    ~Icon() = default;

    Icon& operator=(const Icon&) = default;
    Icon& operator=(Icon&&) = default;

    Game::JSON::Object  json() const; // Serialize to JSON

    void  resolve(const std::function<const RPG::Texture& (const std::string&)> library); // Resolve texture in sprites
  };
}