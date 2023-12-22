#include "RolePlayingGame/World.hpp"

const std::array<std::uint8_t, RPG::World::Direction::DirectionCount> RPG::World::Level::Cell::LinkMask = {
  0b00000001, // North
  0b00000010, // North East
  0b00000100, // South East
  0b00001000, // South
  0b00010000, // South West
  0b00100000  // North West
};

const std::array<RPG::World::Coordinates, RPG::World::Direction::DirectionCount>  RPG::World::Directions = {
  RPG::World::Coordinates(+1, +1),  // North
  RPG::World::Coordinates(+1, 0),   // North East
  RPG::World::Coordinates(0, -1),   // South East
  RPG::World::Coordinates(-1, -1),  // South
  RPG::World::Coordinates(-1, 0),   // South West
  RPG::World::Coordinates(0, +1)    // North West
};