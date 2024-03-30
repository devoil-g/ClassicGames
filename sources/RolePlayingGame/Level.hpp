#pragma once

#include <cstdint>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "RolePlayingGame/Cell.hpp"
#include "RolePlayingGame/Color.hpp"
#include "RolePlayingGame/Entity.hpp"
#include "RolePlayingGame/Types.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class ClientWorld;
  
  namespace Level
  {
    const RPG::Color  DefaultColor = RPG::Color::Default;
  }

  class ClientLevel
  {
  private:
    class Layer
    {
    private:
      class Element
      {
      public:
        std::size_t frame; // Current frame
        float elapsed;     // Time elapsed on current frame
        
        Element();
        Element(const Element&) = delete;
        Element(Element&&) = delete;
        ~Element() = default;

        Element& operator=(const Element&) = delete;
        Element& operator=(Element&&) = delete;

        void update(const RPG::Model::Animation& animation, float elapsed);
        void draw(const std::string& spritesheet, const RPG::Model::Animation& animation) const;
      };

    public:
      std::string model;                                 // Model of layer
      std::unordered_map<std::string, Element> elements; // Elements used to play each animation of the model

      Layer() = delete;
      Layer(const std::string& model);
      Layer(const Layer&) = delete;
      Layer(Layer&&) = delete;
      ~Layer() = default;

      Layer& operator=(const Layer&) = delete;
      Layer& operator=(Layer&&) = delete;

      void update(const RPG::ClientWorld& world, float elapsed);
      void draw(const RPG::ClientWorld& world) const;
    };

    void updateEntity(const RPG::ClientWorld& world, const Game::JSON::Object& json); // Update level entity from JSON

  public:
    std::string name;       // Name of the level
    RPG::Color  color;      // Background color
    Layer       background; // Background layer of the level
    Layer       foreground; // Foreground layer of the level

    std::list<RPG::ClientCell>   cells;    // Cells of the level
    std::list<RPG::ClientEntity> entities; // Entities of the level

    ClientLevel() = delete;
    ClientLevel(const RPG::ClientWorld& world, const Game::JSON::Object& json);
    ClientLevel(const ClientLevel&) = delete;
    ClientLevel(ClientLevel&&) = delete;
    ~ClientLevel() = default;

    ClientLevel& operator=(const ClientLevel&) = delete;
    ClientLevel& operator=(ClientLevel&&) = delete;

    RPG::ClientCell&       cell(const RPG::Coordinates& coordinates);
    RPG::ClientCell&       cell(int x, int y);
    const RPG::ClientCell& cell(const RPG::Coordinates& coordinates) const;
    const RPG::ClientCell& cell(int x, int y) const;

    RPG::ClientEntity&       entity(std::size_t id);
    const RPG::ClientEntity& entity(std::size_t id) const;

    Math::Vector<2, float> computePosition(const RPG::Coordinates& coordinates, const RPG::Position& position = { 0.f, 0.f, 0.f }) const; // Compute world position from coordinates and postion

    void update(const RPG::ClientWorld& world, const Game::JSON::Object& json); // Update level from JSON

    void update(const RPG::ClientWorld& world, float elapsed); // Update level
    void draw(const RPG::ClientWorld& world) const;            // Draw level
  };
  
  class ServerWorld;

  class ServerLevel
  {
  private:
    static const RPG::ServerCell InvalidCell; // Empty cell used when adressing invalid coordinates

  public:
    RPG::Color  color;      // Background color
    std::string background; // Model name of background, displayed behind everything (every animations displayed at position [0,0])
    std::string foreground; // Model name of foreground, displayed in front of everything (every animations displayed at position [0,0])

    std::unordered_map<RPG::Coordinates, RPG::ServerCell> cells;    // Cells of the level
    std::list<RPG::ServerEntity>                          entities; // Entities of the level

    ServerLevel() = delete;
    ServerLevel(const Game::JSON::Object& json);
    ServerLevel(const ServerLevel&) = delete;
    ServerLevel(ServerLevel&&) = delete;
    ~ServerLevel() = default;

    ServerLevel& operator=(const ServerLevel&) = delete;
    ServerLevel& operator=(ServerLevel&&) = delete;

    RPG::ServerCell&       cell(const RPG::Coordinates& coordinates);
    RPG::ServerCell&       cell(int x, int y);
    const RPG::ServerCell& cell(const RPG::Coordinates& coordinates) const;
    const RPG::ServerCell& cell(int x, int y) const;

    Game::JSON::Object json() const; // Serialize to JSON
  };
}