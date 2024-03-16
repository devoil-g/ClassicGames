#pragma once

#include <cstdint>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "RolePlayingGame/Color.hpp"
#include "RolePlayingGame/Entity.hpp"
#include "RolePlayingGame/Types.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class Level
  {
  public:
    class Cell
    {
    public:
      static const std::size_t  Width = 14;   // With of a cell in pixel
      static const std::size_t  Height = 12;  // Height of a cell in pixel

      static const bool DefaultSelectable;
      static const int  DefaultHeight;

      bool                            selectable; // True if cell can be selected (default to false)
      int                             height;     // Height of the cell (default to 0)
      std::unordered_set<std::string> tags;       // Tags of the cell

      Cell() = delete;
      Cell(const Game::JSON::Object& json);
      Cell(const Cell&) = default;
      Cell(Cell&&) = default;
      ~Cell() = default;

      Cell& operator=(const Cell&) = default;
      Cell& operator=(Cell&&) = default;

      Game::JSON::Object json() const;   // Serialize to JSON
      RPG::Bounds        bounds() const; // Get cells bounds
    };

  private:
    static const Cell InvalidCell; // Empty cell used when adressing invalid coordinates

  public:
    static const RPG::Color  DefaultColor;
    static const std::string DefaultBackground;
    static const std::string DefaultForeground;

    std::unordered_map<RPG::Coordinates, Cell> cells;      // Cells of the level
    std::list<RPG::Entity>                     entities;   // Entities of the level
    RPG::Color                                 color;      // Background color
    std::string                                background; // Model name of background, displayed behind everything (every animations displayed at position [0,0])
    std::string                                foreground; // Model name of foreground, displayed in front of everything (every animations displayed at position [0,0])

    Level();
    Level(const Game::JSON::Object& json);
    Level(const Level&) = default;
    Level(Level&&) = default;
    ~Level() = default;

    Level&  operator=(const Level&) = default;
    Level&  operator=(Level&&) = default;

    Game::JSON::Object json() const; // Serialize to JSON

    Cell& getCell(const RPG::Coordinates& coordinates);
    Cell& getCell(int x, int y);
    const Cell& getCell(const RPG::Coordinates& coordinates) const;
    const Cell& getCell(int x, int y) const;
  };
}