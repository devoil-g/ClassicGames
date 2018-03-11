#ifndef _ABSTRACT_LIBRARY_HPP_
#define _ABSTRACT_LIBRARY_HPP_

#include <unordered_map>

namespace Game
{
  template<typename Key, typename Data>
  class AbstractLibrary
  {
  protected:
    std::unordered_map<Key, Data>	_library;

  public:
    AbstractLibrary()
      : _library()
    {}

    virtual ~AbstractLibrary()
    {
      clear();
    }

    Data const &	get(Key const & key)		// Return data linked to key
    {
      // Load data if not in library
      if (_library.find(key) == _library.end())
	load(key);

      return _library.find(key)->second;
    }

    void		clear()				// Reset library content
    {
      // Unload each element of the library
      for (std::pair<Key, Data> const & it : _library)
	unload(it.first);

      // Reset library map
      _library.clear();
    }

    virtual void	load(Key const &) = 0;		// Load element in library
    virtual void	unload(Key const &) {};		// Unload element in library (use default destructor by default)
  };
};

#endif
