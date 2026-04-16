#pragma once

/**
 * @file Transformable.hpp
 * @brief Defines the Transformable class template for managing 2D and 3D spatial transformations.
 */

#include "Math/Box.hpp"
#include "Math/Math.hpp"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace Math
{
  /**
   * @brief A template class representing a transformable object in 2D or 3D space.
   *
   * Manages origin, position, scale, and rotation properties and lazily computes
   * the combined transformation matrix. The matrix is cached and only recomputed
   * when a property changes.
   *
   * @tparam Dimension The spatial dimension (must be 2 or 3).
   * @tparam Type The numeric type used for calculations (defaults to Math::DefaultType).
   */
  template<unsigned int Dimension, typename Type = Math::DefaultType>
  class Transformable
  {
    static_assert((Dimension == 2 || Dimension == 3), "Only 2D and 3D are supported for Transformable.");

  public:
    /** @brief Number of independent rotation axes, derived from the dimension (1 for 2D, 3 for 3D). */
    constexpr static unsigned int RotationAxis = (Dimension * (Dimension - 1)) / 2;

  private:
    Math::Vector<Dimension, Type>                             _origin;    /**< @brief The local origin (pivot point) of the transformation. */
    Math::Vector<Dimension, Type>                             _position;  /**< @brief The world-space position. */
    Math::Vector<Dimension, Type>                             _scale;     /**< @brief The scale factor per axis. */
    Math::Vector<RotationAxis, Type>                          _rotation;  /**< @brief The rotation angles (in radians), one per rotation axis. */
    mutable Math::Matrix<Dimension + 1, Dimension + 1, Type>  _matrix;   /**< @brief The cached transformation matrix. */
    mutable bool                                              _updated;   /**< @brief Whether the cached matrix is up to date. */

  public:
    /**
     * @brief Constructs a Transformable with the given spatial properties.
     *
     * @param origin   The local origin (pivot point). Defaults to zero.
     * @param position The initial position. Defaults to zero.
     * @param scale    The initial scale. Defaults to one on all axes.
     * @param rotation The initial rotation angles (in radians). Defaults to zero.
     */
    Transformable(const Math::Vector<Dimension, Type>& origin = { static_cast<Type>(0.0) }, const Math::Vector<Dimension, Type>& position = { static_cast<Type>(0.0) }, const Math::Vector<Dimension, Type>& scale = { static_cast<Type>(1.0) }, const Math::Vector<RotationAxis, Type>& rotation = { static_cast<Type>(0.0) }) :
      _origin(origin),
      _position(position),
      _scale(scale),
      _rotation(rotation),
      _matrix(),
      _updated(false)
    {}

    Transformable(const Transformable&) = default;
    Transformable(Transformable&&) = default;
    ~Transformable() = default;

    Transformable& operator=(const Transformable&) = default;
    Transformable& operator=(Transformable&&) = default;

    /**
     * @brief Sets the local origin (pivot point) and invalidates the cached matrix.
     * @param origin The new origin vector.
     */
    void  setOrigin(const Math::Vector<Dimension, Type>& origin) { _origin = origin; _updated = false; }

    /**
     * @brief Sets the world-space position and invalidates the cached matrix.
     * @param position The new position vector.
     */
    void  setPosition(const Math::Vector<Dimension, Type>& position) { _position = position; _updated = false; }

    /**
     * @brief Sets the scale factor and invalidates the cached matrix.
     * @param scale The new scale vector.
     */
    void  setScale(const Math::Vector<Dimension, Type>& scale) { _scale = scale; _updated = false; }

    /**
     * @brief Sets the rotation angles (wrapped to [0, 2*Pi)) and invalidates the cached matrix.
     * @param angle The new rotation angles in radians.
     */
    void  setRotation(const Math::Vector<RotationAxis, Type>& angle) { for (auto index = 0; index < RotationAxis; index++) _rotation(index) = Math::Modulo(angle(index), static_cast<Type>(Math::Pi * 2.0)); _updated = false; }

    /**
     * @brief Translates the position by the given direction vector.
     * @param direction The offset to add to the current position.
     */
    void  move(const Math::Vector<Dimension, Type>& direction) { setPosition(getPosition() + direction); }

    /**
     * @brief Multiplies the current scale by the given scaling factors.
     * @param scaling The per-axis multiplier applied to the current scale.
     */
    void  scale(const Math::Vector<Dimension, Type>& scaling) { setScale(getScale() * scaling); }

    /**
     * @brief Adds the given rotation angles to the current rotation.
     * @param angle The rotation offset in radians.
     */
    void  rotate(const Math::Vector<RotationAxis, Type>& angle) { setRotation(getRotation() + angle); }

    /** @brief Returns a const reference to the local origin vector. */
    const auto& getOrigin() const { return _origin; }

    /** @brief Returns a const reference to the position vector. */
    const auto& getPosition() const { return _position; }

    /** @brief Returns a const reference to the scale vector. */
    const auto& getScale() const { return _scale; }

    /** @brief Returns a const reference to the rotation vector. */
    const auto& getRotation() const { return _rotation; }

    /**
     * @brief Returns the combined transformation matrix, recomputing it if needed.
     *
     * The matrix is composed as Translation * Rotation * Scale and is cached
     * until a property is modified. Translation accounts for the origin offset.
     *
     * @return A const reference to the (Dimension+1) x (Dimension+1) transformation matrix.
     */
    const auto& getTransform() const
    {
      // Recompute transformation matrix
      if (_updated == false) {
        auto  translation = Math::Matrix<Dimension + 1, Dimension + 1, Type>::translation(_position - _origin);
        auto  scale = Math::Matrix<Dimension + 1, Dimension + 1, Type>::scale(_scale);
        Math::Matrix<Dimension + 1, Dimension + 1, Type>  rotation;
        
        if constexpr (Dimension == 2)
          rotation = Math::Matrix<Dimension + 1, Dimension + 1, Type>::rotation(_rotation.x());
        else
          rotation = Math::Matrix<Dimension + 1, Dimension + 1, Type>::rotation(_rotation.x(), _rotation.y(), _rotation.z());
        
        // Update matrix
        _matrix = translation * rotation * scale;
        _updated = true;
      }

      return _matrix;
    }
  };
}