#pragma once

#include "RolePlayingGame/TcpClient.hpp"

namespace RPG
{
  class GameClient : public TcpClient
  {


  public:
    GameClient();
    ~GameClient();
  };
}