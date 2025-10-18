#pragma once

#define _USE_MATH_DEFINES

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>

#ifndef M_PI
# define M_PI 3.14159265358979323846
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
  using DefaultType = float;  // Default type in math

  template<typename Type = DefaultType>
  constexpr Type  PiValue()
  {
    return static_cast<Type>(M_PI);
  }

  constexpr DefaultType Pi = PiValue<DefaultType>(); // Use this instead of M_PI

  template<typename Type>
  constexpr Type  Modulo(Type i, Type n) // Return the positive modulo n of i
  {
    Type r;
      
    if constexpr (std::is_floating_point<Type>::value)
      r = i - (std::intmax_t)(i / n) * n;
    else
      r = i % n;

    return r < 0 ? r + n : r;
  };
  
  template<std::uintmax_t Power>
  constexpr DefaultType Pow(const DefaultType value) // Compute value^power
  { 
    if constexpr (Power == 0)
      return 1;
    else
      return value * Math::Pow<Power - 1>(value);
  }

  template<typename Type = DefaultType>
  constexpr Type  RadToDeg(Type r)  // Convert radian (* accuracy) to degree
  {
    return (r * 180) / Math::PiValue<Type>();
  }

  template<typename Type = DefaultType>
  constexpr Type  DegToRad(Type r)  // Convert degree to radian (* accuracy)
  {
    return (r * Math::PiValue<Type>()) / 180;
  }

  template<typename Type = DefaultType>
  inline Type Random(Type range = std::is_floating_point<Type>::value ? 1 : std::numeric_limits<Type>::max()) // Return random value [0; 1]
  {
    if constexpr (std::is_floating_point<Type>::value)
      return static_cast<Type>(std::rand()) / static_cast<Type>(RAND_MAX) * range;
    else
      return static_cast<Type>(std::rand()) % range;
  }

  template<typename Type = DefaultType>
  inline DefaultType  Random(Type min, Type max)  // Return random value [min; max]
  {
    return min + Random(max - min);
  }
}