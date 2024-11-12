#include "RolePlayingGame/EntityComponentSystem/Components/ModelComponent.hpp"

RPG::ModelComponent::ModelComponent() :
  actor(),
  position(0.f, 0.f, 0.f),
  direction(RPG::Direction::DirectionNorth),
  color(RPG::Color::White),
  outline(RPG::Color::Transparent)
{}