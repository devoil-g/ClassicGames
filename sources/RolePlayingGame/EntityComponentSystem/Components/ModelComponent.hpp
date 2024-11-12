#pragma once

#include "RolePlayingGame/Color.hpp"
#include "RolePlayingGame/Model.hpp"
#include "RolePlayingGame/Types.hpp"

namespace RPG
{
  class ModelComponent
  {
  public:
    RPG::Model::Actor actor;      // Actor of component's model
    RPG::Position     position;   // Position of entity in 3D world
    RPG::Direction    direction;  // Direction the entity is facing
    RPG::Color        color;      // Sprite color
    RPG::Color        outline;    // Outline color
    
    ModelComponent();
    ModelComponent(const ModelComponent&) = default;
    ModelComponent(ModelComponent&&) = default;
    ~ModelComponent() = default;

    ModelComponent& operator=(const ModelComponent&) = default;
    ModelComponent& operator=(ModelComponent&&) = default;
  };
}