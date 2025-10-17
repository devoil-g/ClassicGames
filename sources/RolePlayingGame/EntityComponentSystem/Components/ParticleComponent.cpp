#include "RolePlayingGame/EntityComponentSystem/Components/ParticleComponent.hpp"

RPG::ParticleComponent::ParticleComponent() :
  physicsSpeed(0.f, 0.f, 0.f),
  physicsGravity(0.f, 0.f, 0.f),
  physicsDrag(0.f),
  physicsFloor(0.f),
  colorStart(RPG::Color::White),
  colorEnd(RPG::Color::White),
  durationFadeIn(0.f),
  durationLife(0.f),
  durationFadeOut(0.f),
  elapsed(0.f)
{}