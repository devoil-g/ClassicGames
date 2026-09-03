#include <algorithm>
#include <stdexcept>

#include "RolePlayingGame/EntityComponentSystem/Components/Components.hpp"

RPG::ServerActionComponent::ServerActionComponent() :
  mode(RPG::ActionMode::Command),
  start(1.f),
  end(1.f),
  progress(1.f),
  speed(0.f),
  action(),
  next()
{}

RPG::ServerActionComponent::Action::Action(RPG::ECS& ecs, RPG::ECS::Entity self) :
  ecs(ecs),
  self(self)
{}

RPG::ClientActionComponent::ClientActionComponent() :
  mode(RPG::ActionMode::Command),
  start(1.f),
  end(1.f),
  progress(1.f),
  speed(0.f),
  action(),
  next()
{}

RPG::ClientActionComponent::Action::Action(RPG::ECS& ecs, RPG::ECS::Entity self) :
  ecs(ecs),
  self(self)
{}

RPG::ClientActionComponent::NextAction::NextAction(float clock, std::function<std::unique_ptr<Action>()>&& builder) :
  clock(clock),
  builder(std::move(builder))
{}