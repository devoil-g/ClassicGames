#include "RolePlayingGame/EntityComponentSystem/Components/NetworkComponent.hpp"

const std::size_t RPG::NetworkComponent::NoController(0);

RPG::NetworkComponent::NetworkComponent() :
  controller(NoController)
{}