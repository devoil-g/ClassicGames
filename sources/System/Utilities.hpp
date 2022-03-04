#pragma once

#include <string>
#include <istream>

namespace Game
{
  namespace Utilities
  {
    template<typename Type>
    constexpr Type str_to_key(std::string_view str)
    {
      Type  key = 0;

      // Copy string in key
      for (unsigned int i = 0; i < std::min(sizeof(Type), str.length()); i++)
        ((char*)&key)[i] = str.at(i);

      return key;
    }

    template<typename Type>
    inline std::string key_to_str(Type key)
    {
      char  str[sizeof(Type) + 1] = { 0 };

      // Copy key in string (ignore warning of std::strncpy)
#pragma warning(suppress:4996; suppress:6053)
      return std::strncpy(str, (const char*)&key, sizeof(Type));
    }

    template<typename Type>
    inline std::istream& read(std::istream& stream, Type* ptr, std::size_t number = 1)
    {
      // Safe read, throw exception in case of error
      if (stream.read((char*)ptr, number * sizeof(Type)).good() == false)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      return stream;
    }

    template<typename Type>
    inline Type  swap_endianness(const Type& value)
    {
      Type  result = value;

      // Swap bytes of value
      for (int index = 0; index < sizeof(Type) / 2; index++)
        std::swap(((uint8_t*)&result)[index], ((uint8_t*)&result)[sizeof(Type) - index - 1]);
      return result;
    }
  };
}