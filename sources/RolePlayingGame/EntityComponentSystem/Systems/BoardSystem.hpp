#pragma once

#include <unordered_map>

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"
#include "RolePlayingGame/Types.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class ClientScene;

  class BoardSystem : public RPG::ECS::System
  {
  private:
    std::unordered_map<RPG::Coordinates, RPG::ECS::Entity>  _map; // Map of coordinates to entity

  public:
    BoardSystem() = delete;
    BoardSystem(RPG::ECS& ecs);
    BoardSystem(const BoardSystem&) = delete;
    BoardSystem(BoardSystem&&) = delete;
    ~BoardSystem() = default;

    BoardSystem& operator=(const BoardSystem&) = delete;
    BoardSystem& operator=(BoardSystem&&) = delete;

    RPG::ECS::Entity  getCell(RPG::Coordinates coordinates) const;            // Get cell registered on board
    void              registerCell(RPG::ECS& ecs, RPG::ECS::Entity entity);   // Register a cell on board
    void              unregisterCell(RPG::ECS& ecs, RPG::ECS::Entity entity); // Unregister a cell on board
  };

  class ServerBoardSystem : public BoardSystem
  {
  public:
    ServerBoardSystem() = delete;
    ServerBoardSystem(RPG::ECS& ecs);
    ServerBoardSystem(const ServerBoardSystem&) = delete;
    ServerBoardSystem(ServerBoardSystem&&) = delete;
    ~ServerBoardSystem() = default;

    ServerBoardSystem& operator=(const ServerBoardSystem&) = delete;
    ServerBoardSystem& operator=(ServerBoardSystem&&) = delete;

    void              load(RPG::ECS& ecs, const Game::JSON::Array& cells);  // Deserialize cells from JSON array
    Game::JSON::Array json(RPG::ECS& ecs) const;                            // Serialize game board
  };

  class ClientBoardSystem : public BoardSystem
  {
  private:
    RPG::ECS::Entity  _cursorModel; // Model of the cursor
    RPG::ECS::Entity  _cursorCell;  // Cell of cursor

  public:
    ClientBoardSystem() = delete;
    ClientBoardSystem(RPG::ECS& ecs);
    ClientBoardSystem(const ClientBoardSystem&) = delete;
    ClientBoardSystem(ClientBoardSystem&&) = delete;
    ~ClientBoardSystem() = default;

    ClientBoardSystem& operator=(const ClientBoardSystem&) = delete;
    ClientBoardSystem& operator=(ClientBoardSystem&&) = delete;

    RPG::ECS::Entity  getCursor() const;                                  // Get current cursor cell
    void              setCursor(RPG::ECS& ecs, RPG::ECS::Entity entity);  // Change cursor cell
    
    RPG::ECS::Entity  intersect(RPG::ECS& ecs, const Math::Vector<2>& coords) const;  // Find cell at coords
    
    void  executeCursor(RPG::ECS& ecs, float elapsed);                        // Update bord cursor
    void  executeCell(RPG::ECS& ecs, float elapsed);                          // Update cells board
    void  executeCell(RPG::ECS& ecs, RPG::ECS::Entity entity, float elapsed); // Update a single cell

    void  handlePacket(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);    // Handle a packet
    void  handleLoad(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);      // Load/reload of resources
    void  handleLoadCells(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json); // Load/reload of cells
  };
}