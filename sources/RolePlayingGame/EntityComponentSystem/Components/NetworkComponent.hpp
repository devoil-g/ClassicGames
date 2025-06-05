#pragma once

#include <cstddef>

#include "RolePlayingGame/TcpServer.hpp"

namespace RPG
{
  class NetworkComponent
  {
  public:
    static constexpr std::size_t  NoController = RPG::TcpServer::InvalidId;

    std::size_t controller; // ID of network controller of entity, 0 for none

    NetworkComponent();
    NetworkComponent(const NetworkComponent&) = default;
    NetworkComponent(NetworkComponent&&) = default;
    ~NetworkComponent() = default;

    NetworkComponent& operator=(const NetworkComponent&) = default;
    NetworkComponent& operator=(NetworkComponent&&) = default;
  };
}