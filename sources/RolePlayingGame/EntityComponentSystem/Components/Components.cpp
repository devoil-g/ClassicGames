#include "RolePlayingGame/EntityComponentSystem/Components/Components.hpp"

RPG::ActionComponent::ActionComponent() :
  action(),
  next(),
  mode(Mode::Wait),
  wait(0.f)
{}