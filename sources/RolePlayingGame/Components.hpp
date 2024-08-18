#pragma once

#include "Math/Vector.hpp"
#include "RolePlayingGame/Model.hpp"
#include "RolePlayingGame/EntityComponentSystem.hpp"

namespace RPG
{
  class EntityComponent
  {
  public:
    static const RPG::Coordinates DefaultCoordinates;
    static const RPG::Direction   DefaultDirection;
    static const RPG::Position    DefaultPosition;
    static const std::string      DefaultModel;

    std::string       id;           // Identifier of entity
    RPG::Coordinates  coordinates;  // Coordinates of entity's cell
    RPG::Direction    direction;    // Direction the entity is facing
    RPG::Position     position;     // Position of entity in its cell
    std::string       model;        // Model of the entity
   
    // Characteristics
    int attack;
    int defense;

    EntityComponent();
    EntityComponent(const Game::JSON::Object& json);
    EntityComponent(const EntityComponent&) = default;
    EntityComponent(EntityComponent&&) = default;
    ~EntityComponent() = default;

    EntityComponent&  operator=(const EntityComponent&) = default;
    EntityComponent&  operator=(EntityComponent&&) = default;

    Game::JSON::Object  json() const; // Serialize to JSON
  };


  class NetworkComponent
  {
  public:
    static const std::size_t  NoController;

    std::size_t controller; // ID of network controller of entity, 0 for none

    NetworkComponent();
    NetworkComponent(const NetworkComponent&) = default;
    NetworkComponent(NetworkComponent&&) = default;
    ~NetworkComponent() = default;

    NetworkComponent& operator=(const NetworkComponent&) = default;
    NetworkComponent& operator=(NetworkComponent&&) = default;
  };

  class CellComponent
  {
  public:
    static const RPG::Coordinates DefaultCoordinates;
    static const float            DefaultHeight;
    static const bool             DefaultBlocked;

    RPG::Coordinates  coordinates;  // Coordinates of the cell
    float             height;       // Height of the cell
    bool              blocked;      // Cell can't be walked

    CellComponent();
    CellComponent(const Game::JSON::Object& json);
    CellComponent(const CellComponent&) = default;
    CellComponent(CellComponent&&) = default;
    ~CellComponent() = default;

    CellComponent&  operator=(const CellComponent&) = default;
    CellComponent&  operator=(CellComponent&&) = default;

    Game::JSON::Object  json() const; // Serialize to JSON
  };

  class DisplayComponent
  {
  public:
    RPG::Position   position;   // Position of entity in 3D world
    RPG::Direction  direction;  // Direction the entity is facing
    RPG::Color      color;      // Sprite color
    RPG::Color      outline;    // Outline color
    
    const RPG::Model*     model;      // Current model
    const RPG::Animation* animation;  // Current animation
    
    std::size_t frame;    // Current frame
    float       elapsed;  // Time elapsed on current frame
    bool        loop;     // True: loop animation, false: idle when over

    DisplayComponent();
    DisplayComponent(const DisplayComponent&) = default;
    DisplayComponent(DisplayComponent&&) = default;
    ~DisplayComponent() = default;

    DisplayComponent& operator=(const DisplayComponent&) = default;
    DisplayComponent& operator=(DisplayComponent&&) = default;
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

    float elapsed;  // Elapsed time of particle

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
    Math::Vector<3>         position;                     // Position of the emitter
    float                   frequencyLow, frequencyHigh;  // Number of particles emitted per second, low and high frequency
    RPG::ParticleComponent  particleLow, particleHigh;    // Emitted particles properties, low and high properties
    std::string             model;                        // Name of the model of particles

    float next;     // Time before next particle
    float duration; // Duration of the emitter

    ParticleEmitterComponent();
    ParticleEmitterComponent(const ParticleEmitterComponent&) = default;
    ParticleEmitterComponent(ParticleEmitterComponent&&) = default;
    ~ParticleEmitterComponent() = default;

    ParticleEmitterComponent& operator=(const ParticleEmitterComponent&) = default;
    ParticleEmitterComponent& operator=(ParticleEmitterComponent&&) = default;
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
}