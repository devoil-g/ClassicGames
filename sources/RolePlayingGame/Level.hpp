#pragma once

#include <cstdint>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "RolePlayingGame/Camera.hpp"
#include "RolePlayingGame/Cell.hpp"
#include "RolePlayingGame/Color.hpp"
#include "RolePlayingGame/Entity.hpp"
#include "RolePlayingGame/Types.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class ClientWorld;
  
  namespace Level
  {
    const RPG::Color  DefaultColor = RPG::Color::Default;
  }

  class ClientLevel
  {
  private:
    void updateEntity(RPG::ClientWorld& world, const Game::JSON::Object& json); // Update level entity from JSON

  public:
    std::string name;   // Name of the level
    RPG::Color  color;  // Background color
    RPG::Camera camera; // Player's camera
    
    std::list<RPG::ClientCell>   cells;    // Cells of the level
    std::list<RPG::ClientEntity> entities; // Entities of the level

    ClientLevel() = delete;
    ClientLevel(RPG::ClientWorld& world, const Game::JSON::Object& json);
    ClientLevel(const ClientLevel&) = delete;
    ClientLevel(ClientLevel&&) = delete;
    ~ClientLevel() = default;

    ClientLevel& operator=(const ClientLevel&) = delete;
    ClientLevel& operator=(ClientLevel&&) = delete;

    RPG::ClientCell&       cell(const RPG::Coordinates& coordinates);
    RPG::ClientCell&       cell(int x, int y);
    const RPG::ClientCell& cell(const RPG::Coordinates& coordinates) const;
    const RPG::ClientCell& cell(int x, int y) const;

    RPG::ClientEntity&       entity(std::size_t id);
    const RPG::ClientEntity& entity(std::size_t id) const;

    Math::Vector<3> position(const RPG::ClientEntity& entity) const;                                                       // Compute entity world position in level
    Math::Vector<3> position(const RPG::Coordinates& coordinate, const RPG::Position& position = { 0.f, 0.f, 0.f }) const; // Compute world position in level

    void update(RPG::ClientWorld& world, const Game::JSON::Object& json); // Update level from JSON

    void update(RPG::ClientWorld& world, float elapsed); // Update level
    void draw(const RPG::ClientWorld& world) const;      // Draw level
  };
  
  class ServerWorld;

  class ServerLevel
  {
  private:
    static const RPG::ServerCell InvalidCell; // Empty cell used when adressing invalid coordinates

  public:
    std::string name;  // Name of the level
    RPG::Color  color; // Background color

    std::unordered_map<RPG::Coordinates, RPG::ServerCell> cells;    // Cells of the level
    std::list<RPG::ServerEntity>                          entities; // Entities of the level

    ServerLevel() = delete;
    ServerLevel(const RPG::ServerWorld& world, const Game::JSON::Object& json);
    ServerLevel(const ServerLevel&) = delete;
    ServerLevel(ServerLevel&&) = delete;
    ~ServerLevel() = default;

    ServerLevel& operator=(const ServerLevel&) = delete;
    ServerLevel& operator=(ServerLevel&&) = delete;

    RPG::ServerCell&       cell(const RPG::Coordinates& coordinates);
    RPG::ServerCell&       cell(int x, int y);
    const RPG::ServerCell& cell(const RPG::Coordinates& coordinates) const;
    const RPG::ServerCell& cell(int x, int y) const;

    Game::JSON::Object json() const; // Serialize to JSON
  };
}