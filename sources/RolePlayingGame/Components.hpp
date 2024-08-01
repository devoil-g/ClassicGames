#pragma once

#include "Math/Vector.hpp"
#include "RolePlayingGame/Model.hpp"

namespace RPG
{
  // COMMON COMPONENTS --------------------------------------------------------

  class BaseComponent
  {
  public:
    std::string       model;        // Entity's model name
    RPG::Coordinates  coordinates;  // Cell coordinates of entity
    RPG::Position     position;     // Position of the entity in world

    BaseComponent();
    BaseComponent(const BaseComponent&) = default;
    BaseComponent(BaseComponent &&) = default;
    ~BaseComponent() = default;

    BaseComponent& operator=(const BaseComponent&) = default;
    BaseComponent& operator=(BaseComponent&&) = default;
  };

  // SERVER COMPONENTS --------------------------------------------------------

  class ServerActionComponent
  {
  public:
    class AbstractAction
    {
    public:
      AbstractAction() = default;
      AbstractAction(const AbstractAction&) = default;
      AbstractAction(AbstractAction&&) = default;
      virtual ~AbstractAction() = 0;

      AbstractAction& operator=(const AbstractAction&) = default;
      AbstractAction& operator=(AbstractAction&&) = default;

      virtual bool execute() = 0;
    };


  };

  // CLIENT COMPONENTS --------------------------------------------------------

  class PositionComponent
  {
  public:
    Math::Vector<3, float>  position;   // Position of entity on its cell (x: screen +X, y: screen +Y, z: screen -Y)
    RPG::Direction          direction;  // Direction the entity is facing

    PositionComponent();
    PositionComponent(const PositionComponent&) = default;
    PositionComponent(PositionComponent&&) = default;
    ~PositionComponent() = default;

    PositionComponent& operator=(const PositionComponent&) = default;
    PositionComponent& operator=(PositionComponent&&) = default;
  };

  class SpriteComponent
  {
  public:
    const RPG::Sprite*  sprite;   // Sprite of the entity
    RPG::Color          color;    // Sprite color
    RPG::Color          outline;  // Outline color

    SpriteComponent();
    SpriteComponent(const SpriteComponent&) = default;
    SpriteComponent(SpriteComponent&&) = default;
    ~SpriteComponent() = default;

    SpriteComponent&  operator=(const SpriteComponent&) = default;
    SpriteComponent&  operator=(SpriteComponent&&) = default;
  };

  class AnimationComponent
  {
  public:
    const RPG::Model*     model;       // Model of the entity
    const RPG::Animation* animation;   // Current animation

    std::size_t frame;   // Current frame
    float       elapsed; // Time elapsed on current frame
    bool        loop;    // True: loop animation, false: idle when over

    AnimationComponent();
    AnimationComponent(const AnimationComponent&) = default;
    AnimationComponent(AnimationComponent&&) = default;
    ~AnimationComponent() = default;

    AnimationComponent& operator=(const AnimationComponent&) = default;
    AnimationComponent& operator=(AnimationComponent&&) = default;
  };

  class MoveComponent
  {
  public:
    Math::Vector<3, float>  position;   // Target position
    float                   remaining;  // Remaining time

    MoveComponent();
    MoveComponent(const MoveComponent&) = default;
    MoveComponent(MoveComponent&&) = default;
    ~MoveComponent() = default;

    MoveComponent&  operator=(const MoveComponent&) = default;
    MoveComponent&  operator=(MoveComponent&&) = default;
  };

  class CoordinatesComponent
  {
  public:
    Math::Vector<2, int>  coordinates;  // Coordinates of entity in level

    CoordinatesComponent();
    CoordinatesComponent(const CoordinatesComponent&) = default;
    CoordinatesComponent(CoordinatesComponent&&) = default;
    ~CoordinatesComponent() = default;

    CoordinatesComponent& operator=(const CoordinatesComponent&) = default;
    CoordinatesComponent& operator=(CoordinatesComponent&&) = default;
  };

  class ParticleComponent
  {
  public:
    Math::Vector<3> physicsSpeed;     // Particule speed vector
    Math::Vector<3> physicsGravity;   // Gravity vector, applied on speed vector
    float           physicsDrag;      // Drag factor, applied on speed vector
    float           physicsFloor;     // Floor height
    RPG::Color      colorStart;       // Start color of the particle
    RPG::Color      colorEnd;         // End color of particle
    float           durationFadeIn;   // Duration of particle fade in
    float           durationLife;     // Duration of particle life
    float           durationFadeOut;  // Duration of particle fade out

    float           elapsed;          // Elapsed time of particle

    ParticleComponent();
    ParticleComponent(const ParticleComponent&) = default;
    ParticleComponent(ParticleComponent&&) = default;
    ~ParticleComponent() = default;

    ParticleComponent& operator=(const ParticleComponent&) = default;
    ParticleComponent& operator=(ParticleComponent&&) = default;
  };

  class ParticleEmitterComponent
  {
  public:
    Math::Vector<3>         size;                         // Size of the emitter
    Math::Vector<3>         offset;                       // Offset of the emitter (added to position)
    float                   frequencyLow, frequencyHigh;  // Number of particles emitted per second, low and high frequency
    RPG::ParticleComponent  particleLow, particleHigh;    // Emitted particles properties, low and high properties
    std::string             model;                        // Name of the model of particles

    float           next;                         // Time before next particle
    float           duration;                     // Duration of the emitter

    ParticleEmitterComponent();
    ParticleEmitterComponent(const ParticleEmitterComponent&) = default;
    ParticleEmitterComponent(ParticleEmitterComponent&&) = default;
    ~ParticleEmitterComponent() = default;

    ParticleEmitterComponent& operator=(const ParticleEmitterComponent&) = default;
    ParticleEmitterComponent& operator=(ParticleEmitterComponent&&) = default;
  };

  class ActionsComponent
  {
  public:
    bool  pickup;     // Entity can be picked-up (item)
    bool  container;  // Entity can be opened (chest)
    bool  dialog;     // Entity can be talked to (dialog)
    bool  fight;      // Entity can be attacked (fighter)
    bool  trigger;    // Entity can be triggered (button)

    RPG::Bounds bounds; // Bound of the entity

    ActionsComponent();
    ActionsComponent(const ActionsComponent&) = default;
    ActionsComponent(ActionsComponent&&) = default;
    ~ActionsComponent() = default;

    ActionsComponent& operator=(const ActionsComponent&) = default;
    ActionsComponent& operator=(ActionsComponent&&) = default;
  };
}