#pragma once

/**
 * @file Matrix.hpp
 * @brief Templated matrix class for linear algebra operations.
 *
 * Provides a generic Col×Row matrix with compile-time dimensions and element type.
 * Supports standard arithmetic, matrix multiplication, transposition, inversion,
 * and factory methods for common affine transformations (translation, scaling,
 * rotation, reflection, shearing). JSON serialization/deserialization is also
 * available via the Game::JSON interface.
 *
 * @note Template specializations for inverse, reflection, rotation, and shear
 *       are defined in Matrix.cpp for 3×3 and 4×4 float/double matrices.
 */

#include <cmath>
#include <stdexcept>
#include <string>
#include <array>

#include "Math/Math.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace Math
{
  /**
   * @brief Generic column-major matrix of compile-time size.
   * @tparam Col   Number of columns (must be > 0).
   * @tparam Row   Number of rows (defaults to Col, producing a square matrix).
   * @tparam Type  Element type (defaults to Math::DefaultType, i.e. float).
   */
  template<unsigned int Col, unsigned int Row = Col, typename Type = Math::DefaultType>
  class Matrix
  {
    static_assert(Col > 0 && Row > 0, "Invalid matrix size.");

  private:
    using Self = Matrix<Col, Row, Type>;

    std::array<std::array<Type, Col>, Row> _matrix; // Hold matrix values

  public:
    /** @brief Default constructor – zero-initializes every element. */
    Matrix() :
      _matrix()
    {}

    /**
     * @brief Variadic constructor.
     * @details Accepts either exactly Col×Row values (one per element, row-major
     *          order) or a single value that is broadcast to every element.
     * @tparam Types  Parameter pack convertible to Type.
     * @param  args   Element values.
     */
    template<typename ... Types>
    Matrix(Types... args) :
      _matrix()
    {
      Type values[]{ args... };

      // Compilation time error if invalid matrix
      static_assert(sizeof(values) / sizeof(Type) == Col * Row || sizeof(values) / sizeof(Type) == 1, "Invalid matrix parameters.");

      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < Col; col++)
          (*this)(col, row) = values[(sizeof(values) / sizeof(Type) == Col * Row) ? (row * Col + col) : (0)];
    }

    /**
     * @brief Construct from a raw pointer to Col×Row contiguous values (row-major).
     * @param values  Pointer to at least Col×Row elements.
     */
    Matrix(const Type* values) :
      _matrix()
    {
      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < Col; col++)
          (*this)(col, row) = values[row * Col + col];
    }

    /**
     * @brief Construct from a JSON array of arrays.
     * @details Expects a JSON array of @p Row elements, each being a JSON array
     *          of @p Col numbers.
     * @param json  JSON array to deserialize.
     * @throws std::runtime_error if the JSON dimensions do not match Col×Row.
     */
    Matrix(const Game::JSON::Array& json) :
      _matrix()
    {
      // Check JSON array size (number of columns)
      if (json.size() != Row)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

      // Extract values from JSON
      for (auto row = 0; row < Row; row++)
      {
        const auto& array = json.get(row).array();

        // Check JSON array size (number of columns)
        if (array.size() != Col)
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

        for (auto col = 0; col < Col; col++)
          (*this)(col, row) = static_cast<Type>(array.get(col).number());
      }
    }

    /** @brief Copy constructor (defaulted). */
    Matrix(const Self&) = default;

    /** @brief Destructor (defaulted). */
    ~Matrix() = default;

    /** @brief Copy-assignment operator (defaulted). */
    Self& operator=(const Self&) = default;
    /** @brief Move-assignment operator (defaulted). */
    Self& operator=(Self&&) = default;

    /**
     * @brief Element-wise equality comparison (defaulted).
     * @param v  Matrix to compare against.
     * @return true if every element is equal.
     */
    bool  operator==(const Self& v) const = default;
    /**
     * @brief Element-wise inequality comparison (defaulted).
     * @param v  Matrix to compare against.
     * @return true if any element differs.
     */
    bool  operator!=(const Self& v) const = default;

    /**
     * @brief Access element at (@p col, @p row) by reference.
     * @param col  Zero-based column index.
     * @param row  Zero-based row index.
     * @return Mutable reference to the element.
     */
    constexpr auto& operator()(unsigned int col, unsigned int row)
    {
      return _matrix[row][col];
    }

    /**
     * @brief Access element at (@p col, @p row) by value (const).
     * @param col  Zero-based column index.
     * @param row  Zero-based row index.
     * @return Copy of the element.
     */
    constexpr auto  operator()(unsigned int col, unsigned int row) const
    {
      return _matrix[row][col];
    }

    /**
     * @brief In-place element-wise addition.
     * @param v  Matrix to add.
     * @return Reference to *this after addition.
     */
    auto& operator+=(const Self& v)
    {
      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < Col; col++)
          (*this)(col, row) += v(col, row);
      return *this;
    }

    /**
     * @brief Element-wise addition (returns a new matrix).
     * @param v  Matrix to add.
     * @return Resulting matrix.
     */
    auto  operator+(const Self& v) const
    {
      return Self(*this) += v;
    }

    /**
     * @brief In-place element-wise subtraction.
     * @param v  Matrix to subtract.
     * @return Reference to *this after subtraction.
     */
    auto& operator-=(const Self& v)
    {
      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < Col; col++)
          (*this)(col, row) -= v(col, row);
      return *this;
    }

    /**
     * @brief Element-wise subtraction (returns a new matrix).
     * @param v  Matrix to subtract.
     * @return Resulting matrix.
     */
    auto  operator-(const Self& v) const
    {
      return Self(*this) -= v;
    }

    /**
     * @brief In-place scalar multiplication.
     * @param v  Scalar multiplier.
     * @return Reference to *this after scaling.
     */
    auto& operator*=(Type v)
    {
      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < Col; col++)
          (*this)(col, row) *= v;
      return *this;
    }

    /**
     * @brief Scalar multiplication (returns a new matrix).
     * @param v  Scalar multiplier.
     * @return Resulting matrix.
     */
    auto  operator*(Type v) const
    {
      return Self(*this) *= v;
    }

    /**
     * @brief In-place scalar division.
     * @param v  Scalar divisor (must be non-zero in debug builds).
     * @return Reference to *this after division.
     * @throws std::runtime_error (debug only) if @p v is zero.
     */
    auto& operator/=(Type v)
    {
#ifdef _DEBUG
      if (v == Type(0))
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif
      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < Col; col++)
          (*this)(col, row) /= v;
      return *this;
    }

    /**
     * @brief Scalar division (returns a new matrix).
     * @param v  Scalar divisor.
     * @return Resulting matrix.
     */
    auto  operator/(Type v) const
    {
      return Self(*this) /= v;
    }

    /**
     * @brief In-place matrix multiplication.
     * @details Multiplies this (Row×Col) matrix by @p v (Col×OtherCol) and stores
     *          the result back in *this. Only valid when the result dimensions
     *          match the original matrix.
     * @tparam OtherCol  Column count of the right-hand-side matrix.
     * @param  v  Right-hand-side matrix.
     * @return Reference to *this after multiplication.
     */
    template<unsigned int OtherCol>
    auto& operator*=(const Math::Matrix<OtherCol, Col, Type>& v)
    {
      return *this = *this * v;
    }

    /**
     * @brief Matrix multiplication (returns a new matrix).
     * @details Performs standard matrix multiplication of this (Row×Col) matrix
     *          by @p v (Col×OtherCol), yielding a (Row×OtherCol) result.
     * @tparam OtherCol  Column count of the right-hand-side matrix.
     * @param  v  Right-hand-side matrix.
     * @return Product matrix of dimensions Row×OtherCol.
     */
    template<unsigned int OtherCol>
    auto  operator*(const Math::Matrix<OtherCol, Col, Type>& v) const
    {
      Math::Matrix<OtherCol, Row, Type> matrix;

      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < OtherCol; col++)
          for (auto i = 0; i < Col; i++)
            matrix(col, row) += (*this)(i, row) * v(col, i);
      return matrix;
    }

    /**
     * @brief Convert every element to a different numeric type.
     * @tparam NewType  Target element type.
     * @return A new matrix with the same dimensions, elements static_cast to NewType.
     */
    template<typename NewType>
    auto  convert() const
    {
      Math::Matrix<Col, Row, NewType> converted;

      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < Col; col++)
          converted(col, row) = static_cast<NewType>((*this)(col, row));

      return converted;
    }

    /**
     * @brief Serialize the matrix to a JSON array of arrays.
     * @return A Game::JSON::Array of @p Row arrays, each containing @p Col numbers.
     */
    auto  json() const
    {
      Game::JSON::Array json;

      // Pre-allocate columns
      json.reserve(Row);

      // Dump matrix to JSON
      for (auto row = 0; row < Row; row++)
      {
        Game::JSON::Array line;

        // Pre-allocate rows
        json.reserve(Col);

        for (auto col = 0; col < Col; col++)
          line.push(static_cast<double>((*this)(col, row)));

        json.push(std::move(line));
      }

      return json;
    }

    /**
     * @brief Compute the transpose of this matrix.
     * @return A new Row×Col matrix whose element (r,c) equals (*this)(c,r).
     */
    auto  transpose() const
    {
      Math::Matrix<Row, Col, Type>  matrix;

      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < Col; col++)
          matrix(row, col) = (*this)(col, row);
      return matrix;
    }

    /**
     * @brief Generate the identity matrix.
     * @note Only valid for square matrices (Row == Col).
     * @return An identity matrix with ones on the diagonal.
     */
    static auto identite()
    {
      // Compilation time error if invalid matrix
      static_assert(Row == Col, "Invalid matrix identite.");

      Self matrix;

      for (auto i = 0; i < Row; i++)
        matrix(i, i) = Type(1);
      return matrix;
    }

    /**
     * @brief Generate a translation matrix from individual components.
     * @details Requires a square matrix of size ≥ 2. The number of translation
     *          components must equal Col − 1.
     * @tparam Types  Parameter pack convertible to Type.
     * @param  value  First translation component.
     * @param  args   Remaining Col − 2 translation components.
     * @return An identity matrix with translation values in the last column.
     */
    template<typename ... Types>
    static auto translation(Type value, Types... args)
    {
      auto  matrix = identite();
      Type  transformation[]{ value, std::forward<Types>(args)... };

      // Compilation time error if not square matrix
      static_assert(Row == Col && Col > 1, "Invalid translation matrix.");
      static_assert(sizeof(transformation) / sizeof(Type) == Col - 1, "Invalid translation matrix parameters.");

      for (auto i = 0; i < Col - 1; i++)
        matrix(Row - 1, i) = transformation[i];
      return matrix;
    }

    /**
     * @brief Generate a translation matrix from a column vector.
     * @param args  Column vector of Col − 1 translation components.
     * @return An identity matrix with translation values in the last column.
     */
    static auto translation(const Math::Matrix<1, Col - 1, Type>& args)
    {
      auto matrix = identite();

      // Compilation time error if not square matrix
      static_assert(Row == Col && Col > 1, "Invalid translation matrix.");

      for (auto i = 0; i < Col - 1; i++)
        matrix(Row - 1, i) = args(0, i);
      return matrix;
    }

    /**
     * @brief Generate a scaling matrix from individual components.
     * @details Accepts either Col − 1 values (per-axis scaling) or a single
     *          value (uniform scaling). Requires a square matrix of size ≥ 2.
     * @tparam Types  Parameter pack convertible to Type.
     * @param  value  First (or uniform) scale factor.
     * @param  args   Remaining scale factors (0 or Col − 2).
     * @return A diagonal matrix with the given scale factors.
     */
    template<typename ... Types>
    static auto scale(Type value, Types... args)
    {
      auto  matrix = identite();
      Type  transformation[]{ value, std::forward<Types>(args)... };

      static_assert(Row == Col && Col > 1, "Invalid scale matrix.");
      static_assert((sizeof(transformation) / sizeof(Type) == Col - 1) || (sizeof(transformation) / sizeof(Type) == 1), "Invalid scale matrix parameters.");

      for (auto i = 0; i < Col - 1; i++)
        matrix(i, i) = transformation[(sizeof(transformation) / sizeof(Type) == Col - 1) ? i : 0];
      return matrix;
    }

    /**
     * @brief Generate a scaling matrix from a column vector.
     * @param args  Column vector of Col − 1 per-axis scale factors.
     * @return A diagonal matrix with the given scale factors.
     */
    static auto scale(const Math::Matrix<1, Col - 1, Type>& args)
    {
      auto matrix = identite();

      static_assert(Row == Col && Col > 1, "Invalid scale matrix.");

      for (auto i = 0; i < Col - 1; i++)
        matrix(i, i) = args(0, i);
      return matrix;
    }

    // Methods specialized in Matrix.cpp

    /**
     * @brief Compute the inverse of a 4×4 matrix using cofactor expansion.
     * @return The inverse matrix.
     * @throws std::runtime_error (debug only) if the determinant is zero.
     */
    Self        inverse() const;

    /**
     * @brief Generate a 2D reflection (mirror) matrix (3×3).
     * @param x  X component of the reflection axis normal.
     * @param y  Y component of the reflection axis normal.
     * @return 3×3 reflection matrix.
     */
    static Self reflection(Type x, Type y);

    /**
     * @brief Generate a 3D reflection (mirror) matrix (4×4).
     * @param x  X component of the reflection plane normal.
     * @param y  Y component of the reflection plane normal.
     * @param z  Z component of the reflection plane normal.
     * @return 4×4 reflection matrix.
     */
    static Self reflection(Type x, Type y, Type z);

    /**
     * @brief Generate a 2D rotation matrix (3×3).
     * @param a  Rotation angle in radians.
     * @return 3×3 rotation matrix.
     */
    static Self rotation(Type a);

    /**
     * @brief Generate a 3D rotation matrix from Euler angles (4×4, ZYX order).
     * @param x  Rotation around the X axis in radians.
     * @param y  Rotation around the Y axis in radians.
     * @param z  Rotation around the Z axis in radians.
     * @return 4×4 rotation matrix.
     */
    static Self rotation(Type x, Type y, Type z);

    /**
     * @brief Generate a 3D rotation matrix around an arbitrary axis (4×4).
     * @param a  Rotation angle in radians.
     * @param x  X component of the rotation axis (will be normalized).
     * @param y  Y component of the rotation axis (will be normalized).
     * @param z  Z component of the rotation axis (will be normalized).
     * @return 4×4 rotation matrix.
     * @throws std::runtime_error (debug only) if the axis has zero length.
     */
    static Self rotation(Type a, Type x, Type y, Type z);

    /**
     * @brief Generate a 2D shearing matrix (3×3).
     * @param xy  Shear factor of X by Y.
     * @param yx  Shear factor of Y by X.
     * @return 3×3 shearing matrix.
     */
    static Self shear(Type xy, Type yx);

    /**
     * @brief Generate a 3D shearing matrix (4×4).
     * @param xy  Shear factor of X by Y.
     * @param xz  Shear factor of X by Z.
     * @param yx  Shear factor of Y by X.
     * @param yz  Shear factor of Y by Z.
     * @param zx  Shear factor of Z by X.
     * @param zy  Shear factor of Z by Y.
     * @return 4×4 shearing matrix.
     */
    static Self shear(Type xy, Type xz, Type yx, Type yz, Type zx, Type zy);
  };
}