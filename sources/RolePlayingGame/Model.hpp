#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "RolePlayingGame/Sprite.hpp"
#include "RolePlayingGame/Texture.hpp"
#include "RolePlayingGame/Types.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class Model : public std::enable_shared_from_this<Model>
  {
  private:
    class Animation
    {
    public:
      class Frame
      {
      public:
        static const float  DefaultDuration;

        std::vector<RPG::Sprite>  sprites;  // Sprites of frame, per direction or unique
        float                     duration; // Duration of the frame in seconds (0 for infinite, default to 0)

        Frame(const Game::JSON::Object& json);
        Frame(const Frame&) = default;
        Frame(Frame&&) = default;
        ~Frame() = default;

        Frame& operator=(const Frame&) = delete;
        Frame& operator=(Frame&&) = delete;

        Game::JSON::Object  json() const; // Serialize to JSON
      };

      std::vector<Frame>  frames;   // Frames of the animation
      float               duration; // Total duration of animation

      Animation();
      Animation(const Game::JSON::Object& json);
      Animation(const Animation&) = default;
      Animation(Animation&&) = default;
      ~Animation() = default;

      Animation& operator=(const Animation&) = delete;
      Animation& operator=(Animation&&) = delete;

      Game::JSON::Object  json() const;                         // Serialize to JSON
      void                load(const Game::JSON::Object& json); // Load frames from JSON
    };

  public:
    class Actor
    {
    public:
      static const std::wstring DefaultAnimation;
      static const std::wstring IdleAnimation;
      static const std::wstring WalkAnimation;
      static const std::wstring RunAnimation;

      enum class Mode
      {
        Normal,   // Play then stop
        Loop,     // Play then repeat
        PingPong  // Play then reverse
      };

    private:
      std::reference_wrapper<RPG::Model>             _model;     // Actor's model
      std::reference_wrapper<RPG::Model::Animation>  _animation; // Current animation
      std::size_t                                    _frame;     // Current frame index
      float                                          _cursor;    // Time in current frame
      float                                          _speed;     // Speed of animation
      Mode                                           _mode;      // Animation mode

    public:
      Actor();
      Actor(RPG::Model& model);
      Actor(const Actor&) = default;
      Actor(Actor&&) = default;
      ~Actor() = default;

      Actor& operator=(const Actor&) = default;
      Actor& operator=(Actor&&) = default;

      bool  operator==(const RPG::Model& model) const;  // Check if given model is used
      bool  operator!=(const RPG::Model& model) const;  // Check if given model is not used

      void                update(float elapsed);                  // Update actor animation
      const RPG::Sprite&  sprite(RPG::Direction direction) const; // Get current sprite
      const std::wstring& icon() const;                           // Get actor's icon name

      void  setModel(RPG::Model& model);                                                          // Reset actor's model
      void  setAnimation(const std::wstring& name, Mode loop = Mode::Normal, float speed = 1.f);  // Start an animation
      void  setSpeed(float speed);                                                                // Set animation speed
      void  setMode(Mode mode);                                                                   // Set animation mode

      float getSpeed() const; // Get animation speed
      Mode  getMode() const;  // Get animation mode
    };

    friend  RPG::Model::Actor;

  private:
    static const std::wstring  DefaultIcon;

    std::unordered_map<std::wstring, RPG::Model::Animation> _animations;  // Animations stored by name
    std::wstring                                            _icon;        // Name of model's icon

  public:
    static Model  ErrorModel; // Error model

    Model();
    Model(const Game::JSON::Object& json);
    Model(const Model&) = default;
    Model(Model&&) = default;
    ~Model() = default;

    Model& operator=(const Model&) = default;
    Model& operator=(Model&&) = default;

    Game::JSON::Object  json() const;                         // Serialize to JSON
    void                load(const Game::JSON::Object& json); // Load model from JSON

    void  resolve(const std::function<const RPG::Texture& (const std::wstring&)> library);  // Resolve texture in sprites
  };
}