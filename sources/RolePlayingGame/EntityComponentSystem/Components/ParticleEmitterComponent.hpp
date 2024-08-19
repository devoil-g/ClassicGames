#pragma once

#include <string>

#include "Math/Vector.hpp"
#include "RolePlayingGame/EntityComponentSystem/Components/ParticleComponent.hpp"

namespace RPG
{
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
}