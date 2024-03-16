#pragma once

#include <unordered_map>

#include "RolePlayingGame/Level.hpp"
#include "RolePlayingGame/Model.hpp"

namespace RPG
{
  class World
  {
  private:
    std::unordered_map<std::string, RPG::Level> _levels;
    std::unordered_map<std::string, RPG::Model> _models;
  };
}