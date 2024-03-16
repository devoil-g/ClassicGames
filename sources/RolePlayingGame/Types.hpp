#pragma once

#include <cstdint>

#include "Math/Vector.hpp"

namespace RPG
{
  enum Direction : std::uint8_t
  {
    DirectionNorth, DirN = DirectionNorth,          // North
    DirectionNorthEast, DirNE = DirectionNorthEast, // North East
    DirectionSouthEast, DirSE = DirectionSouthEast, // South East
    DirectionSouth, DirS = DirectionSouth,          // South
    DirectionSouthWest, DirSW = DirectionSouthWest, // South West
    DirectionNorthWest, DirNW = DirectionNorthWest, // North West

    DirectionCount  // Number of directions
  };

  using Coordinates = Math::Vector<2, int>;
  using Position = Math::Vector<3, float>;

  const std::array<RPG::Coordinates, RPG::Direction::DirectionCount> DirectionCoordinates = { // Array of directions coordinates
    RPG::Coordinates(+1, +1), // North
    RPG::Coordinates(+1, 0),  // North East
    RPG::Coordinates(0, -1),  // South East
    RPG::Coordinates(-1, -1), // South
    RPG::Coordinates(-1, 0),  // South West
    RPG::Coordinates(0, +1)   // North West
  };

  class Bounds // Bounding box
  {
  public:
    Math::Vector<2, int> origin; // Left and top position
    Math::Vector<2, int> size;   // Width and height of bounds

    Bounds(const Math::Vector<2, int>& origin, const Math::Vector<2, int>& size);
    Bounds(const Bounds&) = default;
    Bounds(Bounds&&) = default;
    ~Bounds() = default;

    Bounds& operator=(const Bounds&) = default;
    Bounds& operator=(Bounds&&) = default;

    bool contains(const Math::Vector<2, int>& position) const; // Ckeck if position is contained by bounds
    bool contains(int x, int y) const;                         // Ckeck if position is contained by bounds
  };
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