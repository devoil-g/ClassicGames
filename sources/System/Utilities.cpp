#include <codecvt>
#include <cstdlib>
#include <locale>

#include "System/Utilities.hpp"

std::string Game::Utilities::Convert(const std::wstring& string)
{
  return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(string);
}

std::wstring  Game::Utilities::Convert(const std::string& string)
{
  return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(string);
}

void  Game::Utilities::Clear()
{
#ifdef _WIN32
  std::system("CLS");
#else
  std::system("clear");
#endif
}