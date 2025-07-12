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
  actions()
{}

std::size_t RPG::ClientActionComponent::Action::IndexGenerator = 0;

RPG::ClientActionComponent::Action::Action(RPG::ECS& ecs, RPG::ECS::Entity self) :
  index(++IndexGenerator),
  ecs(ecs),
  self(self)
{}