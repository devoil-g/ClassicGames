#pragma once

#include <array>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <string>

#include "Math/Math.hpp"
#include "Math/Matrix.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace Math
{
  template<unsigned int Size, typename Type = float>
  class Vector : public Math::Matrix<Size, 1, Type>
  {
  public:
    Vector() = default;

    template<typename ... Types>
    Vector(Types... args) :
      Math::Matrix<Size, 1, Type>(std::forward<Types>(args)...)
    {}

    Vector(const Game::JSON::Array& json)
    {
      // Check JSON array size
      if (json.size() != Size)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

      // Extract values from JSON
      for (unsigned int i = 0; i < Size; i++)
        (*this)(i) = (Type)json.get(i).number();
    }

    Vector(const Math::Vector<Size, Type>& other) = default;

    Vector(const Math::Matrix<Size, 1, Type>& other) :
      Math::Matrix<Size, 1, Type>(other)
    {}

    ~Vector() = default;

    Vector<Size, Type>& operator=(const Math::Vector<Size, Type>& other) = default;

    template<unsigned int wSize>
    Math::Vector<wSize, Type>&  convert() // Cast current vector to a lower dimension
    {
      // Check that requested dimension is valid
      static_assert(wSize <= Size, "Invalid vector convertion parameters.");

      return *reinterpret_cast<Math::Vector<wSize, Type>*>(this);
    }

    template<unsigned int wSize>
    const Math::Vector<wSize, Type>&  convert() const // Cast current vector to a lower dimension
    {
      // Check that requested dimension is valid
      static_assert(wSize <= Size, "Invalid vector convertion parameters.");

      return *reinterpret_cast<const Math::Vector<wSize, Type>*>(this);
    }

    Type& operator()(unsigned int c) { return Math::Matrix<Size, 1, Type>::operator()(c, 0); }       // Get nth component of vector
    Type  operator()(unsigned int c) const { return Math::Matrix<Size, 1, Type>::operator()(c, 0); } // Get nth component of vector

    template<unsigned int N>
    Type& get() { return Math::Matrix<Size, 1, Type>::template get<N, 0>(); }       // Get nth component of vector

    template<unsigned int N>
    Type  get() const { return Math::Matrix<Size, 1, Type>::template get<N, 0>(); } // Get nth component of vector

    Type& x() { return get<0>(); } // Get first component of vector
    Type& y() { return get<1>(); } // Get second component of vector
    Type& z() { return get<2>(); } // Get third component of vector
    Type& w() { return get<3>(); } // Get fourth component of vector

    Type  x() const { return get<0>(); } // Get first component of vector
    Type  y() const { return get<1>(); } // Get second component of vector
    Type  z() const { return get<2>(); } // Get third component of vector
    Type  w() const { return get<3>(); } // Get fourth component of vector

    bool  operator==(const Math::Vector<Size, Type>& v) const // Vector comparison
    {
      return Math::Matrix<Size, 1, Type>::operator==(v);
    }

    bool  operator!=(const Math::Vector<Size, Type>& v) const // Vector comparison
    {
      return Math::Matrix<Size, 1, Type>::operator!=(v);
    }

    Math::Vector<Size, Type>& operator*=(const Math::Vector<Size, Type>& v) // Vector multiplication
    {
      for (unsigned int i = 0; i < Size; i++)
        (*this)(i) *= v(i);
      return *this;
    }

    Math::Vector<Size, Type>  operator*(const Math::Vector<Size, Type>& v) const // Vector multiplication
    {
      return Math::Vector<Size, Type>(*this) *= v;
    }

    Math::Vector<Size, Type>& operator*=(Type c) // Vector multiplication
    {
      Math::Matrix<Size, 1, Type>::operator*=(c);
      return *this;
    }

    Math::Vector<Size, Type>  operator*(Type c) const // Vector multiplication
    {
      return Math::Vector<Size, Type>(*this) *= c;
    }

    template<unsigned int mSize>
    Math::Vector<Size, Type>& operator*=(const Math::Matrix<mSize, Size, Type>& v) // Vector multiplication
    {
      Math::Matrix<Size, 1, Type>::operator*=(v);
      return *this;
    }

    template<unsigned int mSize>
    Math::Vector<Size, Type>  operator*(const Math::Matrix<mSize, Size, Type>& v) const // Vector multiplication
    {
      return Math::Vector<Size, Type>(*this) *= v;
    }

    Math::Vector<Size, Type>& operator/=(const Math::Vector<Size, Type>& v) // Vector division
    {
      for (unsigned int i = 0; i < Size; i++)
        (*this)(i) /= v(i);
      return *this;
    }

    Math::Vector<Size, Type>  operator/(const Math::Vector<Size, Type>& v) const // Vector division
    {
      return Math::Vector<Size, Type>(*this) /= v;
    }

    Math::Vector<Size, Type>& operator/=(Type c) // Vector division
    {
      Math::Matrix<Size, 1, Type>::operator/=(c);
      return *this;
    }

    Math::Vector<Size, Type>  operator/(Type c) const // Vector division
    {
      return Math::Vector<Size>(*this) /= c;
    }

    Math::Vector<Size, Type>& operator+=(const Math::Vector<Size, Type>& v) // Vector addition
    {
      for (unsigned int i = 0; i < Size; i++)
        (*this)(i) += v(i);
      return *this;
    }

    Math::Vector<Size, Type>  operator+(const Math::Vector<Size, Type>& v) const // Vector addition
    {
      return Math::Vector<Size, Type>(*this) += v;
    }

    Math::Vector<Size, Type>& operator-=(const Math::Vector<Size, Type>& v) // Vector subtraction
    {
      for (unsigned int i = 0; i < Size; i++)
        (*this)(i) -= v(i);
      return *this;
    }

    Math::Vector<Size, Type>  operator-(const Math::Vector<Size, Type>& v) const // Vector subtraction
    {
      return Math::Vector<Size, Type>(*this) -= v;
    }

    Game::JSON::Array json() const
    {
      Game::JSON::Array json;

      // Pre-allocate array
      json._vector.reserve(Size);

      // Dump vector to JSON
      for (unsigned int n = 0; n < Size; n++)
        json.push((double)(*this)(n));

      return json;
    }

    template<typename RetType = Type>
    RetType length() const // Return ray length
    {
      RetType r = static_cast<RetType>(0.0);

      for (unsigned int i = 0; i < Size; i++)
        r += static_cast<RetType>(Math::Pow<2>((*this)(i)));

      return static_cast<RetType>(std::sqrt(r));
    }

    template<typename RetType = Type>
    static RetType  cos(const Math::Vector<Size, Type>& A, const Math::Vector<Size, Type>& B) // Calculate cosinus between two vectors
    {
      RetType l = static_cast<RetType>(0.0), m = static_cast<RetType>(0.0);

      for (unsigned int n = 0; n < Size; n++)
      {
        l += static_cast<RetType>(Math::Pow<2>(A(n)));
        m += static_cast<RetType>(Math::Pow<2>(B(n)));
      }

#ifdef _DEBUG
      // Should not happen
      if (l * m == 0)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

      return Math::Vector<Size, Type>::scalar<RetType>(A, B) / static_cast<RetType>(std::sqrt(l * m));
    }

    template<typename RetType = Type>
    static RetType  angle(const Math::Vector<Size, Type>& v) // Compute 2D vector angle with X axis [O, 2pi[.
    {
      return static_cast<RetType>(std::atan2(v.y(), v.x()));
    }

    template<typename RetType = Type>
    static RetType  angle(const Math::Vector<Size, Type>& A, const Math::Vector<Size, Type>& B) // Calculate angle (radian) between two rays
    {
      return static_cast<RetType>(std::acos(Math::Vector<Size, Type>::cos<RetType>(A, B)));
    }

    template<typename RetType = Type>
    static RetType  scalar(const Math::Vector<Size, Type>& A, const Math::Vector<Size, Type>& B) // Apply scalar product to rays
    {
      RetType r = static_cast<RetType>(0.0);

      for (unsigned int i = 0; i < Size; i++)
        r += A(i) * B(i);

      return r;
    }

    template<typename RetType = Type, typename ... Vectors>
    static RetType  determinant(Vectors... args) // Calcultate vector determinant
    {
      Math::Vector<Size, Type>  vec[]{ args... };

      static_assert(sizeof(vec) / sizeof(Math::Vector<Size, Type>) == Size, "Invalid vector determinant parameters.");

      // Only support two dimension determinant
      static_assert(Size == 2, "Vector determinant size not supported.");

      return static_cast<RetType>(vec[0].x() * vec[1].y() - vec[0].y() * vec[1].x());
    }

    static Math::Vector<Size, Type> cross(const Math::Vector<Size, Type>& A, const Math::Vector<Size, Type>& B) // Apply vectoriel to rays
    {
      // Only support three dimension determinant
      static_assert(Size == 3, "Vector cross size not supported.");

      return Math::Vector<Size, Type>(A.y() * B.z() - A.z() * B.y(), A.z() * B.x() - A.x() * B.z(), A.x() * B.y() - A.y() * B.x());
    }
  };

  template<typename Type, typename RetType = Type>
  static std::pair<RetType, RetType>  intersection(const Math::Vector<2, Type>& origin_A, const Math::Vector<2, Type>& direction_A, const Math::Vector<2, Type>& origin_B, const Math::Vector<2, Type>& direction_B)  // Compute intersection points of segments
  {
    RetType rs = Math::Vector<2>::determinant<RetType>(direction_A, direction_B);

    // Stop if segments are parallele
    if (rs == static_cast<RetType>(0.0))
      return { std::numeric_limits<RetType>::quiet_NaN(), std::numeric_limits<RetType>::quiet_NaN() };

    // Compute intersection point of segments
    Math::Vector<2, Type> qp(origin_B - origin_A);

    return { Math::Vector<2>::determinant<RetType>(qp, direction_B) / rs, Math::Vector<2>::determinant<RetType>(qp, direction_A) / rs };
  }
}

// Vector to std::ostream
template<unsigned int Size, typename Type>
std::ostream& operator<<(std::ostream& stream, const Math::Vector<Size, Type>& vector)
{
  stream << "[";
  for (unsigned int i = 0; i < Size; i++)
    stream << vector(i) << (i == (Size - 1) ? "" : ", ");
  stream << "]";
  return stream;
}