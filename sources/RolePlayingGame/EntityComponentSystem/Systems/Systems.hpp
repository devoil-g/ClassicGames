#pragma once

#include <string>
#include <unordered_map>

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"
#include "RolePlayingGame/Icon.hpp"

namespace RPG
{
  class IconSystem : public RPG::ECS::System
  {
  public:
    static const unsigned int InterfaceWidth = 256;
    static const unsigned int InterfaceHeight = 144;

  protected:
    std::unordered_map<std::string, RPG::Icon>  _icons; // Registered icons

    const RPG::Icon&  getIcon(const std::string& name) const;

  public:
    IconSystem() = default;
    IconSystem(const IconSystem&) = delete;
    IconSystem(IconSystem&&) = delete;
    ~IconSystem() = default;

    IconSystem& operator=(const IconSystem&) = delete;
    IconSystem& operator=(IconSystem&&) = delete;
  };

  class ServerIconSystem : public RPG::IconSystem
  {
  public:
    ServerIconSystem() = default;
    ServerIconSystem(const ServerIconSystem&) = delete;
    ServerIconSystem(ServerIconSystem&&) = delete;
    ~ServerIconSystem() = default;

    ServerIconSystem& operator=(const ServerIconSystem&) = delete;
    ServerIconSystem& operator=(ServerIconSystem&&) = delete;

    void              load(RPG::ECS& ecs, const Game::JSON::Array& icons);  // Deserialize icons from JSON array
    Game::JSON::Array json(RPG::ECS& ecs) const;                            // Serialize icons to JSON array
  };

  /*
  class MovingSystem : public RPG::ECS::System
  {
  public:
    MovingSystem() = default;
    MovingSystem(const MovingSystem&) = delete;
    MovingSystem(MovingSystem&&) = delete;
    ~MovingSystem() = default;

    MovingSystem& operator=(const MovingSystem&) = delete;
    MovingSystem& operator=(MovingSystem&&) = delete;

    void  setMove(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, RPG::ECS::Entity entity, Math::Vector<3, float> position, float duration);

    void  execute(RPG::ECS& ecs, RPG::ClientWorld& world, RPG::ClientLevel& level, float elapsed); // Update move of entities
  };
  */
}