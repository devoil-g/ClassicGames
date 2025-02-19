#pragma once

#include "RolePlayingGame/Color.hpp"
#include "RolePlayingGame/Model.hpp"

namespace RPG
{
  class CursorComponent
  {
  public:
    bool  grid; // 

    CursorComponent();
    CursorComponent(const CursorComponent&) = default;
    CursorComponent(CursorComponent&&) = default;
    ~CursorComponent() = default;

    CursorComponent& operator=(const CursorComponent&) = default;
    CursorComponent& operator=(CursorComponent&&) = default;
  };

  /*
  class MoveComponent
  {
  public:
    Math::Vector<3, float>  position;   // Target position
    float                   remaining;  // Remaining time

    MoveComponent();
    MoveComponent(const MoveComponent&) = default;
    MoveComponent(MoveComponent&&) = default;
    ~MoveComponent() = default;

    MoveComponent&  operator=(const MoveComponent&) = default;
    MoveComponent&  operator=(MoveComponent&&) = default;
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
  */
}