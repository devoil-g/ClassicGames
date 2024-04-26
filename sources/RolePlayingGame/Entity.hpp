#pragma once

#include <cstdint>
#include <list>
#include <string>

#include <SFML/Graphics.hpp>

#include "RolePlayingGame/Color.hpp"
#include "RolePlayingGame/Color.hpp"
#include "RolePlayingGame/Model.hpp"
#include "RolePlayingGame/Texture.hpp"
#include "RolePlayingGame/Types.hpp"
#include "System/Config.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  namespace Entity
  {
    const RPG::Position  DefaultPosition = RPG::Position(0.f, 0.f, 0.f);
    const RPG::Direction DefaultDirection = RPG::Direction::DirectionNorth;
    const std::string    DefaultModel = "";
  }

  class ClientWorld;
  class ClientLevel;
  
  class ClientEntity
  {
  public:
    const std::uint64_t id; // Unique entity identifier

    RPG::Coordinates coordinates; // Current cell coordinates of entity
    RPG::Position    position;    // Position of entity in its cell (screen X, Y, altitude)
    RPG::Direction   direction;   // Direction the entity is facing
    RPG::Color       outline;     // Color of entity outline

  private:
    struct {
      RPG::Coordinates target;    // Final targeted cell coordinates
      RPG::Position    position;  // Targeted position
      float            remaining; // Remaining time of move
      float            speed;     // Speed of move
    } _move;      // Entity movement
    
    struct {
      const RPG::Model*            model;     // Model of the entity
      const RPG::Model::Animation* animation; // Current animation
      const RPG::Texture*          texture;   // Texture of the model

      std::size_t frame;   // Current frame
      float       elapsed; // Time elapsed on current frame
      bool        loop;    // True: loop animation, false: idle when over
    } _animation; // Entity animation

    struct {
      bool pickup;    // Entity can be picked-up (item)
      bool container; // Entity can be opened (chest)
      bool dialog;    // Entity can be talked to (dialog)
      bool fight;     // Entity can be attacked (fighter)
      bool trigger;   // Entity can be triggered (button)
    } _actions;

    void  updateMove(RPG::ClientWorld& world, RPG::ClientLevel& level, float elapsed);
    void  updateAnimation(RPG::ClientWorld& world, RPG::ClientLevel& level, float elapsed);

    void  updateMove(RPG::ClientWorld& world, RPG::ClientLevel& level, const Game::JSON::Object& json);
    void  updateAnimation(RPG::ClientWorld& world, RPG::ClientLevel& level, const Game::JSON::Object& json);

    void  handleMoveRun(RPG::ClientWorld& world, RPG::ClientLevel& level, RPG::Coordinates coordinates, RPG::Position position, RPG::Direction direction, RPG::Coordinates target, float speed); // Move to coordinates/position
    void  handleMoveTeleport(RPG::ClientWorld& world, RPG::ClientLevel& level, RPG::Coordinates coordinates, RPG::Position position, RPG::Direction direction);                                  // Teleport to coordinates/position
    void  handleMoveCancel(RPG::ClientWorld& world, RPG::ClientLevel& level);                                                                                                                    // Interrupt current move

    void  handleAnimationStart(RPG::ClientWorld& world, RPG::ClientLevel& level, const std::string& name, bool loop);
    void  handleAnimationModel(RPG::ClientWorld& world, RPG::ClientLevel& level, const std::string& name);

  public:
    ClientEntity() = delete;
    ClientEntity(RPG::ClientWorld& world, RPG::ClientLevel& level, const Game::JSON::Object& json);
    ClientEntity(const ClientEntity&) = delete;
    ClientEntity(ClientEntity&&) = delete;
    ~ClientEntity() = default;

    ClientEntity& operator=(const ClientEntity&) = delete;
    ClientEntity& operator=(ClientEntity&&) = delete;
    
    void  setModel(RPG::ClientWorld& world, RPG::ClientLevel& level, const std::string& name);
    void  setAnimation(RPG::ClientWorld& world, RPG::ClientLevel& level, const std::string& name, bool loop);

    void        update(RPG::ClientWorld& world, RPG::ClientLevel& level, float elapsed);
    void        draw(const RPG::ClientWorld& world, const RPG::ClientLevel& level) const;
    RPG::Bounds bounds(const RPG::ClientWorld& world, const RPG::ClientLevel& level) const; // Get bounds of entity in 2D world coordinates
    
    void  update(RPG::ClientWorld& world, RPG::ClientLevel& level, const Game::JSON::Object& json); // Update entity from JSON

    bool  isPickup() const;     // Get pickup action
    bool  isContainer() const;  // Get container action
    bool  isDialog() const;     // Get dialog action
    bool  isFight() const;      // Get fight action
    bool  isTrigger() const;    // Get trigger action
  };

  class ServerWorld;
  class ServerLevel;

  class ServerEntity
  {
  private:
    static std::uint64_t generateId(); // Entity unique ID generator
    static std::uint64_t _maxId;       // Highest ID used (for ID generator)

  public:
    const std::uint64_t id; // Unique entity identifier

    RPG::Coordinates coordinates; // Current cell coordinate of entity
    RPG::Position    position;    // Position of entity in its cell (screen X, Y, altitude)
    RPG::Direction   direction;   // Direction the entity is facing

  private:
    struct {
      RPG::Coordinates target;     // Move target coordinates, used to know if move is completed
      float            completion; // Completion of move [0, +1]
      float            speed;      // Speed of completion
    } _move; // Entity movement

    struct {
      std::string model; // Model of the entity
    } _animation; // Entity animation

    struct {
      std::string pickup;    // ID of picked-up (item)
      std::string container; // ID of container (chest)
      std::string dialog;    // ID of dialog (dialog)
      std::string fight;     // ID of fight (fighter)
      std::string trigger;   // ID of trigger (button)
    } _actions;

    void  updateMove(const RPG::ServerWorld& world, float elapsed);

  public:
    ServerEntity() = delete;
    ServerEntity(const RPG::ServerWorld& world, const RPG::ServerLevel& level, const Game::JSON::Object& json);
    ServerEntity(const ServerEntity&) = delete;
    ServerEntity(ServerEntity&&) = delete;
    ~ServerEntity() = default;

    ServerEntity& operator=(const ServerEntity&) = delete;
    ServerEntity& operator=(ServerEntity&&) = delete;

    void  update(const RPG::ServerWorld& world, float elapsed);
    
    Game::JSON::Object  json() const; // Serialize entity to JSON

    void  move();
    void  teleport();
  };
}