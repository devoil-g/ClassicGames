#pragma once

#include "RolePlayingGame/Color.hpp"
#include "RolePlayingGame/Model.hpp"
#include "RolePlayingGame/Types.hpp"

namespace RPG
{
  class DisplayComponent
  {
  public:
    RPG::Position   position;   // Position of entity in 3D world
    RPG::Direction  direction;  // Direction the entity is facing
    RPG::Color      color;      // Sprite color
    RPG::Color      outline;    // Outline color
    
    const RPG::Model*     model;      // Current model
    const RPG::Animation* animation;  // Current animation
    
    std::size_t frame;    // Current frame
    float       elapsed;  // Time elapsed on current frame
    bool        loop;     // True: loop animation, false: idle when over

    DisplayComponent();
    DisplayComponent(const DisplayComponent&) = default;
    DisplayComponent(DisplayComponent&&) = default;
    ~DisplayComponent() = default;

    DisplayComponent& operator=(const DisplayComponent&) = default;
    DisplayComponent& operator=(DisplayComponent&&) = default;
  };
}