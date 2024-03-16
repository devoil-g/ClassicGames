#include "RolePlayingGame/Types.hpp"

RPG::Bounds::Bounds(const Math::Vector<2, int>& origin, const Math::Vector<2, int>& size) :
  origin(origin),
  size(size)
{}

bool  RPG::Bounds::contains(const Math::Vector<2, int>& position) const
{
  // Check if points is contained in bounds
  return position.x() >= origin.x() && position.y() >= origin.y() && position.x() < origin.x() + size.x() && position.y() < origin.y() + size.y();
}

bool  RPG::Bounds::contains(int x, int y) const
{
  // Check if points is contained in bounds
  return contains({ x, y });
}