#pragma once

#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace Math
{
  template<unsigned int Dimension, typename Type = Math::DefaultType>
  class Transformable
  {
  public:
    constexpr static unsigned int RotationAxis = (Dimension * (Dimension - 1)) / 2;

  private:
    Math::Vector<Dimension, Type>                     _origin;
    Math::Vector<Dimension, Type>                     _position;
    Math::Vector<Dimension, Type>                     _scale;
    Math::Vector<RotationAxis, Type>                  _rotation;
    Math::Matrix<Dimension + 1, Dimension + 1, Type>  _matrix;
    bool                                              _updated;

  public:
    Transformable(const Math::Vector<Dimension, Type>& origin = { static_cast<Type>(0.0) }, const Math::Vector<Dimension, Type>& position = { static_cast<Type>(0.0) }, const Math::Vector<Dimension, Type>& scale = { static_cast<Type>(1.0) }, const Math::Vector<RotationAxis, Type>& rotation = { static_cast<Type>(0.0) }) :
      _origin(origin),
      _position(position),
      _scale(scale),
      _rotation(rotation),
      _matrix(),
      _updated(false)
    {
      static_assert((Dimension == 2 || Dimension == 3), "Only 2D and 3D are supported for Transformable.");
    }

    Transformable(const Transformable&) = default;
    Transformable(Transformable&&) = default;
    ~Transformable() = default;

    Transformable& operator=(const Transformable&) = default;
    Transformable& operator=(Transformable&&) = default;

    void  setOrigin(const Math::Vector<Dimension, Type>& origin) { _origin = origin; _updated = false; }
    void  setPosition(const Math::Vector<Dimension, Type>& position) { _position = position; _updated = false; }
    void  setScale(const Math::Vector<Dimension, Type>& scale) { _scale = scale; _updated = false; }
    void  setRotation(const Math::Vector<RotationAxis, Type>& angle) { for (auto index = 0; index < RotationAxis; index++) _rotation(index) = Math::Modulo(angle(index), static_cast<Type>(Math::Pi * 2.0)); _updated = false; }

    void  move(const Math::Vector<2, Type>& direction) { setPosition(getPosition() + direction); }
    void  scale(const Math::Vector<2, Type>& scaling) { setScale(getScale() + scaling); }
    void  rotate(const Math::Vector<RotationAxis, Type>& angle) { setRotation(getRotation() + angle); }

    const auto& getOrigin() const { return _origin; }
    const auto& getPosition() const { return _position; }
    const auto& getScale() const { return _scale; }
    const auto& getRotation() const { return _rotation; }

    const auto& getTransform() const
    {
      // Recompute transformation matrix
      if (_updated == false) {
        auto  origin = Math::Matrix<Dimension + 1, Dimension + 1, Type>::translation(_origin);
        auto  translation = Math::Matrix<Dimension + 1, Dimension + 1, Type>::translation(_position);
        auto  scale = Math::Matrix<Dimension + 1, Dimension + 1, Type>::scale(_scale);
        auto  rotation = (Dimension == 2) ?
          Math::Matrix<Dimension + 1, Dimension + 1, Type>::rotation(_rotation.x()) :
          Math::Matrix<Dimension + 1, Dimension + 1, Type>::rotation(_rotation.x(), _rotation.y(), _rotation.z());
        
        // Update matrix
        _matrix = translation * rotation * scale * origin;
        _updated = true;
      }

      return _matrix;
    }
  };
}