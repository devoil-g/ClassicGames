#pragma once

#include <iostream>

#include "Math/Vector.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace Math
{
  template<unsigned int Dimension, typename Type = Math::DefaultType>
  class Box
  {
  public:
    Math::Vector<Dimension, Type> position;
    Math::Vector<Dimension, Type> size;

    Box() = default;
    Box(const Box&) = default;
    Box(Box&&) = default;
    ~Box() = default;

    Box(const Math::Vector<Dimension, Type>& position, const Math::Vector<Dimension, Type>& size) :
      position(position),
      size(size)
    {}

    Box(const Game::JSON::Object& json) :
      position(json.get(L"position").array()),
      size(json.get(L"size").array())
    {}

    Box&  operator=(const Box&) = default;
    Box&  operator=(Box&&) = default;

    Box& operator=(const Game::JSON::Object& json)
    {
      // Deserialize JSON
      position = json.get(L"position").array();
      size = json.get(L"size").array();
    }

    bool  contains(const Math::Vector<Dimension, Type>& point)
    {
      // Check every dimensions
      for (unsigned int dim = 0; dim < Dimension; dim++) {
        if (point(dim) < std::min(position(dim), position(dim) + size(dim)) ||
          point(dim) >= std::max(position(dim), position(dim) + size(dim))) {
          return false;
        }
      }

      // In bounds
      return true;
    }

    Game::JSON::Object json() const
    {
      Game::JSON::Object json;

      // Serialize to JSON
      json.set(L"position", position.json());
      json.set(L"size", size.json());

      return json;
    }
  };
}

// Box to std::ostream
template<unsigned int Dimension, typename Type>
std::ostream& operator<<(std::ostream& stream, const Math::Box<Dimension, Type>& box)
{
  return stream << "{" << box.position << "," << box.size << "}";
}