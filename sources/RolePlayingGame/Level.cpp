#include "RolePlayingGame/Level.hpp"

const std::array<RPG::Level::Coordinates, RPG::Level::Direction::DirectionCount>  RPG::Level::Directions = {
      RPG::Level::Coordinates(+1, +1),  // Up
      RPG::Level::Coordinates(+1, 0),   // UpRight
      RPG::Level::Coordinates(0, -1),   // DownRight
      RPG::Level::Coordinates(-1, -1),  // Down
      RPG::Level::Coordinates(-1, 0),   // DownLeft
      RPG::Level::Coordinates(0, +1)    // UpLeft
};