#pragma once

#include <filesystem>

namespace Game
{
  namespace Config
  {
    void  initialize(int, char **);

    extern std::filesystem::path  ExecutablePath; // Path to the executable (without executable name, '/' or '\' terminated)
    extern unsigned int           ThreadNumber;   // Number of maximum parallele threads
  };
}