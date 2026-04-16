#pragma once

/**
 * @file Vector.hpp
 * @brief Templated vector class for linear algebra operations.
 *
 * Provides a generic N-dimensional vector built on top of Math::Matrix<1, Size, Type>.
 * Supports component-wise arithmetic, scalar/dot product, cross product, determinant,
 * angle computation, dimensional conversion, type conversion, and JSON
 * serialization/deserialization via the Game::JSON interface.
 *
 * Also defines the free function Math::intersection() for 2D segment intersection
 * and an operator<< overload for stream output.
 *
 * @see Math::Matrix
 */

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
  /**
   * @brief Generic N-dimensional vector with compile-time size.
   *
   * Inherits from Math::Matrix<1, Size, Type> (single-column matrix) and exposes
   * convenient vector-specific operations such as dot product, cross product,
   * length, angle computation, and named component accessors (x, y, z, w).
   *
   * @tparam Size Number of components (dimensions) of the vector.
   * @tparam Type Element type (defaults to Math::DefaultType, i.e. float).
   */
  template<unsigned int Size, typename Type = Math::DefaultType>
  class Vector : public Math::Matrix<1, Size, Type>
  {
  private:
    using Self = Math::Vector<Size, Type>;
    using Base = Math::Matrix<1, Size, Type>;

  public:
    /** @brief Default constructor – zero-initializes every component. */
    Vector() = default;

    /**
     * @brief Variadic constructor for initializing vector components.
     * @tparam Types  Parameter pack of component types (implicitly convertible to Type).
     * @param value   First component value.
     * @param args    Remaining component values.
     */
    template<typename ... Types>
    Vector(Type value, Types... args) :
      Base(value, std::forward<Types>(args)...)
    {
    }

    /**
     * @brief Construct a vector from a raw pointer to component values.
     * @param values Pointer to an array of at least @p Size elements.
     */
    Vector(const Type* values) :
      Base(values)
    {
    }

    /**
     * @brief Construct a vector from a JSON array.
     *
     * Each element of the JSON array is read as a number and cast to @p Type.
     *
     * @param json JSON array whose size must equal @p Size.
     * @throws std::runtime_error If the JSON array size does not match @p Size.
     */
    Vector(const Game::JSON::Array& json) :
      Base()
    {
      // Check JSON array size
      if (json.size() != Size)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

      // Extract values from JSON
      for (auto i = 0; i < Size; i++)
        (*this)(i) = static_cast<Type>(json.get(i).number());
    }

    /** @brief Default copy constructor. */
    Vector(const Self& other) = default;

    /** @brief Default move constructor. */
    Vector(Self&& other) = default;

    /**
     * @brief Construct a vector from a base matrix (copy).
     * @param other Source matrix to copy from.
     */
    Vector(const Base& other) :
      Base(other)
    {
    }

    /**
     * @brief Construct a vector from a base matrix (move).
     * @param other Source matrix to move from.
     */
    Vector(const Base&& other) :
      Base(other)
    {
    }

    /** @brief Default destructor. */
    ~Vector() = default;

    /** @brief Default copy assignment operator. */
    Self& operator=(const Self& other) = default;

    /** @brief Default move assignment operator. */
    Self& operator=(Self&& other) = default;

    /** @brief Equality comparison operator. */
    bool  operator==(const Self& v) const = default;

    /** @brief Inequality comparison operator. */
    bool  operator!=(const Self& v) const = default;

    /**
     * @brief Reinterpret this vector as a lower-dimension vector (mutable).
     *
     * Returns a reference to the first @p wSize components of this vector,
     * reinterpreted as a Math::Vector<wSize, Type>.
     *
     * @warning Uses reinterpret_cast and relies on contiguous memory layout
     *          compatibility between Vector<Size> and Vector<wSize>.
     *
     * @tparam wSize Target dimension (must be <= @p Size).
     * @return Reference to the reinterpreted lower-dimension vector.
     */
    template<unsigned int wSize>
    auto& convert()
    {
      // Check that requested dimension is valid
      static_assert(wSize <= Size, "Invalid vector conversion parameters.");

      return *reinterpret_cast<Math::Vector<wSize, Type>*>(this);
    }

    /**
     * @brief Reinterpret this vector as a lower-dimension vector (const).
     *
     * Returns a const reference to the first @p wSize components of this vector,
     * reinterpreted as a Math::Vector<wSize, Type>.
     *
     * @warning Uses reinterpret_cast and relies on contiguous memory layout
     *          compatibility between Vector<Size> and Vector<wSize>.
     *
     * @tparam wSize Target dimension (must be <= @p Size).
     * @return Const reference to the reinterpreted lower-dimension vector.
     */
    template<unsigned int wSize>
    const auto& convert() const
    {
      // Check that requested dimension is valid
      static_assert(wSize <= Size, "Invalid vector convertion parameters.");

      return *reinterpret_cast<const Math::Vector<wSize, Type>*>(this);
    }

    /**
     * @brief Convert the vector's element type to a different type.
     *
     * Creates a new vector with each component cast to @p NewType.
     *
     * @tparam NewType Target element type.
     * @return A new vector of the same dimension with elements of type @p NewType.
     */
    template<typename NewType>
    auto  convert() const
    {
      Math::Vector<Size, NewType> converted;

      // Convert each value of the vector
      for (auto i = 0; i < Size; i++)
        converted(i) = static_cast<NewType>((*this)(i));

      return converted;
    }

    /**
     * @brief Access the nth component of the vector (mutable).
     * @param c Zero-based component index.
     * @return Reference to the component value.
     */
    constexpr auto& operator()(unsigned int c) { return Base::operator()(0, c); }

    /**
     * @brief Access the nth component of the vector (const).
     * @param c Zero-based component index.
     * @return Copy of the component value.
     */
    constexpr auto  operator()(unsigned int c) const { return Base::operator()(0, c); }

    /** @brief Get a mutable reference to the first component (x). */
    constexpr auto& x() { return (*this)(0); }

    /**
     * @brief Get a mutable reference to the second component (y).
     * @pre Size >= 2.
     */
    constexpr auto& y() { return (*this)(1); }

    /**
     * @brief Get a mutable reference to the third component (z).
     * @pre Size >= 3.
     */
    constexpr auto& z() { return (*this)(2); }

    /**
     * @brief Get a mutable reference to the fourth component (w).
     * @pre Size >= 4.
     */
    constexpr auto& w() { return (*this)(3); }

    /** @brief Get the value of the first component (x). */
    constexpr auto  x() const { return (*this)(0); }

    /**
     * @brief Get the value of the second component (y).
     * @pre Size >= 2.
     */
    constexpr auto  y() const { return (*this)(1); }

    /**
     * @brief Get the value of the third component (z).
     * @pre Size >= 3.
     */
    constexpr auto  z() const { return (*this)(2); }

    /**
     * @brief Get the value of the fourth component (w).
     * @pre Size >= 4.
     */
    constexpr auto  w() const { return (*this)(3); }

    /**
     * @brief Component-wise multiplication assignment with another vector.
     * @param v Vector to multiply with.
     * @return Reference to this vector after multiplication.
     */
    auto& operator*=(const Self& v)
    {
      for (unsigned int i = 0; i < Size; i++)
        (*this)(i) *= v(i);
      return *this;
    }

    /**
     * @brief Component-wise multiplication with another vector.
     * @param v Vector to multiply with.
     * @return New vector containing the component-wise product.
     */
    auto  operator*(const Self& v) const
    {
      return Self(*this) *= v;
    }

    /**
     * @brief Scalar multiplication assignment.
     * @param c Scalar value to multiply each component by.
     * @return Reference to this vector after multiplication.
     */
    auto& operator*=(Type c)
    {
      Base::operator*=(c);
      return *this;
    }

    /**
     * @brief Scalar multiplication.
     * @param c Scalar value to multiply each component by.
     * @return New vector containing the scaled result.
     */
    auto  operator*(Type c) const
    {
      return Self(*this) *= c;
    }

    /**
     * @brief Matrix multiplication assignment.
     * @tparam mSize Number of rows of the right-hand-side matrix.
     * @param v Matrix to multiply with (Size columns × mSize rows).
     * @return Reference to this vector after multiplication.
     */
    template<unsigned int mSize>
    auto& operator*=(const Math::Matrix<Size, mSize, Type>& v)
    {
      Base::operator*=(v);
      return *this;
    }

    /**
     * @brief Matrix multiplication.
     * @tparam mSize Number of rows of the right-hand-side matrix.
     * @param v Matrix to multiply with (Size columns × mSize rows).
     * @return New vector containing the result of the multiplication.
     */
    template<unsigned int mSize>
    auto  operator*(const Math::Matrix<Size, mSize, Type>& v) const
    {
      return Self(*this) *= v;
    }

    /**
     * @brief Component-wise division assignment with another vector.
     * @param v Vector to divide by.
     * @return Reference to this vector after division.
     */
    auto& operator/=(const Self& v)
    {
      for (unsigned int i = 0; i < Size; i++)
        (*this)(i) /= v(i);
      return *this;
    }

    /**
     * @brief Component-wise division with another vector.
     * @param v Vector to divide by.
     * @return New vector containing the component-wise quotient.
     */
    auto  operator/(const Self& v) const
    {
      return Self(*this) /= v;
    }

    /**
     * @brief Scalar division assignment.
     * @param c Scalar value to divide each component by.
     * @return Reference to this vector after division.
     */
    auto& operator/=(Type c)
    {
      Base::operator/=(c);
      return *this;
    }

    /**
     * @brief Scalar division.
     * @param c Scalar value to divide each component by.
     * @return New vector containing the scaled result.
     */
    auto  operator/(Type c) const
    {
      return Self(*this) /= c;
    }

    /**
     * @brief Component-wise addition assignment.
     * @param v Vector to add.
     * @return Reference to this vector after addition.
     */
    auto& operator+=(const Self& v)
    {
      for (unsigned int i = 0; i < Size; i++)
        (*this)(i) += v(i);
      return *this;
    }

    /**
     * @brief Component-wise addition.
     * @param v Vector to add.
     * @return New vector containing the component-wise sum.
     */
    auto  operator+(const Self& v) const
    {
      return Self(*this) += v;
    }

    /**
     * @brief Component-wise subtraction assignment.
     * @param v Vector to subtract.
     * @return Reference to this vector after subtraction.
     */
    auto& operator-=(const Self& v)
    {
      for (unsigned int i = 0; i < Size; i++)
        (*this)(i) -= v(i);
      return *this;
    }

    /**
     * @brief Component-wise subtraction.
     * @param v Vector to subtract.
     * @return New vector containing the component-wise difference.
     */
    auto  operator-(const Self& v) const
    {
      return Self(*this) -= v;
    }

    /**
     * @brief Serialize the vector to a JSON array.
     *
     * Each component is cast to double and pushed into the JSON array.
     *
     * @return A Game::JSON::Array containing all vector components.
     */
    auto  json() const
    {
      Game::JSON::Array json;

      // Pre-allocate array
      json.reserve(Size);

      // Dump vector to JSON
      for (auto n = 0; n < Size; n++)
        json.push(static_cast<double>((*this)(n)));

      return json;
    }

    /**
     * @brief Compute the Euclidean length (magnitude) of the vector.
     * @tparam RetType Return type for the computation (defaults to @p Type).
     * @return The length (L2 norm) of the vector.
     */
    template<typename RetType = Type>
    auto length() const
    {
      auto r = static_cast<RetType>(0.0);

      for (auto i = 0; i < Size; i++)
        r += static_cast<RetType>(Math::Pow<2>((*this)(i)));

      return static_cast<RetType>(std::sqrt(r));
    }

    /**
     * @brief Compute the cosine of the angle between two vectors.
     *
     * Calculates cos(θ) = (A · B) / (|A| × |B|).
     *
     * @tparam RetType Return type for the computation (defaults to @p Type).
     * @param A First vector.
     * @param B Second vector.
     * @return The cosine of the angle between @p A and @p B.
     * @throws std::runtime_error In debug mode, if both vectors have zero length.
     */
    template<typename RetType = Type>
    static auto cos(const Self& A, const Self& B)
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

    /**
     * @brief Compute the angle of a 2D vector relative to the positive X axis.
     *
     * Returns the angle in radians in the range (-π, π] using std::atan2.
     *
     * @tparam RetType Return type for the computation (defaults to @p Type).
     * @param v The 2D vector.
     * @return The angle in radians.
     */
    template<typename RetType = Type>
    static auto angle(const Self& v)
    {
      return static_cast<RetType>(std::atan2(v.y(), v.x()));
    }

    /**
     * @brief Compute the angle (in radians) between two vectors.
     *
     * Calculates θ = acos(cos(A, B)).
     *
     * @tparam RetType Return type for the computation (defaults to @p Type).
     * @param A First vector.
     * @param B Second vector.
     * @return The angle between @p A and @p B in radians.
     */
    template<typename RetType = Type>
    static auto angle(const Self& A, const Self& B)
    {
      return static_cast<RetType>(std::acos(Self::cos<RetType>(A, B)));
    }

    /**
     * @brief Compute the dot (scalar) product of two vectors.
     *
     * Calculates A · B = Σ(Aᵢ × Bᵢ).
     *
     * @tparam RetType Return type for the computation (defaults to @p Type).
     * @param A First vector.
     * @param B Second vector.
     * @return The scalar product of @p A and @p B.
     */
    template<typename RetType = Type>
    static auto scalar(const Self& A, const Self& B)
    {
      auto r = static_cast<RetType>(0.0);

      for (unsigned int i = 0; i < Size; i++)
        r += A(i) * B(i);

      return r;
    }

    /**
     * @brief Compute the determinant of a set of 2D vectors.
     *
     * Only supported for 2-dimensional vectors. Computes:
     * det = v₀.x × v₁.y − v₀.y × v₁.x.
     *
     * @tparam RetType  Return type for the computation (defaults to @p Type).
     * @tparam Vectors  Parameter pack of vector types.
     * @param args      Exactly @p Size vectors (must be 2 for 2D).
     * @return The determinant value.
     */
    template<typename RetType = Type, typename ... Vectors>
    static auto determinant(Vectors... args)
    {
      Self  vec[]{ std::forward<Vectors>(args)... };

      static_assert(sizeof(vec) / sizeof(*vec) == Size, "Invalid vector determinant parameters.");

      // Only support two dimension determinant
      static_assert(Size == 2, "Vector determinant size not supported.");

      return static_cast<RetType>(vec[0].x() * vec[1].y() - vec[0].y() * vec[1].x());
    }

    /**
     * @brief Compute the cross product of two 3D vectors.
     *
     * Only supported for 3-dimensional vectors. Produces a vector
     * perpendicular to both @p A and @p B following the right-hand rule.
     *
     * @param A First 3D vector.
     * @param B Second 3D vector.
     * @return The cross product vector A × B.
     */
    static auto cross(const Self& A, const Self& B)
    {
      // Only support three dimension determinant
      static_assert(Size == 3, "Vector cross size not supported.");

      return Self(A.y() * B.z() - A.z() * B.y(), A.z() * B.x() - A.x() * B.z(), A.x() * B.y() - A.y() * B.x());
    }
  };

  /**
   * @brief Compute intersection parameters of two 2D line segments.
   *
   * Given two segments defined by an origin and a direction vector each,
   * computes the parametric values (t, u) at which the lines intersect.
   * Returns NaN for both parameters if the segments are parallel.
   *
   * @tparam Type Element type of the vectors.
   * @param origin_A    Origin point of the first segment.
   * @param direction_A Direction vector of the first segment.
   * @param origin_B    Origin point of the second segment.
   * @param direction_B Direction vector of the second segment.
   * @return A pair (t, u) of parametric intersection values, or (NaN, NaN) if parallel.
   */
  template<typename Type>
  static auto intersection(const Math::Vector<2, Type>& origin_A, const Math::Vector<2, Type>& direction_A, const Math::Vector<2, Type>& origin_B, const Math::Vector<2, Type>& direction_B)
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

/**
 * @brief Stream insertion operator for Math::Vector.
 *
 * Outputs the vector in the format "[x, y, z, ...]".
 *
 * @tparam Size Number of components in the vector.
 * @tparam Type Element type of the vector.
 * @param stream Output stream to write to.
 * @param vector Vector to output.
 * @return Reference to the output stream.
 */
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