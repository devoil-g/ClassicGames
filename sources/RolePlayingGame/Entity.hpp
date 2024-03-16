#pragma once

#include <cstdint>
#include <string>

#include "RolePlayingGame/Types.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class Level;

  class Entity
  {
  private:
    static std::uint64_t generateId(); // Entity unique ID generator
    static std::uint64_t _maxId;       // Highest ID used (for ID generator)

    void updateServerMove(float elapsed); // Update move properties
    void updateClientMove(float elapsed); // Update move properties
    
  public:
    static const RPG::Coordinates DefaultCoordinates;
    static const RPG::Position    DefaultPosition;
    static const RPG::Direction   DefaultDirection;
    static const std::string      DefaultModel;

    const std::uint64_t id; // Unique entity identifier
    
    RPG::Coordinates coordinates; // Current cell coordinate of entity
    RPG::Position    position;    // Position of entity in its cell
    RPG::Direction   direction;   // Direction the entity is facing
    std::string      model;       // Name of entity's model

    struct Move
    {
      float            speed;      // Move speed
      float            completion; // Move completion ratio (from 0 to 1)
      RPG::Position    position;   // Position at start of move
      RPG::Coordinates target;     // Final target coordinates of move

      Move();
      Move(const Move&) = default;
      Move(Move&&) = default;
      ~Move() = default;
    } move; // Move properties

    Entity() = delete;
    Entity(const Game::JSON::Object& json);
    Entity(const Entity& other) = delete;
    Entity(Entity&& other) = delete;
    ~Entity() = default;

    Entity& operator=(const Entity&) = delete;
    Entity& operator=(Entity&&) = delete;

    Game::JSON::Object  json() const; // Serialize entity

    void  updateServer(float elasped);
    void  updateClient(float elasped);
  };
}