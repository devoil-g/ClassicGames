#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

#include "Math/Vector.hpp"

namespace RPG
{
  class Level
  {
  private:
    using Coordinates = Math::Vector<2, int>;

    enum Direction
    {
      DirectionNorth, DirN = DirectionNorth,          // North
      DirectionNorthEast, DirNE = DirectionNorthEast, // North East
      DirectionSouthEast, DirSE = DirectionSouthEast, // South East
      DirectionSouth, DirS = DirectionSouth,          // South
      DirectionSouthWest, DirSW = DirectionSouthWest, // South West
      DirectionNorthWest, DirNW = DirectionNorthWest, // North West

      DirectionCount  // Number of directions
    };

    static const std::array<RPG::Level::Coordinates, RPG::Level::Direction::DirectionCount>  Directions;  // Array of directions coordinates

    class Cell
    {
      std::int16_t  height;   // Height of the cell
      std::uint16_t exit;     // Index of exit in exit array (0xFFFF when no exit)
      bool          blocked;  // 
    };

    std::unordered_map<RPG::Level::Coordinates, RPG::Level::Cell> _cells; // Hexagonal map, [x,y] connected to [x-1,y],[x+1,y],[x,y-1],[x,y+1],[x-1,y-1],[x+1,y-1]

    class Exit
    {
      std::string level;  // Name of level destination
      std::string entry;  // Entry point in level destination
    };

    std::unordered_map<std::string, RPG::Level::Coordinates>  _entries; // Entry points of the level per name
    std::vector<RPG::Level::Exit>                             _exits;   // Array of exit destinations, index is given in Cell::exit

  public:
    Level();
    Level(const Level&) = delete;
    ~Level() = default;
  };
}