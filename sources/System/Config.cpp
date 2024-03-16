#include <stdexcept>
#include <iostream>
#include <thread>
#include <locale>
#include <codecvt>

#ifdef _WIN32
# include <windows.h>
#else
# include <unistd.h>
# include <linux/limits.h>
#endif

#include "System/Config.hpp"

namespace Game
{
  namespace Config
  {
    std::filesystem::path ExecutablePath(".");
    unsigned int          ThreadNumber(0);
  };
};

void  Game::Config::initialize(int argc, char** argv)
{
#ifdef _WIN32
  ::WCHAR   path[MAX_PATH + 1] = { 0 };

  // Get executable path
  ::GetModuleFileNameW(nullptr, path, MAX_PATH);
#else
  char  path[PATH_MAX + 1] = { 0 };

  // Find executable path from /proc/self/exe
  if (::readlink("/proc/self/exe", path, sizeof(path) - 1) == -1)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // Get executable directory
  Game::Config::ExecutablePath = std::filesystem::path(path).parent_path();

  // Initialize random
  std::srand((unsigned int)std::time(nullptr));

  // Detect maximum of thread concurrency
  Game::Config::ThreadNumber = std::thread::hardware_concurrency();

  // If failed...
  if (Game::Config::ThreadNumber == 0) {
    Game::Config::ThreadNumber = 1;
    std::cerr << "[Game::Config]: Warning, failed to detect maximum of concurrency thread." << std::endl;
  }
}