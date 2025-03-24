#include "RolePlayingGame/EntityComponentSystem/Components/ParticleEmitterComponent.hpp"

RPG::ParticleEmitterComponent::ParticleEmitterComponent() :
  size(0.f, 0.f, 0.f),
  position(0.f, 0.f, 0.f),
  frequencyLow(0.f),
  frequencyHigh(0.f),
  particleLow(),
  particleHigh(),
  animation(""),
  next(0.f),
  duration(0.f)
{}