#include "RolePlayingGame/Components.hpp"

RPG::SpriteComponent::SpriteComponent() :
  sprite(nullptr),
  color(RPG::Color::White),
  outline(RPG::Color::Transparent)
{}

RPG::AnimationComponent::AnimationComponent() :
  model(nullptr),
  animation(nullptr),
  frame(0),
  elapsed(0.f),
  loop(false)
{}

RPG::PositionComponent::PositionComponent() :
  position(),
  direction(RPG::Direction::DirectionNorth)
{}