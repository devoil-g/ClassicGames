#pragma once

/**
 * @file Box.hpp
 * @brief Templated axis-aligned box (AABB) class for N-dimensional space.
 *
 * Provides a generic N-dimensional axis-aligned bounding box defined by a
 * position and a size, built on top of Math::Vector<Dimension, Type>.
 * Supports point containment testing, JSON serialization/deserialization
 * via the Game::JSON interface, and stream output.
 *
 * @see Math::Vector
 */

#include <iostream>

#include "Math/Math.hpp"
#include "Math/Vector.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace Math
{
  /**
   * @brief Generic N-dimensional axis-aligned bounding box (AABB).
   *
   * Represents an axis-aligned box defined by a position vector and a size
   * vector. Provides default and copy/move constructors, JSON deserialization,
   * equality comparison, point containment testing, and JSON serialization.
   *
   * @tparam Dimension Number of spatial dimensions of the box.
   * @tparam Type      Element type (defaults to Math::DefaultType, i.e. float).
   */
  template<unsigned int Dimension, typename Type = Math::DefaultType>
  class Box
  {
  public:
    Math::Vector<Dimension, Type> position; /**< @brief Origin corner of the box. */
    Math::Vector<Dimension, Type> size;     /**< @brief Extent (width, height, ...) of the box along each axis. */

    /** @brief Default constructor – leaves position and size zero-initialized. */
    Box() = default;

    /** @brief Copy constructor (defaulted). */
    Box(const Box&) = default;

    /** @brief Move constructor (defaulted). */
    Box(Box&&) = default;

    /** @brief Destructor (defaulted). */
    ~Box() = default;

    /**
     * @brief Constructs a box from a position and a size.
     * @param position Origin corner of the box.
     * @param size     Extent of the box along each axis.
     */
    Box(const Math::Vector<Dimension, Type>& position, const Math::Vector<Dimension, Type>& size) :
      position(position),
      size(size)
    {
    }

    /**
     * @brief Constructs a box from a JSON object.
     *
     * Expects the JSON object to contain "position" and "size" array fields.
     *
     * @param json JSON object holding the box data.
     */
    Box(const Game::JSON::Object& json) :
      position(json.get(L"position").array()),
      size(json.get(L"size").array())
    {
    }

    /** @brief Copy-assignment operator (defaulted). */
    Box& operator=(const Box&) = default;

    /** @brief Move-assignment operator (defaulted). */
    Box& operator=(Box&&) = default;

    /** @brief Equality comparison operator (defaulted, member-wise). */
    bool  operator==(const Math::Box<Dimension, Type>& other) const = default;

    /** @brief Inequality comparison operator (defaulted, member-wise). */
    bool  operator!=(const Math::Box<Dimension, Type>& other) const = default;

    /**
     * @brief Tests whether a point is contained within the box.
     *
     * The containment check is inclusive on the lower bound and exclusive on the
     * upper bound for each dimension. Negative sizes are handled correctly by
     * comparing against min/max of the position and position+size.
     *
     * @param point The point to test for containment.
     * @return true if the point lies inside the box, false otherwise.
     */
    auto  contains(const Math::Vector<Dimension, Type>& point) const
    {
      // Check every dimensions
      for (auto dim = 0; dim < Dimension; dim++) {
        if (point(dim) < std::min(position(dim), (Type)(position(dim) + size(dim))) ||
          point(dim) >= std::max(position(dim), (Type)(position(dim) + size(dim)))) {
          return false;
        }
      }

      // In bounds
      return true;
    }

    /**
     * @brief Serializes the box to a JSON object.
     *
     * Produces a Game::JSON::Object with "position" and "size" fields,
     * each serialized via the corresponding vector's json() method.
     *
     * @return A Game::JSON::Object representing this box.
     */
    auto  json() const
    {
      Game::JSON::Object json;

      // Serialize to JSON
      json.set(L"position", position.json());
      json.set(L"size", size.json());

      return json;
    }
  };
}

/**
 * @brief Outputs a box to an std::ostream in the format {position,size}.
 *
 * @tparam Dimension Number of dimensions of the box.
 * @tparam Type      Element type of the box.
 * @param stream The output stream to write to.
 * @param box    The box to output.
 * @return Reference to the output stream.
 */
template<unsigned int Dimension, typename Type>
std::ostream& operator<<(std::ostream& stream, const Math::Box<Dimension, Type>& box)
{
  return stream << "{" << box.position << "," << box.size << "}";
}