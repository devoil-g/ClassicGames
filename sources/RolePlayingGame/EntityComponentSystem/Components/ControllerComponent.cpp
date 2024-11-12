#include "RolePlayingGame/EntityComponentSystem/Components/ControllerComponent.hpp"

const std::size_t RPG::ControllerComponent::NoController(0);

RPG::ControllerComponent::ControllerComponent() :
  controller(NoController)
{}