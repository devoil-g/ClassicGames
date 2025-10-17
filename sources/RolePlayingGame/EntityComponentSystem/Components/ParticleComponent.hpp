#pragma once

#include "Math/Vector.hpp"
#include "RolePlayingGame/Color.hpp"

namespace RPG
{
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
}