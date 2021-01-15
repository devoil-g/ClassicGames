#pragma once

#define _USE_MATH_DEFINES

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>

#ifndef M_PI
# define M_PI	3.14159265358979323846
#endif

#if __cplusplus < __cpp_2017
namespace std
{
  template<typename Type>
  inline constexpr const Type& clamp(const Type& value, const Type& low, const Type& high)
  {
    return std::min(high, std::max(low, value));
  }
};
#endif

namespace Math
{
  constexpr float const Pi = (float)M_PI;	// Use this instead of M_PI

  template<int Mod>
  inline int  Modulo(int i) // Return the positive modulo Mod of i
  {
    int	r = i % Mod;

    return r < 0 ? r + Mod : r;
  }

  inline int  Modulo(int i, int n)  // Return the positive modulo n of i
  {
    int	r = i % n;

    return r < 0 ? r + n : r;
  };

  inline float  Modulo(float i, float n)  // Return the positive modulo i of n
  {
    float r = i - (int)(i / n) * i;

    return r < 0.f ? r + n : r;
  };

  template<unsigned int Power>
  inline float Pow(const float value) { return value * Math::Pow<Power - 1>(value); } // Compute value^Power

  template<>
  inline float Pow<0>(const float value) { return 1.f; }                              // Compute value^Power

  inline float  RadToDeg(float r) { return (r * 180) / Math::Pi; }; // Convert radian (* accuracy) to degree
  inline float  DegToRad(float r) { return (r * Math::Pi) / 180; }; // Convert degree to radian (* accuracy)

  inline float  Random() { return (float)std::rand() / (float)RAND_MAX; };  // Return random value [0; 1]
}