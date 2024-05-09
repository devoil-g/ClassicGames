#pragma once

#include "Math/Vector.hpp"
#include "RolePlayingGame/Model.hpp"

namespace RPG
{
  class PositionComponent
  {
  public:
    Math::Vector<3, float>  position;   // Position of entity on its cell (x: screen +X, y: screen +Y, z: screen -Y)
    RPG::Direction          direction;  // Direction the entity is facing

    PositionComponent();
    PositionComponent(const PositionComponent&) = default;
    PositionComponent(PositionComponent&&) = default;
    ~PositionComponent() = default;

    PositionComponent& operator=(const PositionComponent&) = default;
    PositionComponent& operator=(PositionComponent&&) = default;
  };

  class CoordinatesComponent
  {
  public:
    Math::Vector<2, int>  coordinates;  // Coordinates of entity in level

    CoordinatesComponent();
    CoordinatesComponent(const CoordinatesComponent&) = default;
    CoordinatesComponent(CoordinatesComponent&&) = default;
    ~CoordinatesComponent() = default;

    CoordinatesComponent& operator=(const CoordinatesComponent&) = default;
    CoordinatesComponent& operator=(CoordinatesComponent&&) = default;
  };

  class DrawableComponent
  {
  public:
    RPG::Model*             model;      // Model of the entity
    RPG::Model::Animation*  animation;  // Current animation
    RPG::Texture*           texture;    // Texture of the model
    RPG::Color              outline;    // Outline color

    std::size_t frame;   // Current frame
    float       elapsed; // Time elapsed on current frame
    bool        loop;    // True: loop animation, false: idle when over

    DrawableComponent();
    DrawableComponent(const DrawableComponent&) = default;
    DrawableComponent(DrawableComponent&&) = default;
    ~DrawableComponent() = default;

    DrawableComponent& operator=(const DrawableComponent&) = default;
    DrawableComponent& operator=(DrawableComponent&&) = default;
  };

  class BoundsComponent // Used for selectable entities
  {
  public:
    RPG::Bounds bounds;

    BoundsComponent();
    BoundsComponent(const BoundsComponent&) = default;
    BoundsComponent(BoundsComponent&&) = default;
    ~BoundsComponent() = default;

    BoundsComponent& operator=(const BoundsComponent&) = default;
    BoundsComponent& operator=(BoundsComponent&&) = default;
  };

  class ActionsComponent
  {
  public:
    bool  pickup;     // Entity can be picked-up (item)
    bool  container;  // Entity can be opened (chest)
    bool  dialog;     // Entity can be talked to (dialog)
    bool  fight;      // Entity can be attacked (fighter)
    bool  trigger;    // Entity can be triggered (button)

    ActionsComponent();
    ActionsComponent(const ActionsComponent&) = default;
    ActionsComponent(ActionsComponent&&) = default;
    ~ActionsComponent() = default;

    ActionsComponent& operator=(const ActionsComponent&) = default;
    ActionsComponent& operator=(ActionsComponent&&) = default;
  };
}