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
  protected:
    std::unordered_map<std::wstring, RPG::Model> _models; // Registered models
    
    const RPG::Model& getModel(const std::wstring& name);

  public:
    ModelSystem() = delete;
    ModelSystem(RPG::ECS& ecs);
    ModelSystem(const ModelSystem&) = delete;
    ModelSystem(ModelSystem&&) = delete;
    ~ModelSystem() = default;

    ModelSystem& operator=(const ModelSystem&) = delete;
    ModelSystem& operator=(ModelSystem&&) = delete;
  };

  class ServerModelSystem : public RPG::ModelSystem
  {
  public:
    ServerModelSystem() = delete;
    ServerModelSystem(RPG::ECS& ecs);
    ServerModelSystem(const ServerModelSystem&) = delete;
    ServerModelSystem(ServerModelSystem&&) = delete;
    ~ServerModelSystem() = default;

    ServerModelSystem&  operator=(const ServerModelSystem&) = delete;
    ServerModelSystem&  operator=(ServerModelSystem&&) = delete;

    void              load(const Game::JSON::Array& models);  // Deserialize models from JSON array
    Game::JSON::Array json() const;                           // Serialize models to JSON array
  };

  class ClientModelSystem : public RPG::ModelSystem
  {
  private:
    RPG::Camera                                     _camera;    // Camera of the scene
    std::unordered_map<std::wstring, RPG::Texture>  _textures;  // Loaded textures

    const RPG::Texture& getTexture(const std::wstring& name); // Get texture from cache

    void  handleLoad(const Game::JSON::Object& json);         // Load/reload of resources
    void  handleLoadModels(const Game::JSON::Object& json);   // Load/reload of models
    void  handleLoadTexture(const Game::JSON::Object& json);  // Load a texture file

  public:
    static bool CloserEntity(RPG::ECS& ecs, RPG::ECS::Entity aEntity, RPG::ECS::Entity bEntity);  // Check if A if closer to the camera than B
    static bool FartherEntity(RPG::ECS& ecs, RPG::ECS::Entity aEntity, RPG::ECS::Entity bEntity); // Check if A if farther to the camera than B

    ClientModelSystem() = delete;
    ClientModelSystem(RPG::ECS& ecs);
    ClientModelSystem(const ClientModelSystem&) = delete;
    ClientModelSystem(ClientModelSystem&&) = delete;
    ~ClientModelSystem() = default;

    ClientModelSystem&  operator=(const ClientModelSystem&) = delete;
    ClientModelSystem&  operator=(ClientModelSystem&&) = delete;

    const RPG::Camera&  getCamera() const;  // Get current camera
    
    bool  intersect(RPG::ECS::Entity entity, const Math::Vector<2>& coords) const;  // Check if coords is in bound of entity

    void  setModel(RPG::ECS::Entity entity, const std::wstring& name);                                            // Set model of entity
    void  setAnimation(RPG::ECS::Entity entity, const std::wstring& name, bool loop = false, float speed = 1.f);  // Set animation of entity
    
    void  executeCamera(float elapsed);                             // Update camera control
    void  executeAnimation(float elapsed);                          // Update animation of every entity
    void  executeAnimation(RPG::ECS::Entity entity, float elapsed); // Update animation of a single entity
    void  executeDraw();                                            // Draw every entity
    void  executeDraw(RPG::ECS::Entity entity);                     // Draw a single entity

    void  handlePacket(const Game::JSON::Object& json); // Handle a packet
  };
}