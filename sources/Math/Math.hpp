/**
 * @file Math.hpp
 * @brief Core math utilities including constants, conversions, and random number generation.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>

/**
 * @namespace Math
 * @brief Provides common mathematical constants, operations, and utility functions.
 */
namespace Math
{
  /** @brief Default floating-point type used across Math utilities. */
  using DefaultType = float;

  /**
   * @brief Returns the value of Pi as the specified type.
   * @tparam Type The numeric type to represent Pi.
   * @return Pi cast to @p Type.
   */
  template<typename Type>
  constexpr Type  PiValue()
  {
    return static_cast<Type>(3.14159265358979323846);
  }

  /** @brief Pi constant using the default floating-point type. Use this instead of M_PI. */
  constexpr DefaultType Pi = PiValue<DefaultType>();

  /**
   * @brief Computes the positive modulo of @p i by @p n.
   *
   * For floating-point types, performs a truncated division; for integral types,
   * uses the built-in modulo operator. The result is always in the range [0, n).
   *
   * @tparam Type The numeric type (integral or floating-point).
   * @param i The dividend.
   * @param n The divisor (must be positive).
   * @return The positive remainder of @p i divided by @p n.
   */
  template<typename Type>
  constexpr Type  Modulo(Type i, Type n)
  {
    Type r;
      
    if constexpr (std::is_floating_point<Type>::value)
      r = i - (std::intmax_t)(i / n) * n;
    else
      r = i % n;

    return r < 0 ? r + n : r;
  };
  
  /**
   * @brief Computes @p value raised to the compile-time @p Power.
   *
   * Uses recursive template instantiation to unroll the exponentiation at compile time.
   *
   * @tparam Power The non-negative exponent (evaluated at compile time).
   * @tparam Type  The numeric type of the base value.
   * @param value  The base value.
   * @return @p value raised to the power @p Power.
   */
  template<std::uintmax_t Power, typename Type>
  constexpr Type Pow(const Type value)
  { 
    if constexpr (Power == 0)
      return 1;
    else
      return value * Math::Pow<Power - 1>(value);
  }

  /**
   * @brief Converts an angle from radians to degrees.
   * @tparam Type The numeric type.
   * @param r The angle in radians.
   * @return The angle in degrees.
   */
  template<typename Type>
  constexpr Type  RadToDeg(Type r)
  {
    return (r * 180) / Math::PiValue<Type>();
  }

  /**
   * @brief Converts an angle from degrees to radians.
   * @tparam Type The numeric type.
   * @param r The angle in degrees.
   * @return The angle in radians.
   */
  template<typename Type>
  constexpr Type  DegToRad(Type r)
  {
    return (r * Math::PiValue<Type>()) / 180;
  }

  /**
   * @brief Returns a random value in the range [0, @p range).
   *
   * For floating-point types the default range is [0, 1). For integral types
   * the default range is [0, std::numeric_limits<Type>::max()).
   *
   * @tparam Type  The numeric type (defaults to @ref DefaultType).
   * @param range  The upper bound of the range (exclusive).
   * @return A pseudo-random value of type @p Type.
   */
  template<typename Type = DefaultType>
  inline Type Random(Type range = std::is_floating_point<Type>::value ? 1 : std::numeric_limits<Type>::max())
  {
    if constexpr (std::is_floating_point<Type>::value)
      return static_cast<Type>(std::rand()) / static_cast<Type>(RAND_MAX) * range;
    else
      return static_cast<Type>(std::rand()) % range;
  }

  /**
   * @brief Returns a random value in the range [@p min, @p max].
   * @tparam Type The numeric type.
   * @param min The lower bound of the range (inclusive).
   * @param max The upper bound of the range (inclusive).
   * @return A pseudo-random value between @p min and @p max.
   */
  template<typename Type>
  inline Type  Random(Type min, Type max)
  {
    return min + Random(max - min);
  }
}