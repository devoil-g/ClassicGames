#pragma once

#include <cstddef>

namespace RPG
{
  class NetworkComponent
  {
  public:
    static const std::size_t  NoController;

    std::size_t controller; // ID of network controller of entity, 0 for none

    NetworkComponent();
    NetworkComponent(const NetworkComponent&) = default;
    NetworkComponent(NetworkComponent&&) = default;
    ~NetworkComponent() = default;

    NetworkComponent& operator=(const NetworkComponent&) = default;
    NetworkComponent& operator=(NetworkComponent&&) = default;
  };
}