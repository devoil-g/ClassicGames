#pragma once

#include <string>

namespace Game
{
  namespace Config
  {
    void                initialize(int, char **);

    extern std::string  ExecutablePath; // Path to the executable (without executable name, '/' or '\' terminated)
    extern unsigned int ThreadNumber;   // Number of maximum parallele threads
  };
}