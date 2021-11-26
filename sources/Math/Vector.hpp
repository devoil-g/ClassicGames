#pragma once

#include <array>
#include <iostream>
#include <cstring>
#include <string>

#include "Math/Math.hpp"

namespace Math
{
  template<unsigned int vSize>
  class Vector
  {
  private:
    std::array<float, vSize>  _vector;

  public:
    Vector() :
      _vector{ 0 }
    {
      // Compilation time error if invalid vector size
      static_assert(vSize > 0, "Invalid vector size.");
    };

    template<typename ... Floats>
    Vector(Floats... args)
    {
      float vec[]{ args... };

      // Check for invalid template parameters
      static_assert(sizeof(vec) / sizeof(float) == vSize, "Invalid vector parameters.");

      std::memcpy(_vector.data(), vec, _vector.size() * sizeof(float));
    };

    ~Vector() = default;

    template<unsigned int wSize>
    inline Math::Vector<wSize>& convert() // Cast current vector to a lower dimension
    {
      // Check that requested dimension is valid
      static_assert(wSize <= vSize, "Invalid vector convertion parameters.");

      return *reinterpret_cast<Math::Vector<wSize>*>(this);
    }

    template<unsigned int wSize>
    inline const Math::Vector<wSize>& convert() const // Cast current vector to a lower dimension
    {
      // Check that requested dimension is valid
      static_assert(wSize <= vSize, "Invalid vector convertion parameters.");

      return *reinterpret_cast<const Math::Vector<wSize>*>(this);
    }

    inline float& operator()(unsigned int c) { return _vector[c]; } // Get nth component of vector
    
    inline float  operator()(unsigned int c) const { return _vector[c]; } // Get nth component of vector
    
    inline float& x() { return (*this)(0); }  // Get first component of vector
    inline float& y() { return (*this)(1); }  // Get second component of vector
    inline float& z() { return (*this)(2); }  // Get third component of vector
    inline float& w() { return (*this)(3); }  // Get fourth component of vector

    inline float  x() const { return (*this)(0); }  // Get first component of vector
    inline float  y() const { return (*this)(1); }  // Get second component of vector
    inline float  z() const { return (*this)(2); }  // Get third component of vector
    inline float  w() const { return (*this)(3); }  // Get fourth component of vector

    inline bool operator==(const Math::Vector<vSize>& v)  // Vector comparison
    {
      return (std::memcmp(this->_vector.data(), v._vector.data(), vSize * sizeof(float)) == 0) ? true : false;
    }

    inline Math::Vector<vSize>& operator*=(const Math::Vector<vSize>& v)  // Vector multiplication
    {
      for (unsigned int i = 0; i < vSize; i++)
        (*this)(i) *= v(i);
      return *this;
    }

    inline Math::Vector<vSize>  operator*(const Math::Vector<vSize>& v) const // Vector multiplication
    {
      return Math::Vector<vSize>(*this) *= v;
    }

    inline Math::Vector<vSize>& operator*=(float c) // Vector multiplication
    {
      for (unsigned int i = 0; i < vSize; i++)
        (*this)(i) *= c;
      return *this;
    }

    inline Math::Vector<vSize>  operator*(float c) const  // Vector multiplication
    {
      return Math::Vector<vSize>(*this) *= c;
    }

    inline Math::Vector<vSize>& operator/=(const Math::Vector<vSize>& v)  // Vector division
    {
      for (unsigned int i = 0; i < vSize; i++)
        (*this)(i) /= v(i);
      return *this;
    }

    inline Math::Vector<vSize>  operator/(const Math::Vector<vSize>& v) const // Vector division
    {
      return Math::Vector<vSize>(*this) /= v;
    }

    inline Math::Vector<vSize>& operator/=(float c) // Vector division
    {
      for (unsigned int i = 0; i < vSize; i++)
        (*this)(i) /= c;
      return *this;
    }

    inline Math::Vector<vSize>  operator/(float c) const  // Vector division
    {
      return Math::Vector<vSize>(*this) /= c;
    }

    inline Math::Vector<vSize>& operator+=(const Math::Vector<vSize>& v)  // Vector addition
    {
      for (unsigned int i = 0; i < vSize; i++)
        (*this)(i) += v(i);
      return *this;
    }

    inline Math::Vector<vSize>  operator+(const Math::Vector<vSize>& v) const // Vector addition
    {
      return Math::Vector<vSize>(*this) += v;
    }

    inline Math::Vector<vSize>& operator-=(const Math::Vector<vSize>& v)  // Vector subtraction
    {
      for (unsigned int i = 0; i < vSize; i++)
        (*this)(i) -= v(i);
      return *this;
    }

    inline Math::Vector<vSize>  operator-(const Math::Vector<vSize>& v) const  // Vector subtraction
    {
      return Math::Vector<vSize>(*this) -= v;
    }

    inline float  length() const  // Return ray length
    {
      float r = 0.f;

      for (unsigned int i = 0; i < vSize; i++)
        r += Math::Pow<2>((*this)(i));

      return std::sqrt(r);
    }

    static inline float cos(const Math::Vector<vSize>& A, const Math::Vector<vSize>& B) // Calculate cosinus between two vectors
    {
      float l = 0.f, m = 0.f;

      for (unsigned int n = 0; n < vSize; n++)
      {
        l += Math::Pow<2>(A(n));
        m += Math::Pow<2>(B(n));
      }

#ifdef _DEBUG
      // Should not happen
      if (l * m == 0)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

      return Math::Vector<vSize>::scalar(A, B) / std::sqrt(l * m);
    }

    static inline float angle(const Math::Vector<vSize>& v) // Compute 2D vector angle with X axis [O, 2pi[.
    {
      return std::atan2(v.y(), v.x());
    }

    static inline float angle(const Math::Vector<vSize>& A, const Math::Vector<vSize>& B) // Calculate angle (radian) between two rays
    {
      return std::acos(Math::Vector<vSize>::cos(A, B));
    }

    static inline float scalar(const Math::Vector<vSize>& A, const Math::Vector<vSize>& B)  // Apply scalar product to rays
    {
      float r = 0.f;

      for (unsigned int i = 0; i < vSize; i++)
        r += A(i) * B(i);

      return r;
    }

    template<typename ... Vectors>
    static inline float determinant(Vectors... args)  // Calcultate vector determinant
    {
      Math::Vector<vSize> vec[]{ args... };

      static_assert(sizeof(vec) / sizeof(Math::Vector<vSize>) == vSize, "Invalid vector determinant parameters.");

      // Only support two dimension determinant
      static_assert(vSize == 2, "Vector determinant size not supported.");

      return vec[0].x() * vec[1].y() - vec[0].y() * vec[1].x();
    }

    static Math::Vector<vSize>  cross(const Math::Vector<vSize>& A, const Math::Vector<vSize>& B);  // Apply vectoriel to rays
  };

  static std::pair<float, float>  intersection(const Math::Vector<2>& origin_A, const Math::Vector<2>& direction_A, const Math::Vector<2>& origin_B, const Math::Vector<2>& direction_B)  // Compute intersection points of segments
  {
    float rs = Math::Vector<2>::determinant(direction_A, direction_B);

    // Stop if segments are parallele
    if (rs == 0.f)
      return std::pair<float, float>(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());

    // Compute intersection point of segments
    Math::Vector<2> qp(origin_B - origin_A);

    return std::pair<float, float>(Math::Vector<2>::determinant(qp, direction_B) / rs, Math::Vector<2>::determinant(qp, direction_A) / rs);
  }
}

// Vector to std::ostream
template<unsigned int vSize>
std::ostream& operator<<(std::ostream& stream, const Math::Vector<vSize>& vector)
{
  if (vSize == 0)
    stream << "[]";
  else {
    stream << "[";
    for (int i = 0; i < vSize; i++)
      stream << vector(i) << (i == (vSize - 1) ? "" : ", ");
    stream << "]";
  }
  return stream;
}