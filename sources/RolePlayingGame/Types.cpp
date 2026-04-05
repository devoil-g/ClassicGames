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