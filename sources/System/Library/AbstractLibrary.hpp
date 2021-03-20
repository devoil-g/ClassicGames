#pragma once

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

    virtual ~AbstractLibrary() = 0
    {
      // Properly destruct library elements
      clear();
    }

    const Data& get(const Key& key) // Return data linked to key
    {
      // Load data if not in library
      if (_library.find(key) == _library.end())
        load(key);

      return _library.find(key)->second;
    }

    void  clear() // Reset library content
    {
      // Unload each element of the library
      for (const auto& it : _library)
        unload(it.first);

      // Reset library map
      _library.clear();
    }

    virtual void  load(const Key&) = 0;   // Load element in library
    virtual void  unload(const Key&) {};  // Unload element in library (use default destructor by default)
  };
}