#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

namespace Game
{
  template<typename Key, typename Data>
  class AbstractLibrary
  {
  protected:
    std::unordered_map<Key, Data> _library;

  public:
    AbstractLibrary() = default;
    virtual ~AbstractLibrary() = 0;

    Data& get(const Key& key) // Return data linked to key
    {
      auto it = _library.find(key);

      // Load data if not in library
      if (it == _library.end()) {
        load(key);
        it = _library.find(key);
        if (it == _library.end())
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      }

      return it->second;
    }

    void  clear() // Reset library content
    {
      // Unload each element of the library
      for (auto it = _library.begin; it != _library.end();) {
        auto tmp = it++;

        unload(tmp);
      }
    }

    virtual void  load(const Key&) = 0;   // Load element in library

    void          unload(const Key& key)  // Unload element in library
    {
      auto it = _library.find(key);

      // No entry for given key
      if (it == _library.end())
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

      // Erase element
      _library.erase();
    }
  };
}

template<typename Key, typename Data>
Game::AbstractLibrary<Key, Data>::~AbstractLibrary() {}