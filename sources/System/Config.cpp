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
    std::string	  ExecutablePath(".");
    unsigned int  ThreadNumber(0);
  };
};

void  Game::Config::initialize(int argc, char** argv)
{
#ifdef _WIN32
  HMODULE handle;
  WCHAR   path[MAX_PATH] = { 0 };

  // This error should never happen...
  handle = GetModuleHandle(nullptr);
  if (handle == nullptr)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  GetModuleFileNameW(handle, path, MAX_PATH);
  Game::Config::ExecutablePath = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(std::wstring(path).substr(0, std::wstring(path).find_last_of('\\') + 1));
#else
  char  path[PATH_MAX + 1] = { 0 };

  // Find executable path from /proc/self/exe
  if (readlink("/proc/self/exe", path, sizeof(path) - 1) == -1)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  
  Game::Config::ExecutablePath = std::string(path).substr(0, std::string(path).find_last_of('/') + 1);
#endif

  // Initialize random
  std::srand((unsigned int)std::time(nullptr));

  // Detect maximum of thread concurrency.
  Game::Config::ThreadNumber = std::thread::hardware_concurrency();

  // If failed...
  if (Game::Config::ThreadNumber == 0) {
    Game::Config::ThreadNumber = 1;
    std::cerr << "[Game::Config]: Warning, failed to detect maximum of concurrency thread." << std::endl;
  }
}
