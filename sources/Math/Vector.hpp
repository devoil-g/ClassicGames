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
  template<unsigned int Size, typename Type = Math::DefaultType>
  class Vector : public Math::Matrix<1, Size, Type>
  {
  private:
    using Self = Math::Vector<Size, Type>;
    using Base = Math::Matrix<1, Size, Type>;

  public:
    Vector() = default;

    template<typename ... Types>
    Vector(Types... args) :
      Base(std::forward<Types>(args)...)
    {}

    Vector(const Type* values) :
      Base(values)
    {}

    Vector(const Game::JSON::Array& json)
    {
      // Copy JSON to vector
      *this = json;
    }

    Vector(const Self& other) = default;
    Vector(Self&& other) = default;

    Vector(const Base& other) :
      Base(other)
    {
    }

    Vector(const Base&& other) :
      Base(other)
    {
    }

    ~Vector() = default;

    Self& operator=(const Self& other) = default;
    Self& operator=(Self&& other) = default;

    auto& operator=(const Game::JSON::Array& json)
    {
      // Check JSON array size
      if (json.size() != Size)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

      // Extract values from JSON
      for (auto i = 0; i < Size; i++)
        (*this)(i) = static_cast<Type>(json.get(i).number());

      return *this;
    }

    template<unsigned int wSize>
    auto& convert() // Cast current vector to a lower dimension
    {
      // Check that requested dimension is valid
      static_assert(wSize <= Size, "Invalid vector conversion parameters.");

      return *reinterpret_cast<Math::Vector<wSize, Type>*>(this);
    }

    template<unsigned int wSize>
    const auto& convert() const // Cast current vector to a lower dimension
    {
      // Check that requested dimension is valid
      static_assert(wSize <= Size, "Invalid vector convertion parameters.");

      return *reinterpret_cast<const Math::Vector<wSize, Type>*>(this);
    }

    auto& operator()(unsigned int c) { return Base::operator()(0, c); }       // Get nth component of vector
    auto  operator()(unsigned int c) const { return Base::operator()(0, c); } // Get nth component of vector

    auto& x() { return (*this)(0); } // Get first component of vector
    auto& y() { return (*this)(1); } // Get second component of vector
    auto& z() { return (*this)(2); } // Get third component of vector
    auto& w() { return (*this)(3); } // Get fourth component of vector

    auto  x() const { return (*this)(0); } // Get first component of vector
    auto  y() const { return (*this)(1); } // Get second component of vector
    auto  z() const { return (*this)(2); } // Get third component of vector
    auto  w() const { return (*this)(3); } // Get fourth component of vector

    auto  operator==(const Self& v) const // Vector comparison
    {
      return Base::operator==(v);
    }

    auto  operator!=(const Self& v) const // Vector comparison
    {
      return Base::operator!=(v);
    }

    auto& operator*=(const Self& v) // Vector multiplication
    {
      for (unsigned int i = 0; i < Size; i++)
        (*this)(i) *= v(i);
      return *this;
    }

    auto  operator*(const Self& v) const // Vector multiplication
    {
      return Self(*this) *= v;
    }

    auto& operator*=(Type c) // Vector multiplication
    {
      Base::operator*=(c);
      return *this;
    }

    auto  operator*(Type c) const // Vector multiplication
    {
      return Self(*this) *= c;
    }

    template<unsigned int mSize>
    auto& operator*=(const Math::Matrix<Size, mSize, Type>& v) // Vector multiplication
    {
      Base::operator*=(v);
      return *this;
    }

    template<unsigned int mSize>
    auto  operator*(const Math::Matrix<Size, mSize, Type>& v) const // Vector multiplication
    {
      return Self(*this) *= v;
    }

    auto& operator/=(const Self& v) // Vector division
    {
      for (unsigned int i = 0; i < Size; i++)
        (*this)(i) /= v(i);
      return *this;
    }

    auto  operator/(const Self& v) const // Vector division
    {
      return Self(*this) /= v;
    }

    auto& operator/=(Type c) // Vector division
    {
      Base::operator/=(c);
      return *this;
    }

    auto  operator/(Type c) const // Vector division
    {
      return Math::Vector<Size>(*this) /= c;
    }

    auto& operator+=(const Self& v) // Vector addition
    {
      for (unsigned int i = 0; i < Size; i++)
        (*this)(i) += v(i);
      return *this;
    }

    auto  operator+(const Self& v) const // Vector addition
    {
      return Self(*this) += v;
    }

    auto& operator-=(const Self& v) // Vector subtraction
    {
      for (unsigned int i = 0; i < Size; i++)
        (*this)(i) -= v(i);
      return *this;
    }

    auto  operator-(const Self& v) const // Vector subtraction
    {
      return Self(*this) -= v;
    }

    Game::JSON::Array json() const
    {
      Game::JSON::Array json;

      // Pre-allocate array
      json.reserve(Size);

      // Dump vector to JSON
      for (auto n = 0; n < Size; n++)
        json.push((double)(*this)(n));

      return json;
    }

    template<typename RetType = Type>
    auto length() const // Return ray length
    {
      auto r = static_cast<RetType>(0.0);

      for (auto i = 0; i < Size; i++)
        r += static_cast<RetType>(Math::Pow<2>((*this)(i)));

      return static_cast<RetType>(std::sqrt(r));
    }

    template<typename RetType = Type>
    static auto cos(const Self& A, const Self& B) // Calculate cosinus between two vectors
    {
      auto l = static_cast<RetType>(0.0), m = static_cast<RetType>(0.0);

      for (auto n = 0; n < Size; n++)
      {
        l += static_cast<RetType>(Math::Pow<2>(A(n)));
        m += static_cast<RetType>(Math::Pow<2>(B(n)));
      }

#ifdef _DEBUG
      // Should not happen
      if (l * m == 0)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

      return Self::scalar<RetType>(A, B) / static_cast<RetType>(std::sqrt(l * m));
    }

    template<typename RetType = Type>
    static auto angle(const Self& v) // Compute 2D vector angle with X axis [O, 2pi[.
    {
      return static_cast<RetType>(std::atan2(v.y(), v.x()));
    }

    template<typename RetType = Type>
    static auto angle(const Self& A, const Self& B) // Calculate angle (radian) between two rays
    {
      return static_cast<RetType>(std::acos(Self::cos<RetType>(A, B)));
    }

    template<typename RetType = Type>
    static auto scalar(const Self& A, const Self& B) // Apply scalar product to rays
    {
      auto r = static_cast<RetType>(0.0);

      for (unsigned int i = 0; i < Size; i++)
        r += A(i) * B(i);

      return r;
    }

    template<typename RetType = Type, typename ... Vectors>
    static auto determinant(Vectors... args) // Calcultate vector determinant
    {
      Self  vec[]{ std::forward<Vectors>(args)... };

      static_assert(sizeof(vec) / sizeof(*vec) == Size, "Invalid vector determinant parameters.");

      // Only support two dimension determinant
      static_assert(Size == 2, "Vector determinant size not supported.");

      return static_cast<RetType>(vec[0].x() * vec[1].y() - vec[0].y() * vec[1].x());
    }

    static auto cross(const Self& A, const Self& B) // Apply vectoriel to rays
    {
      // Only support three dimension determinant
      static_assert(Size == 3, "Vector cross size not supported.");

      return Self(A.y() * B.z() - A.z() * B.y(), A.z() * B.x() - A.x() * B.z(), A.x() * B.y() - A.y() * B.x());
    }
  };

  template<typename Type>
  static auto intersection(const Math::Vector<2, Type>& origin_A, const Math::Vector<2, Type>& direction_A, const Math::Vector<2, Type>& origin_B, const Math::Vector<2, Type>& direction_B)  // Compute intersection points of segments
  {
    auto rs = Math::Vector<2, Type>::determinant(direction_A, direction_B);

    // Stop if segments are parallele
    if (rs == static_cast<Type>(0.0))
      return std::pair<Type, Type>{ std::numeric_limits<Type>::quiet_NaN(), std::numeric_limits<Type>::quiet_NaN() };

    // Compute intersection point of segments
    auto qp(origin_B - origin_A);

    return std::pair<Type, Type>{ Math::Vector<2, Type>::determinant(qp, direction_B) / rs, Math::Vector<2, Type>::determinant(qp, direction_A) / rs };
  }
}

// Vector to std::ostream
template<unsigned int Size, typename Type>
std::ostream& operator<<(std::ostream& stream, const Math::Vector<Size, Type>& vector)
{
  stream << "[";
  for (auto i = 0; i < Size; i++)
    stream << vector(i) << (i == (Size - 1) ? "" : ", ");
  stream << "]";
  return stream;
}