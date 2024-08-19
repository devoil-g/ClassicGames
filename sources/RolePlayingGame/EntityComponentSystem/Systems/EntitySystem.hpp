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
    EntitySystem() = default;
    EntitySystem(const EntitySystem&) = delete;
    EntitySystem(EntitySystem&&) = delete;
    ~EntitySystem() = default;

    EntitySystem& operator=(const EntitySystem&) = delete;
    EntitySystem& operator=(EntitySystem&&) = delete;

    RPG::ECS::Entity  getEntity(RPG::ECS& ecs, const std::string& id) const;  // Get an entity from its ID
  };

  class ServerEntitySystem : public RPG::EntitySystem
  {
  public:
    ServerEntitySystem() = default;
    ServerEntitySystem(const ServerEntitySystem&) = delete;
    ServerEntitySystem(ServerEntitySystem&&) = delete;
    ~ServerEntitySystem() = default;

    ServerEntitySystem& operator=(const ServerEntitySystem&) = delete;
    ServerEntitySystem& operator=(ServerEntitySystem&&) = delete;

    void              load(RPG::ECS& ecs, const Game::JSON::Array& models); // Deserialize entities from JSON array
    Game::JSON::Array json(RPG::ECS& ecs) const;                            // Serialize entities to JSON array
  };

  class ClientEntitySystem : public RPG::EntitySystem
  {
  public:
    ClientEntitySystem() = default;
    ClientEntitySystem(const ClientEntitySystem&) = delete;
    ClientEntitySystem(ClientEntitySystem&&) = delete;
    ~ClientEntitySystem() = default;

    ClientEntitySystem& operator=(const ClientEntitySystem&) = delete;
    ClientEntitySystem& operator=(ClientEntitySystem&&) = delete;

    RPG::ECS::Entity  getEntityAtPixel(RPG::ECS& ecs, const Math::Vector<2>& pixel) const; // Get entity at mouse position

    void  executePosition(RPG::ECS& ecs);                           // Update DisplayComponent with new position of entities
    void  executePosition(RPG::ECS& ecs, RPG::ECS::Entity entity);  // Update DisplayComponent with new position of a single entity

    void  handlePacket(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);        // Handle a packet
    void  handleLoad(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);          // Load/reload of resources
    void  handleLoadEntities(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);  // Load/reload of entities
  };
}