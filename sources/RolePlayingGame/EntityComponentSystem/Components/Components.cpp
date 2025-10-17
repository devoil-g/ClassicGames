#include <algorithm>
#include <stdexcept>

#include "RolePlayingGame/EntityComponentSystem/Components/Components.hpp"

RPG::ActionComponent::Mode  RPG::ActionComponent::StringToMode(const std::wstring& string)
{
  auto pos = std::distance(ModeNames.begin(), std::find(ModeNames.begin(), ModeNames.end(), string));

  // Check range
  if (pos >= ModeNames.size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  else
    return static_cast<Mode>(pos);
}

std::wstring  RPG::ActionComponent::ModeToString(RPG::ActionComponent::Mode mode)
{
  // Check range
  return std::wstring(ModeNames.at(static_cast<unsigned int>(mode)));
}

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