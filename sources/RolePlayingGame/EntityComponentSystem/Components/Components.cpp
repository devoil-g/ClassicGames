#include "RolePlayingGame/EntityComponentSystem/Components/Components.hpp"

RPG::ActionComponent::ActionComponent() :
  mode(Mode::Wait),
  wait(0.f)
{}

RPG::ServerActionComponent::ServerActionComponent() :
  ActionComponent(),
  action(),
  next()
{}

RPG::ServerActionComponent::Action::Action(RPG::ECS& ecs, RPG::ECS::Entity self) :
  ecs(ecs),
  self(self)
{}

RPG::ClientActionComponent::ClientActionComponent() :
  action(),
  next()
{}

RPG::ClientActionComponent::Action::Action(RPG::ECS& ecs, RPG::ECS::Entity self, std::size_t index) :
  ecs(ecs),
  self(self),
  index(index)
{}