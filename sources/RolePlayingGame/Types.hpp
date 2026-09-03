#pragma once

#include <array>
#include <cstdint>

#include "Math/Vector.hpp"

namespace RPG
{
  enum class ActionMode {
    Command,  // Start an action
    Execute   // Executing an action
  };

  static constexpr std::array<std::wstring_view, 3> ActionModeNames = {
    L"command",
    L"action"
  };

  std::wstring    ActionModeToString(RPG::ActionMode actionMode); // Action mode to string
  RPG::ActionMode StringToActionMode(const std::wstring& name);   // String to Action mode

  enum Direction : std::uint8_t
  {
    DirectionNorth, DirN = DirectionNorth,          // North
    DirectionNorthEast, DirNE = DirectionNorthEast, // North East
    DirectionSouthEast, DirSE = DirectionSouthEast, // South East
    DirectionSouth, DirS = DirectionSouth,          // South
    DirectionSouthWest, DirSW = DirectionSouthWest, // South West
    DirectionNorthWest, DirNW = DirectionNorthWest, // North West

    DirectionNone, DirNone = DirectionNone, // No directions
    DirectionCount = DirNone                // Number of directions
  };
  
  constexpr std::array<std::wstring_view, RPG::Direction::DirectionCount + 1> DirectionNames = {
    L"N", L"NE", L"SE", L"S", L"SW", L"NW", L"None"
  };

  std::wstring   DirectionToString(RPG::Direction direction); // Direction to string
  RPG::Direction StringToDirection(const std::wstring& name); // String to direction

  using Coordinates = Math::Vector<2, int>;
  using Position = Math::Vector<3, float>;

  const std::array<RPG::Coordinates, RPG::Direction::DirectionCount + 1> DirectionCoordinates = { // Array of directions coordinates
    RPG::Coordinates(+1, +1), // North
    RPG::Coordinates(+1, 0),  // North East
    RPG::Coordinates(0, -1),  // South East
    RPG::Coordinates(-1, -1), // South
    RPG::Coordinates(-1, 0),  // South West
    RPG::Coordinates(0, +1),  // North West
    RPG::Coordinates(0, 0)    // None
  };

  const Math::Vector<2> CellSize = { 18.f, 12.f };
  const Math::Vector<3> CellOffset = { 18.f, 6.f, 12.f };
}

namespace std
{
  template<>
  struct hash<RPG::Coordinates>
  {
    std::size_t operator()(const RPG::Coordinates& c) const
    {
      return *((std::size_t*)&c);
    }
  };
}