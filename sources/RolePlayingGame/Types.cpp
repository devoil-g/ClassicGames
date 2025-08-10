#include "RolePlayingGame/Types.hpp"

std::wstring  RPG::DirectionToString(RPG::Direction direction)
{
  // Get direction name
  return std::wstring(RPG::DirectionNames.at(direction));
}

RPG::Direction  RPG::StringToDirection(const std::wstring& name)
{
  // Find matching direction name
  for (int direction = 0; direction < RPG::Direction::DirectionCount; direction++)
    if (name == RPG::DirectionNames[direction])
      return (RPG::Direction)direction;

  // Error
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

RPG::Bounds::Bounds(const Math::Vector<2>& origin, const Math::Vector<2>& size) :
  origin(origin),
  size(size)
{}

bool  RPG::Bounds::contains(const Math::Vector<2>& position) const
{
  // Check if points is contained in bounds
  return position.x() >= origin.x() && position.y() >= origin.y() && position.x() < origin.x() + size.x() && position.y() < origin.y() + size.y();
}

bool  RPG::Bounds::contains(float x, float y) const
{
  // Check if points is contained in bounds
  return contains({ x, y });
}