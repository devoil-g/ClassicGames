#pragma once

#include <string>

#include "Math/Vector.hpp"
#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class ClientScene;

  class EntitySystem : public RPG::ECS::System
  {
  public:
    EntitySystem() = delete;
    EntitySystem(RPG::ECS& ecs);
    EntitySystem(const EntitySystem&) = delete;
    EntitySystem(EntitySystem&&) = delete;
    ~EntitySystem() = default;

    EntitySystem& operator=(const EntitySystem&) = delete;
    EntitySystem& operator=(EntitySystem&&) = delete;

    RPG::ECS::Entity  getEntity(const std::wstring& id) const;  // Get an entity from its ID
  };

  class ServerEntitySystem : public RPG::EntitySystem
  {
  public:
    ServerEntitySystem() = delete;
    ServerEntitySystem(RPG::ECS& ecs);
    ServerEntitySystem(const ServerEntitySystem&) = delete;
    ServerEntitySystem(ServerEntitySystem&&) = delete;
    ~ServerEntitySystem() = default;

    ServerEntitySystem& operator=(const ServerEntitySystem&) = delete;
    ServerEntitySystem& operator=(ServerEntitySystem&&) = delete;

    void              load(const Game::JSON::Array& models);  // Deserialize entities from JSON array
    Game::JSON::Array json() const;                           // Serialize entities to JSON array
  };

  class ClientEntitySystem : public RPG::EntitySystem
  {
  private:
    void  handleLoad(const Game::JSON::Object& json);         // Load/reload of resources
    void  handleLoadEntities(const Game::JSON::Object& json); // Load/reload of entities

  public:
    ClientEntitySystem() = delete;
    ClientEntitySystem(RPG::ECS& ecs);
    ClientEntitySystem(const ClientEntitySystem&) = delete;
    ClientEntitySystem(ClientEntitySystem&&) = delete;
    ~ClientEntitySystem() = default;

    ClientEntitySystem& operator=(const ClientEntitySystem&) = delete;
    ClientEntitySystem& operator=(ClientEntitySystem&&) = delete;

    RPG::ECS::Entity  intersect(const Math::Vector<2>& coords) const;                           // Find entity at coords
    bool              intersect(RPG::ECS::Entity entity, const Math::Vector<2>& coords) const;  // Check entity intersect at position

    void  executePosition();                        // Update WorldComponent with new position of entities
    void  executePosition(RPG::ECS::Entity entity); // Update WorldComponent with new position of a single entity

    void  handlePacket(const Game::JSON::Object& json); // Handle a packet
    
  };
}