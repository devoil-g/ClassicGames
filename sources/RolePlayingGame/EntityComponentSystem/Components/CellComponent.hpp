#pragma once

#include "RolePlayingGame/Types.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class CellComponent
  {
  public:
    static const RPG::Coordinates DefaultCoordinates;
    static const float            DefaultHeight;
    static const bool             DefaultBlocked;

    RPG::Coordinates  coordinates;  // Coordinates of the cell
    float             height;       // Height of the cell
    bool              blocked;      // Cell can't be walked

    CellComponent();
    CellComponent(const Game::JSON::Object& json);
    CellComponent(const CellComponent&) = default;
    CellComponent(CellComponent&&) = default;
    ~CellComponent() = default;

    CellComponent&  operator=(const CellComponent&) = default;
    CellComponent&  operator=(CellComponent&&) = default;

    Game::JSON::Object  json() const; // Serialize to JSON
  };
}