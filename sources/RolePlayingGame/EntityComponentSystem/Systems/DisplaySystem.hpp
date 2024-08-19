#pragma once

#include <string>
#include <unordered_map>

#include "RolePlayingGame/EntityComponentSystem/EntityComponentSystem.hpp"
#include "RolePlayingGame/Camera.hpp"
#include "RolePlayingGame/Model.hpp"
#include "RolePlayingGame/Texture.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class ClientScene;

  class DisplaySystem : public RPG::ECS::System
  {
  public:
    static const float  CellOffsetX;
    static const float  CellOffsetY;

  protected:
    std::unordered_map<std::string, RPG::Model> _models;  // Registered models

  public:
    DisplaySystem() = default;
    DisplaySystem(const DisplaySystem&) = delete;
    DisplaySystem(DisplaySystem&&) = delete;
    ~DisplaySystem() = default;

    DisplaySystem& operator=(const DisplaySystem&) = delete;
    DisplaySystem& operator=(DisplaySystem&&) = delete;
  };

  class ServerDisplaySystem : public RPG::DisplaySystem
  {
  public:
    ServerDisplaySystem() = default;
    ServerDisplaySystem(const ServerDisplaySystem&) = delete;
    ServerDisplaySystem(ServerDisplaySystem&&) = delete;
    ~ServerDisplaySystem() = default;

    ServerDisplaySystem&  operator=(const ServerDisplaySystem&) = delete;
    ServerDisplaySystem&  operator=(ServerDisplaySystem&&) = delete;

    void              load(RPG::ECS& ecs, const Game::JSON::Array& models); // Deserialize models from JSON array
    Game::JSON::Array json(RPG::ECS& ecs) const;                            // Serialize models to JSON array
  };

  class ClientDisplaySystem : public RPG::DisplaySystem
  {
  private:
    RPG::Camera                                   _camera;    // Camera of the scene
    std::unordered_map<std::string, RPG::Texture> _textures;  // Loaded textures

    const RPG::Texture& getTexture(const std::string& name);      // Get texture from cache
    const RPG::Model&   getModel(const std::string& name) const;  // Get model from library

  public:
    static bool CloserEntity(RPG::ECS& ecs, RPG::ECS::Entity aEntity, RPG::ECS::Entity bEntity);  // Check if A if closer to the camera than B
    static bool FartherEntity(RPG::ECS& ecs, RPG::ECS::Entity aEntity, RPG::ECS::Entity bEntity); // Check if A if farther to the camera than B

    ClientDisplaySystem();
    ClientDisplaySystem(const ClientDisplaySystem&) = delete;
    ClientDisplaySystem(ClientDisplaySystem&&) = delete;
    ~ClientDisplaySystem() = default;

    ClientDisplaySystem&  operator=(const ClientDisplaySystem&) = delete;
    ClientDisplaySystem&  operator=(ClientDisplaySystem&&) = delete;

    const RPG::Camera&  getCamera() const;  // Get current camera

    bool  intersect(RPG::ECS& ecs, RPG::ECS::Entity entity, const Math::Vector<2>& coords) const; // Check if coords is in bound of entity

    void  setModel(RPG::ECS& ecs, RPG::ECS::Entity entity, const std::string& name);                        // Set model of entity
    void  setModel(RPG::ECS& ecs, RPG::ECS::Entity entity, const RPG::Model& model);                        // Set model of entity
    void  setAnimation(RPG::ECS& ecs, RPG::ECS::Entity entity, const std::string& name, bool loop);         // Set animation of entity
    void  setAnimation(RPG::ECS& ecs, RPG::ECS::Entity entity, const RPG::Animation& animation, bool loop); // Set animation of entity
    void  setRandomAnimation(RPG::ECS& ecs, RPG::ECS::Entity entity, bool loop);                            // Set a random animation of entity

    void  executeCamera(RPG::ECS& ecs, float elapsed);                              // Update camera control
    void  executeAnimation(RPG::ECS& ecs, float elapsed);                           // Update animation of every entity
    void  executeAnimation(RPG::ECS& ecs, RPG::ECS::Entity entity, float elapsed);  // Update animation of a single entity
    void  executeDraw(RPG::ECS& ecs);                                               // Draw every entity
    void  executeDraw(RPG::ECS& ecs, RPG::ECS::Entity entity);                      // Draw a single entity

    void  handlePacket(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);      // Handle a packet
    void  handleLoad(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);        // Load/reload of resources
    void  handleLoadModels(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);  // Load/reload of models
    void  handleLoadTexture(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json); // Load a texture file
  };
}