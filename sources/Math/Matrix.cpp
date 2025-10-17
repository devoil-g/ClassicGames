#include "Matrix.hpp"

template<unsigned int Col, unsigned int Row, typename Type>
Math::Matrix<Col, Row, Type>  Math::Matrix<Col, Row, Type>::inverse() const
{
  Type  s0 = (*this)(0, 0) * (*this)(1, 1) - (*this)(1, 0) * (*this)(0, 1);
  Type  s1 = (*this)(0, 0) * (*this)(1, 2) - (*this)(1, 0) * (*this)(0, 2);
  Type  s2 = (*this)(0, 0) * (*this)(1, 3) - (*this)(1, 0) * (*this)(0, 3);
  Type  s3 = (*this)(0, 1) * (*this)(1, 2) - (*this)(1, 1) * (*this)(0, 2);
  Type  s4 = (*this)(0, 1) * (*this)(1, 3) - (*this)(1, 1) * (*this)(0, 3);
  Type  s5 = (*this)(0, 2) * (*this)(1, 3) - (*this)(1, 2) * (*this)(0, 3);

  Type  c0 = (*this)(2, 0) * (*this)(3, 1) - (*this)(3, 0) * (*this)(2, 1);
  Type  c1 = (*this)(2, 0) * (*this)(3, 2) - (*this)(3, 0) * (*this)(2, 2);
  Type  c2 = (*this)(2, 0) * (*this)(3, 3) - (*this)(3, 0) * (*this)(2, 3);
  Type  c3 = (*this)(2, 1) * (*this)(3, 2) - (*this)(3, 1) * (*this)(2, 2);
  Type  c4 = (*this)(2, 1) * (*this)(3, 3) - (*this)(3, 1) * (*this)(2, 3);
  Type  c5 = (*this)(2, 2) * (*this)(3, 3) - (*this)(3, 2) * (*this)(2, 3);

  Type  det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;

#ifdef _DEBUG
  // Not supposed to happen
  if (det == 0.0)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  return Math::Matrix<4, 4, Type>(
    (+(*this)(1, 1) * c5 - (*this)(1, 2) * c4 + (*this)(1, 3) * c3),
    (-(*this)(1, 0) * c5 + (*this)(1, 2) * c2 - (*this)(1, 3) * c1),
    (+(*this)(1, 0) * c4 - (*this)(1, 1) * c2 + (*this)(1, 3) * c0),
    (-(*this)(1, 0) * c3 + (*this)(1, 1) * c1 - (*this)(1, 2) * c0),
    
    (-(*this)(0, 1) * c5 + (*this)(0, 2) * c4 - (*this)(0, 3) * c3),
    (+(*this)(0, 0) * c5 - (*this)(0, 2) * c2 + (*this)(0, 3) * c1),
    (-(*this)(0, 0) * c4 + (*this)(0, 1) * c2 - (*this)(0, 3) * c0),
    (+(*this)(0, 0) * c3 - (*this)(0, 1) * c1 + (*this)(0, 2) * c0),

    (+(*this)(3, 1) * s5 - (*this)(3, 2) * s4 + (*this)(3, 3) * s3),
    (-(*this)(3, 0) * s5 + (*this)(3, 2) * s2 - (*this)(3, 3) * s1),
    (+(*this)(3, 0) * s4 - (*this)(3, 1) * s2 + (*this)(3, 3) * s0),
    (-(*this)(3, 0) * s3 + (*this)(3, 1) * s1 - (*this)(3, 2) * s0),

    (-(*this)(2, 1) * s5 + (*this)(2, 2) * s4 - (*this)(2, 3) * s3),
    (+(*this)(2, 0) * s5 - (*this)(2, 2) * s2 + (*this)(2, 3) * s1),
    (-(*this)(2, 0) * s4 + (*this)(2, 1) * s2 - (*this)(2, 3) * s0),
    (+(*this)(2, 0) * s3 - (*this)(2, 1) * s1 + (*this)(2, 2) * s0)
    ) / det;
}

template Math::Matrix<4, 4, float>  Math::Matrix<4, 4, float>::inverse() const;
template Math::Matrix<4, 4, double> Math::Matrix<4, 4, double>::inverse() const;

template<unsigned int Col, unsigned int Row, typename Type>
Math::Matrix<Col, Row, Type>  Math::Matrix<Col, Row, Type>::reflection(Type x, Type y)
{
  // Generate a reflection matrix
  return Math::Matrix<Col, Row, Type>(
    static_cast<Type>(1.0) - static_cast<Type>(2.0) * x * x,
    static_cast<Type>(-2.0) * y * x,
    static_cast<Type>(0.0),

    static_cast<Type>(-2.0) * x * y,
    static_cast<Type>(1.0) - static_cast<Type>(2.0) * y * y,
    static_cast<Type>(0.0),

    static_cast<Type>(0.0),
    static_cast<Type>(0.0),
    static_cast<Type>(1.0));
}

template Math::Matrix<3, 3, float>  Math::Matrix<3, 3, float>::reflection(float, float);
template Math::Matrix<3, 3, double> Math::Matrix<3, 3, double>::reflection(double, double);

template<unsigned int Col, unsigned int Row, typename Type>
Math::Matrix<Col, Row, Type>  Math::Matrix<Col, Row, Type>::reflection(Type x, Type y, Type z)
{
  // Generate a reflection matrix
  return Math::Matrix<Col, Row, Type>(
    static_cast<Type>(1.0) - static_cast<Type>(2.0) * x * x,
    static_cast<Type>(-2.0) * y * x,
    static_cast<Type>(-2.0) * z * x,
    static_cast<Type>(0.0),

    static_cast<Type>(-2.0) * x * y,
    static_cast<Type>(1.0) - static_cast<Type>(2.0) * y * y,
    static_cast<Type>(-2.0)* z* y,
    static_cast<Type>(0.0),

    static_cast<Type>(-2.0)* x* z,
    static_cast<Type>(-2.0)* y* z,
    static_cast<Type>(1.0) - static_cast<Type>(2.0) * z * z,
    static_cast<Type>(0.0),
    
    static_cast<Type>(0.0),
    static_cast<Type>(0.0),
    static_cast<Type>(0.0),
    static_cast<Type>(1.0));
}

template Math::Matrix<4, 4, float>  Math::Matrix<4, 4, float>::reflection(float, float, float);
template Math::Matrix<4, 4, double> Math::Matrix<4, 4, double>::reflection(double, double, double);

template<unsigned int Col, unsigned int Row, typename Type>
Math::Matrix<Col, Row, Type>  Math::Matrix<Col, Row, Type>::rotation(Type z)
{
  // Generate a rotation matrix
  return Math::Matrix<Col, Row, Type>(
    +std::cos(z),
    -std::sin(z),
    static_cast<Type>(0.0),

    +std::sin(z),
    +std::cos(z),
    static_cast<Type>(0.0),
    
    static_cast<Type>(0.0),
    static_cast<Type>(0.0),
    static_cast<Type>(1.0));
}

template Math::Matrix<3, 3, float>  Math::Matrix<3, 3, float>::rotation(float);
template Math::Matrix<3, 3, double> Math::Matrix<3, 3, double>::rotation(double);

template<unsigned int Col, unsigned int Row, typename Type>
Math::Matrix<Col, Row, Type>  Math::Matrix<Col, Row, Type>::rotation(Type x, Type y, Type z)
{
  // Generate rotation matrix
  return
    Math::Matrix<Col, Row, Type>(
      +std::cos(z),
      +std::sin(z),
      static_cast<Type>(0.0),
      static_cast<Type>(0.0),

      -std::sin(z),
      +std::cos(z),
      static_cast<Type>(0.0),
      static_cast<Type>(0.0),

      static_cast<Type>(0.0),
      static_cast<Type>(0.0),
      static_cast<Type>(1.0),
      static_cast<Type>(0.0),
      
      static_cast<Type>(0.0),
      static_cast<Type>(0.0),
      static_cast<Type>(0.0),
      static_cast<Type>(1.0))

    * Math::Matrix<Col, Row, Type>(
      +std::cos(y),
      static_cast<Type>(0.0),
      -std::sin(y),
      static_cast<Type>(0.0),

      static_cast<Type>(0.0),
      static_cast<Type>(1.0),
      static_cast<Type>(0.0),
      static_cast<Type>(0.0),

      +std::sin(y),
      static_cast<Type>(0.0),
      +std::cos(y),
      static_cast<Type>(0.0),
      
      static_cast<Type>(0.0),
      static_cast<Type>(0.0),
      static_cast<Type>(0.0),
      static_cast<Type>(1.0))

    * Math::Matrix<Col, Row, Type>(
      static_cast<Type>(1.0),
      static_cast<Type>(0.0),
      static_cast<Type>(0.0),
      static_cast<Type>(0.0),

      static_cast<Type>(0.0),
      +std::cos(x),
      -std::sin(x),
      static_cast<Type>(0.0),

      static_cast<Type>(0.0),
      +std::sin(x),
      +std::cos(x),
      static_cast<Type>(0.0),
      
      static_cast<Type>(0.0),
      static_cast<Type>(0.0),
      static_cast<Type>(0.0),
      static_cast<Type>(1.0));
}

template Math::Matrix<4, 4, float>  Math::Matrix<4, 4, float>::rotation(float, float, float);
template Math::Matrix<4, 4, double> Math::Matrix<4, 4, double>::rotation(double, double, double);

template<unsigned int Col, unsigned int Row, typename Type>
Math::Matrix<Col, Row, Type>  Math::Matrix<Col, Row, Type>::rotation(Type a, Type x, Type y, Type z)
{
  Type  l = std::sqrt(x * x + y * y + z * z);

#ifdef _DEBUG
  // Should not happen
  if (l == 0)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // Normalize vector
  x /= l;
  y /= l;
  z /= l;

  // Generate a rotation matrix
  return Math::Matrix<Col, Row, Type>(
    x * x * (static_cast<Type>(1.0) - std::cos(a)) + std::cos(a),
    y * x * (static_cast<Type>(1.0) - std::cos(a)) - z * std::sin(a),
    z * x * (static_cast<Type>(1.0) - std::cos(a)) + y * std::sin(a),
    static_cast<Type>(0.0),

    x * y * (static_cast<Type>(1.0) - std::cos(a)) + z * std::sin(a),
    y * y * (static_cast<Type>(1.0) - std::cos(a)) + std::cos(a),
    z * y * (static_cast<Type>(1.0) - std::cos(a)) - x * std::sin(a),
    static_cast<Type>(0.0),

    x * z * (static_cast<Type>(1.0) - std::cos(a)) - y * std::sin(a),
    y * z * (static_cast<Type>(1.0) - std::cos(a)) + x * std::sin(a),
    z * z * (static_cast<Type>(1.0) - std::cos(a)) + std::cos(a),
    static_cast<Type>(0.0),

    static_cast<Type>(0.0),
    static_cast<Type>(0.0),
    static_cast<Type>(0.0),
    static_cast<Type>(1.0));
}

template Math::Matrix<4, 4, float>  Math::Matrix<4, 4, float>::rotation(float, float, float, float);
template Math::Matrix<4, 4, double> Math::Matrix<4, 4, double>::rotation(double, double, double, double);

template<unsigned int Col, unsigned int Row, typename Type>
Math::Matrix<Col, Row, Type>  Math::Matrix<Col, Row, Type>::shear(Type xy, Type yx)
{
  // Generate shearing matrix
  return Math::Matrix<Col, Row, Type>(
    static_cast<Type>(1.0),
    xy,
    static_cast<Type>(0.0),

    yx,
    static_cast<Type>(1.0),
    static_cast<Type>(0.0),
    
    static_cast<Type>(0.0),
    static_cast<Type>(0.0),
    static_cast<Type>(1.0));
}

template Math::Matrix<3, 3, float>  Math::Matrix<3, 3, float>::shear(float, float);
template Math::Matrix<3, 3, double> Math::Matrix<3, 3, double>::shear(double, double);

template<unsigned int Col, unsigned int Row, typename Type>
Math::Matrix<Col, Row, Type>  Math::Matrix<Col, Row, Type>::shear(Type xy, Type xz, Type yx, Type yz, Type zx, Type zy)
{
  // Generate shearing matrix
  return Math::Matrix<Col, Row, Type>(
    static_cast<Type>(1.0),
    xy,
    xz,
    static_cast<Type>(0.0),

    yx,
    static_cast<Type>(1.0),
    yz,
    static_cast<Type>(0.0),

    zx,
    zy,
    static_cast<Type>(1.0),
    static_cast<Type>(0.0),
    
    static_cast<Type>(0.0),
    static_cast<Type>(0.0),
    static_cast<Type>(0.0),
    static_cast<Type>(1.0));
}

template Math::Matrix<4, 4, float>  Math::Matrix<4, 4, float>::shear(float, float, float, float, float, float);
template Math::Matrix<4, 4, double> Math::Matrix<4, 4, double>::shear(double, double, double, double, double, double);