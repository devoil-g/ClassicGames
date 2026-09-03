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

std::wstring  RPG::ActionModeToString(RPG::ActionMode mode)
{
  // Check range
  return std::wstring(ActionModeNames.at(static_cast<unsigned int>(mode)));
}

RPG::ActionMode  RPG::StringToActionMode(const std::wstring& string)
{
  auto pos = std::distance(ActionModeNames.begin(), std::find(ActionModeNames.begin(), ActionModeNames.end(), string));

  // Check range
  if (pos >= ActionModeNames.size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  else
    return static_cast<RPG::ActionMode>(pos);
}