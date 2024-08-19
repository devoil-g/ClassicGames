#include "RolePlayingGame/EntityComponentSystem/Components/DisplayComponent.hpp"

RPG::DisplayComponent::DisplayComponent() :
  position(0.f, 0.f, 0.f),
  direction(RPG::Direction::DirectionNorth),
  color(RPG::Color::White),
  outline(RPG::Color::Transparent),
  model(nullptr),
  animation(nullptr),
  frame(0),
  elapsed(0.f),
  loop(false)
{}