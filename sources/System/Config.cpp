#include <stdexcept>
#include <iostream>
#include <thread>

#ifdef _WIN32
#include <windows.h>
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

void		Game::Config::initialize(int argc, char **argv)
{
#ifdef _WIN32
  HMODULE	handle;
  WCHAR		path[MAX_PATH];
  std::wstring	result;

  // This error should never happen...
  handle = GetModuleHandle(nullptr);
  if (handle == nullptr)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  GetModuleFileNameW(handle, path, MAX_PATH);
  result = std::wstring(path).substr(0, std::wstring(path).find_last_of('\\'));

  Game::Config::ExecutablePath = std::string(result.begin(), result.end()) + std::string("\\");
#else
  // This error should never happen...
  if (argc < 1)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  if (std::string(argv[0]).find_last_of('/') == std::string::npos)
    RT::Config::ExecutablePath = std::string(".");
  else
    RT::Config::ExecutablePath = std::string(argv[0]).substr(0, std::string(argv[0]).find_last_of('/'));

  RT::Config::ExecutablePath += std::string("/");
#endif

  // Using C++11 to detect maximum of thread concurrency.
  Game::Config::ThreadNumber = std::thread::hardware_concurrency();

  // If failed...
  if (Game::Config::ThreadNumber == 0)
  {
    Game::Config::ThreadNumber = 1;
    std::cerr << "Error: failed to detect maximum of concurrency thread." << std::endl;
  }
}
