#pragma once

#include <unordered_map>

#include "RolePlayingGame/Entity.hpp"
#include "RolePlayingGame/Level.hpp"
#include "RolePlayingGame/Model.hpp"
#include "RolePlayingGame/Texture.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class ClientScene;

  class ClientWorld
  {
  private:
    void loadLevel(const Game::JSON::Object& json);    // Load world from JSON
    void loadModels(const Game::JSON::Object& json);    // Load world from JSON
    void loadTextures(const Game::JSON::Object& json);    // Load world from JSON

  public:
    ClientWorld();
    ClientWorld(const ClientWorld&) = delete;
    ClientWorld(ClientWorld&&) = delete;
    ~ClientWorld() = default;

    ClientWorld& operator=(const ClientWorld&) = delete;
    ClientWorld& operator=(ClientWorld&&) = delete;

    std::unique_ptr<RPG::ClientLevel>             level;    // Current level, empty when no level
    std::unordered_map<std::string, RPG::Model>   models;   // Models library
    std::unordered_map<std::string, RPG::Texture> textures; // Texture library
    
    RPG::Model& model(const std::string& name);             // Get model
    const RPG::Model& model(const std::string& name) const; // Get model

    RPG::Texture& texture(const std::string& name);             // Get texture
    const RPG::Texture& texture(const std::string& name) const; // Get texture

    void load(const Game::JSON::Object& json);    // Load world from JSON
    void update(const Game::JSON::Object& json);  // Update world from JSON

    void update(RPG::ClientScene& client, float elapsed); // Update world
    void draw();
  };

  class Server;

  class ServerWorld
  {
  public:
    std::unordered_map<std::string, RPG::ServerLevel> levels;
    std::unordered_map<std::string, RPG::Model>       models;

    ServerWorld() = delete;
    ServerWorld(const Game::JSON::Object& json);
    ServerWorld(const ServerWorld&) = delete;
    ServerWorld(ServerWorld&&) = delete;
    ~ServerWorld() = default;

    ServerWorld& operator=(const ServerWorld&) = delete;
    ServerWorld& operator=(ServerWorld&&) = delete;

    void  connect(RPG::Server& server, std::size_t id);                                 // Handle player connecting
    void  disconnect(RPG::Server& server, std::size_t id);                              // Handle player disconnecting
    void  receive(RPG::Server& server, std::size_t id, const Game::JSON::Object& json); // Handle request from player

    void  update(RPG::Server& server, float elapsed); // Simulate game world

    Game::JSON::Object  json() const; // Serialize world for new client
  };
}