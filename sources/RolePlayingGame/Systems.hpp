#pragma once

#include <unordered_map>

#include "RolePlayingGame/EntityComponentSystem.hpp"
#include "RolePlayingGame/Camera.hpp"
#include "RolePlayingGame/Model.hpp"
#include "RolePlayingGame/Texture.hpp"

namespace RPG
{
  class Server;
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





  class DisplaySystem : public RPG::ECS::System
  {
  public:
    static const float  CellOffsetX;
    static const float  CellOffsetY;

  protected:
    std::unordered_map<std::string, RPG::Model>   _models;  // Registered models

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





  class InterfaceSystem : public RPG::ECS::System
  {
  public:
    InterfaceSystem() = default;
    InterfaceSystem(const InterfaceSystem&) = delete;
    InterfaceSystem(InterfaceSystem&&) = delete;
    ~InterfaceSystem() = default;

    InterfaceSystem& operator=(const InterfaceSystem&) = delete;
    InterfaceSystem& operator=(InterfaceSystem&&) = delete;
  };





  class BoardSystem : public RPG::ECS::System
  {
  private:
    std::unordered_map<RPG::Coordinates, RPG::ECS::Entity>  _map; // Map of coordinates to entity

  public:
    BoardSystem() = default;
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
    ServerBoardSystem() = default;
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
  public:
    ClientBoardSystem() = default;
    ClientBoardSystem(const ClientBoardSystem&) = delete;
    ClientBoardSystem(ClientBoardSystem&&) = delete;
    ~ClientBoardSystem() = default;

    ClientBoardSystem& operator=(const ClientBoardSystem&) = delete;
    ClientBoardSystem& operator=(ClientBoardSystem&&) = delete;

    void  executeCell(RPG::ECS& ecs, float elapsed);                          // Update cells board
    void  executeCell(RPG::ECS& ecs, RPG::ECS::Entity entity, float elapsed); // Update a single cell

    void  handlePacket(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);    // Handle a packet
    void  handleLoad(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);      // Load/reload of resources
    void  handleLoadCells(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json); // Load/reload of cells
  };





  class NetworkSystem : public RPG::ECS::System
  {
  public:
    class Player
    {
    public:
      static const std::string  DefaultName;
      static const Player       ErrorPlayer;

    public:
      std::string name; // Pseudo of the player
      
      Player();
      Player(const Game::JSON::Object& json);
      Player(const Player&) = default;
      Player(Player&&) = default;
      ~Player() = default;

      Player& operator=(const Player&) = default;
      Player& operator=(Player&&) = default;

      Game::JSON::Object  json() const; // Serialize player informations
    };

  protected:
    std::unordered_map<std::size_t, Player> _players; // Description of each player

  public:
    NetworkSystem() = default;
    NetworkSystem(const NetworkSystem&) = delete;
    NetworkSystem(NetworkSystem&&) = delete;
    ~NetworkSystem() = default;

    const Player& getPlayer(std::size_t controller) const;  // Get player informations

    NetworkSystem& operator=(const NetworkSystem&) = delete;
    NetworkSystem& operator=(NetworkSystem&&) = delete;
  };

  class ServerNetworkSystem : public RPG::NetworkSystem
  {
  public:
    ServerNetworkSystem() = default;
    ServerNetworkSystem(const ServerNetworkSystem&) = delete;
    ServerNetworkSystem(ServerNetworkSystem&&) = delete;
    ~ServerNetworkSystem() = default;

    ServerNetworkSystem&  operator=(const ServerNetworkSystem&) = delete;
    ServerNetworkSystem&  operator=(ServerNetworkSystem&&) = delete;

    void  connect(RPG::ECS& ecs, RPG::Server& server, std::size_t id);    // New player join the game
    void  disconnect(RPG::ECS& ecs, RPG::Server& server, std::size_t id); // A player leave the game
  };

  class ClientNetworkSystem : public RPG::NetworkSystem
  {
  private:
    std::size_t self; // ID of player

  public:
    ClientNetworkSystem();
    ClientNetworkSystem(const ClientNetworkSystem&) = delete;
    ClientNetworkSystem(ClientNetworkSystem&&) = delete;
    ~ClientNetworkSystem() = default;

    ClientNetworkSystem& operator=(const ClientNetworkSystem&) = delete;
    ClientNetworkSystem& operator=(ClientNetworkSystem&&) = delete;

    void  handlePacket(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);      // Handle a packet
    void  handleConnect(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);     // New player join the game
    void  handleDisconnect(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);  // A player leave the game
    void  handleAssign(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);      // Assign an entity to a player
    void  handlePlayer(RPG::ECS& ecs, RPG::ClientScene& client, const Game::JSON::Object& json);      // Update player informations
  };





  class ParticleSystem : public RPG::ECS::System
  {
  public:
    ParticleSystem() = default;
    ParticleSystem(const ParticleSystem&) = delete;
    ParticleSystem(ParticleSystem&&) = delete;
    ~ParticleSystem() = default;

    ParticleSystem& operator=(const ParticleSystem&) = delete;
    ParticleSystem& operator=(ParticleSystem&&) = delete;

    void  execute(RPG::ECS& ecs, float elapsed);                                  // Update existing particles
    void  executeParticle(RPG::ECS& ecs, RPG::ECS::Entity entity, float elapsed); // Update a single particle
  };

  class ParticleEmitterSystem : public RPG::ECS::System
  {
  public:
    ParticleEmitterSystem() = default;
    ParticleEmitterSystem(const ParticleSystem&) = delete;
    ParticleEmitterSystem(ParticleSystem&&) = delete;
    ~ParticleEmitterSystem() = default;

    ParticleEmitterSystem& operator=(const ParticleEmitterSystem&) = delete;
    ParticleEmitterSystem& operator=(ParticleEmitterSystem&&) = delete;

    void  execute(RPG::ECS& ecs, float elapsed);                                          // Update existing particle emitters
    void  executeParticleEmitter(RPG::ECS& ecs, RPG::ECS::Entity entity, float elapsed);  // Update a single particle emitter
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