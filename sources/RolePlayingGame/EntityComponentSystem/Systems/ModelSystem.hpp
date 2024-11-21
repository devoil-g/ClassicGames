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

  class ModelSystem : public RPG::ECS::System
  {
  public:
    static const float  CellOffsetX;
    static const float  CellOffsetY;

  protected:
    std::unordered_map<std::string, RPG::Model> _models;  // Registered models
    
    const RPG::Model& getModel(const std::string& name) const;

  public:
    ModelSystem() = default;
    ModelSystem(const ModelSystem&) = delete;
    ModelSystem(ModelSystem&&) = delete;
    ~ModelSystem() = default;

    ModelSystem& operator=(const ModelSystem&) = delete;
    ModelSystem& operator=(ModelSystem&&) = delete;
  };

  class ServerModelSystem : public RPG::ModelSystem
  {
  public:
    ServerModelSystem() = default;
    ServerModelSystem(const ServerModelSystem&) = delete;
    ServerModelSystem(ServerModelSystem&&) = delete;
    ~ServerModelSystem() = default;

    ServerModelSystem&  operator=(const ServerModelSystem&) = delete;
    ServerModelSystem&  operator=(ServerModelSystem&&) = delete;

    void              load(RPG::ECS& ecs, const Game::JSON::Array& models); // Deserialize models from JSON array
    Game::JSON::Array json(RPG::ECS& ecs) const;                            // Serialize models to JSON array
  };

  class ClientModelSystem : public RPG::ModelSystem
  {
  private:
    RPG::Camera                                   _camera;    // Camera of the scene
    std::unordered_map<std::string, RPG::Texture> _textures;  // Loaded textures

    const RPG::Texture& getTexture(const std::string& name);      // Get texture from cache

  public:
    static bool CloserEntity(RPG::ECS& ecs, RPG::ECS::Entity aEntity, RPG::ECS::Entity bEntity);  // Check if A if closer to the camera than B
    static bool FartherEntity(RPG::ECS& ecs, RPG::ECS::Entity aEntity, RPG::ECS::Entity bEntity); // Check if A if farther to the camera than B

    ClientModelSystem();
    ClientModelSystem(const ClientModelSystem&) = delete;
    ClientModelSystem(ClientModelSystem&&) = delete;
    ~ClientModelSystem() = default;

    ClientModelSystem&  operator=(const ClientModelSystem&) = delete;
    ClientModelSystem&  operator=(ClientModelSystem&&) = delete;

    const RPG::Camera&        getCamera() const;                                      // Get current camera
    RPG::Model::Actor         getActor(const std::string& name) const;                // Get a new model's actor by name
    const RPG::Model::Actor&  getActor(RPG::ECS& ecs, RPG::ECS::Entity entity) const; // Get actor of a given entity
    
    bool  intersect(RPG::ECS& ecs, RPG::ECS::Entity entity, const Math::Vector<2>& coords) const; // Check if coords is in bound of entity

    void  setModel(RPG::ECS& ecs, RPG::ECS::Entity entity, const std::string& name);                                            // Set model of entity
    void  setAnimation(RPG::ECS& ecs, RPG::ECS::Entity entity, const std::string& name, bool loop = false, float speed = 1.f);  // Set animation of entity
    void  setAnimationRandom(RPG::ECS& ecs, RPG::ECS::Entity entity, bool loop, float speed = 1.f);                             // Set a random animation of entity

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