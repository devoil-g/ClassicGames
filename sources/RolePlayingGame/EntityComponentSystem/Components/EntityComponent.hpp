#pragma once

#include <string>

#include "RolePlayingGame/Types.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class EntityComponent
  {
  public:
    static const RPG::Coordinates DefaultCoordinates;
    static const RPG::Direction   DefaultDirection;
    static const RPG::Position    DefaultPosition;
    static const std::string      DefaultModel;

    std::string       id;           // Identifier of entity
    RPG::Coordinates  coordinates;  // Coordinates of entity's cell
    RPG::Direction    direction;    // Direction the entity is facing
    RPG::Position     position;     // Position of entity in its cell
    std::string       model;        // Model of the entity
   
    // Characteristics
    int attack;
    int defense;

    EntityComponent();
    EntityComponent(const Game::JSON::Object& json);
    EntityComponent(const EntityComponent&) = default;
    EntityComponent(EntityComponent&&) = default;
    ~EntityComponent() = default;

    EntityComponent&  operator=(const EntityComponent&) = default;
    EntityComponent&  operator=(EntityComponent&&) = default;

    Game::JSON::Object  json() const; // Serialize to JSON
  };
}