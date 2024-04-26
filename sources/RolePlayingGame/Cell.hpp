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
  namespace Cell
  {
    const bool DefaultBlocked = false;
    const int  DefaultHeight = 0;

    const std::size_t Width = 14;  // With of a cell in pixel
    const std::size_t Height = 12; // Height of a cell in pixel
  }

  class ClientCell
  {
  public:
    RPG::Coordinates coordinates; // Coordinates of the cell
    bool             blocked;     // True if cell can be selected
    int              height;      // Height of the cell

    RPG::Color hover;  // Cell hover color
    RPG::Color cursor; // Cell cursor color

    ClientCell() = delete;
    ClientCell(const RPG::ClientWorld& world, const RPG::ClientLevel& level, const Game::JSON::Object& json);
    ClientCell(const ClientCell&) = delete;
    ClientCell(ClientCell&&) = delete;
    ~ClientCell() = default;

    ClientCell& operator=(const ClientCell&) = delete;
    ClientCell& operator=(ClientCell&&) = delete;

    RPG::Bounds bounds(const RPG::ClientWorld& world, const RPG::ClientLevel& level) const; // Get cells bounds
  };

  class ServerCell
  {
  public:
    bool blocked; // True if cell can be selected
    int  height;  // Height of the cell

    ServerCell() = delete;
    ServerCell(const Game::JSON::Object& json);
    ServerCell(const ClientCell&) = delete;
    ServerCell(ClientCell&&) = delete;
    ~ServerCell() = default;

    ServerCell& operator=(const ServerCell&) = delete;
    ServerCell& operator=(ServerCell&&) = delete;

    Game::JSON::Object json() const;   // Serialize to JSON
  };
}