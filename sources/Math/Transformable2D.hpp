#pragma once

#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace Math
{
  template<unsigned int Dimension, typename Type = float>
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
    {}

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
        Math::Matrix<Dimension + 1, Dimension + 1, Type>  origin = Math::Matrix<Dimension + 1, Dimension + 1, Type>::translation();
        Math::Matrix<Dimension + 1, Dimension + 1, Type>  translation;
        Math::Matrix<Dimension + 1, Dimension + 1, Type>  scale;
        Math::Matrix<Dimension + 1, Dimension + 1, Type>  rotation;
        
        _matrix = rotation * scale * translation * origin;


        const Type angle = -_rotation;
        const Type cosine = std::cos(angle);
        const Type sine = std::sin(angle);
        const Type sxc = _scale.x() * cosine;
        const Type syc = _scale.y() * cosine;
        const Type sxs = _scale.x() * sine;
        const Type sys = _scale.y() * sine;
        const Type tx = -_origin.x() * sxc - _origin.y() * sys + _position.x();
        const Type ty = +_origin.x() * sxs - _origin.y() * syc + _position.y();

        // Compute new transformation
        _matrix = Math::Matrix<3, 3>(
          sxc, sys, tx,
          -sxs, syc, ty,
          0.f, 0.f, 1.f
        );

        // Matrix updated
        _updated = true;
      }

      return _matrix;
    }
  };

  template<typename Type>
  class Transformable2D
  {
  private:
    Math::Vector<2, Type>     _origin;
    Math::Vector<2, Type>     _position;
    Math::Vector<2, Type>     _scale;
    Type                      _rotation;
    Math::Matrix<3, 3, Type>  _matrix;
    bool                      _updated;

  public:
    Transformable2D(const Math::Vector<2, Type>& origin = { 0.f, 0.f }, const Math::Vector<2, Type>& position = { 0.f, 0.f }, const Math::Vector<2, Type>& scale = { 1.f, 1.f }, Type rotation = 0.f) :
      _origin(origin),
      _position(position),
      _scale(scale),
      _rotation(rotation),
      _matrix(),
      _updated(false)
    {}

    Transformable2D(const Transformable2D&) = default;
    Transformable2D(Transformable2D&&) = default;
    ~Transformable2D() = default;

    Transformable2D& operator=(const Transformable2D&) = default;
    Transformable2D& operator=(Transformable2D&&) = default;

    void  setOrigin(const Math::Vector<2, Type>& origin) { _origin = origin; _updated = false; }
    void  setPosition(const Math::Vector<2, Type>& position) { _position = position; _updated = false; }
    void  setScale(const Math::Vector<2, Type>& scale) { _scale = scale; _updated = false; }
    void  setRotation(Type angle) { _rotation = Math::Modulo(angle, (Type)(Math::Pi * 2)); _updated = false; }

    void  move(const Math::Vector<2, Type>& direction) { setPosition(getPosition() + direction); }
    void  scale(const Math::Vector<2, Type>& scaling) { setScale(getScale() + scaling); }
    void  rotate(const Math::Vector<2, Type>& angle) { setRotation(getRotation() + angle); }

    const Math::Vector<2, Type>&  getOrigin() const { return _origin; }
    const Math::Vector<2, Type>&  getPosition() const { return _position; }
    const Math::Vector<2, Type>&  getScale() const { return _scale; }
    const Type&                   getRotation() const { return _rotation; }

    const Math::Matrix<3, 3, Type>& getTransform() const
    {
      // Recompute transformation matrix
      if (_updated == false) {
        const Type angle = -_rotation;
        const Type cosine = std::cos(angle);
        const Type sine = std::sin(angle);
        const Type sxc = _scale.x() * cosine;
        const Type syc = _scale.y() * cosine;
        const Type sxs = _scale.x() * sine;
        const Type sys = _scale.y() * sine;
        const Type tx = -_origin.x() * sxc - _origin.y() * sys + _position.x();
        const Type ty = +_origin.x() * sxs - _origin.y() * syc + _position.y();

        // Compute new transformation
        _matrix = Math::Matrix<3, 3>(
          sxc, sys, tx,
          -sxs, syc, ty,
          0.f, 0.f, 1.f
        );

        // Matrix updated
        _updated = true;
      }

      return _matrix;
    }
  };
}