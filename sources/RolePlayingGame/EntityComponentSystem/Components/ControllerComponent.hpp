#pragma once

#include <cstddef>

namespace RPG
{
  class ControllerComponent
  {
  public:
    static const std::size_t  NoController;

    std::size_t controller; // ID of network controller of entity, 0 for none

    ControllerComponent();
    ControllerComponent(const ControllerComponent&) = default;
    ControllerComponent(ControllerComponent&&) = default;
    ~ControllerComponent() = default;

    ControllerComponent& operator=(const ControllerComponent&) = default;
    ControllerComponent& operator=(ControllerComponent&&) = default;
  };
}