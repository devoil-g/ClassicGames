#pragma once

#include <array>
#include <set>
#include <cstdint>
#include <string>
#include <unordered_map>

#include "Math/Vector.hpp"

namespace RPG
{
  class World
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

    static const std::array<RPG::World::Coordinates, RPG::World::Direction::DirectionCount>  Directions;  // Array of directions coordinates

    class Level
    {
      class Cell
      {
        static const std::array<std::uint8_t, DirectionCount> LinkMask; // Bitmask of cell links

        std::set<std::string> tags;   // Sector tags
        std::uint8_t          links;  // Bitmap of connected cells
      };

      class Exit
      {
        std::string level;  // Name of level destination
        std::string entry;  // Entry point in level destination
      };

      std::unordered_map<RPG::World::Coordinates, RPG::World::Level::Cell>  _cells;   // Hexagonal map, [x,y] connected to [x-1,y],[x+1,y],[x,y-1],[x,y+1],[x-1,y-1],[x+1,y+1]
      std::unordered_map<RPG::World::Coordinates, RPG::World::Level::Exit>  _exits;   // Exits destinations
      std::unordered_map<std::string, RPG::World::Coordinates>              _entries; // Entry points of the level per name
    };

    std::unordered_map<std::string, RPG::World::Level>  _levels;  // Levels per name

  public:
    World();
    World(const World&) = delete;
    ~World() = default;
  };
}