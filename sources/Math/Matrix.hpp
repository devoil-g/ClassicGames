#pragma once

#include <cmath>
#include <stdexcept>
#include <string>
#include <array>

#include "Math/Math.hpp"
#include "System/JavaScriptObjectNotation.hpp"

namespace Math
{
  template<unsigned int Col, unsigned int Row = Col, typename Type = Math::DefaultType>
  class Matrix
  {
  private:
    using Self = Matrix<Col, Row, Type>;

    std::array<std::array<Type, Col>, Row> _matrix; // Hold matrix values

  public:
    Matrix() :
      _matrix{0}
    {
      // Compilation time error if invalid matrix
      static_assert(Col > 0 && Row > 0, "Invalid matrix size.");
    }

    template<typename ... Types>
    Matrix(Types... args) :
      _matrix()
    {
      Type values[]{ args... };

      // Compilation time error if invalid matrix
      static_assert(Col > 0 && Row > 0, "Invalid matrix size.");
      static_assert(sizeof(values) / sizeof(Type) == Col * Row || sizeof(values) / sizeof(Type) == 1, "Invalid matrix parameters.");

      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < Col; col++)
          (*this)(col, row) = values[(sizeof(values) / sizeof(Type) == Col * Row) ? (row * Col + col) : (0)];
    }

    Matrix(const Type* values) :
      _matrix()
    {
      // Compilation time error if invalid matrix
      static_assert(Col > 0 && Row > 0, "Invalid matrix size.");

      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < Col; col++)
          (*this)(col, row) = values[row * Col + col];
    }

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
          (*this)(row, col) = (Type)array.get(col).number();
      }
    }

    Matrix(const Self&) = default;

    ~Matrix() = default;

    Self& operator=(const Self&) = default;
    Self& operator=(Self&&) = default;

    Self& operator=(const Game::JSON::Array& json)
    {
      // Check JSON array size (number of columns)
      if (json.size() != Col)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

      // Extract values from JSON
      for (auto row = 0; row < Row; row++)
      {
        const auto& array = json.get(row).array();

        // Check JSON array size (number of columns)
        if (array.size() != Row)
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

        for (auto col = 0; col < Col; col++)
          (*this)(row, col) = (Type)array.get(col).number();
      }

      return *this;
    }

    auto  operator==(const Self& v) const // Matrix comparison
    {
      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < Col; col++)
          if ((*this)(col, row) != v(col, row))
            return false;
      return true;
    }

    auto  operator!=(const Self& v) const // Matrix comparison
    {
      return !(*this == v);
    }

    constexpr Type& operator()(unsigned int col, unsigned int row) // Get matrix value
    {
      return _matrix[row][col];
    }
    
    constexpr Type  operator()(unsigned int col, unsigned int row) const // Get matrix value
    {
      return _matrix[row][col];
    }

    auto& operator+=(const Self& v) // Matrix addition
    {
      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < Col; col++)
          (*this)(col, row) += v(col, row);
      return *this;
    }

    auto  operator+(const Self& v) const // Matrix addition
    {
      return Self(*this) += v;
    }

    auto& operator-=(const Self& v) // Matrix addition
    {
      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < Col; col++)
          (*this)(col, row) -= v(col, row);
      return *this;
    }

    auto  operator-(const Self& v) const // Matrix addition
    {
      return Self(*this) -= v;
    }

    auto& operator*=(Type v) // Matrix division
    {
      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < Col; col++)
          (*this)(col, row) *= v;
      return *this;
    }

    auto  operator*(Type v) const // Matrix division
    {
      return Self(*this) *= v;
    }

    auto& operator/=(Type v) // Matrix division
    {
#ifdef _DEBUG
      if (v == 0.)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif
      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < Col; col++)
          (*this)(col, row) /= v;
      return *this;
    }

    auto  operator/(Type v) const // Matrix division
    {
      return Self(*this) /= v;
    }

    template<unsigned int OtherCol>
    auto& operator*=(const Math::Matrix<OtherCol, Col, Type>& v) // Matrix multiplication
    {
      return *this = *this * v;
    }

    template<unsigned int OtherCol>
    auto  operator*(const Math::Matrix<OtherCol, Col, Type>& v) const // Matrix multiplication
    {
      Math::Matrix<OtherCol, Row, Type> matrix;

      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < OtherCol; col++)
          for (auto i = 0; i < Col; i++)
            matrix(col, row) += (*this)(i, row) * v(col, i);
      return matrix;
    }
    
    Game::JSON::Array json() const
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
          line.push((double)(*this)(col, row));

        json.push(std::move(line));
      }

      return json;
    }

    auto  transpose() const // Generate transpose matrix
    {
      Math::Matrix<Row, Col, Type>  matrix;

      for (auto row = 0; row < Row; row++)
        for (auto col = 0; col < Col; col++)
          matrix(row, col) = (*this)(col, row);
      return matrix;
    }

    static auto identite() // Generate identity matrix
    {
      // Compilation time error if invalid matrix
      static_assert(Row == Col, "Invalid matrix identite.");

      Self matrix;

      for (auto i = 0; i < Row; i++)
        matrix(i, i) = 1.f;
      return matrix;
    }

    template<typename ... Types>
    static auto translation(Types... args) // Generate translation matrix
    {
      auto  matrix = identite();
      Type  transformation[]{ std::forward<Types>(args)... };

      // Compilation time error if not square matrix
      static_assert(Row == Col && Col > 1, "Invalid translation matrix.");
      static_assert(sizeof(transformation) / sizeof(Type) == Col - 1, "Invalid translation matrix parameters.");

      for (auto i = 0; i < Col - 1; i++)
        matrix(Row - 1, i) = transformation[i];
      return matrix;
    }

    template<typename ... Types>
    static auto translation(const Math::Matrix<1, Col - 1, Type>& args) // Generate translation matrix
    {
      auto matrix = identite();
      
      // Compilation time error if not square matrix
      static_assert(Row == Col && Col > 1, "Invalid translation matrix.");

      for (auto i = 0; i < Col - 1; i++)
        matrix(Row - 1, i) = args(0, i);
      return matrix;
    }

    template<typename ... Types>
    static auto scale(Types... args) // Generate scaling matrix
    {
      auto  matrix = identite();
      Type  transformation[]{ std::forward<Types>(args)... };

      static_assert(Row == Col && Col > 1, "Invalid scale matrix.");
      static_assert((sizeof(transformation) / sizeof(Type) == Col - 1) || (sizeof(transformation) / sizeof(Type) == 1), "Invalid scale matrix parameters.");

      for (auto i = 0; i < Col - 1; i++)
        matrix(i, i) = transformation[(sizeof(transformation) / sizeof(Type) == Col - 1) ? i : 0];
      return matrix;
    }

    template<typename ... Types>
    static auto scale(const Math::Matrix<1, Col - 1, Type>& args) // Generate translation matrix
    {
      auto matrix = identite();

      static_assert(Row == Col && Col > 1, "Invalid translation matrix.");

      for (auto i = 0; i < Col - 1; i++)
        matrix(i, i) = args(0, i);
      return matrix;
    }

    // Methods specialized in Matrix.cpp
    Self        inverse() const;                                              // Generate inverse matrix
    static Self reflection(Type x, Type y);                                   // Generate 2D mirror matrix
    static Self reflection(Type x, Type y, Type z);                           // Generate 3D mirror matrix
    static Self rotation(Type a);                                             // Generate 2D rotation matrix
    static Self rotation(Type x, Type y, Type z);                             // Generate 3D rotation matrix
    static Self rotation(Type a, Type x, Type y, Type z);                     // Generate 3D rotation matrix
    static Self shear(Type xy, Type yx);                                      // Generate 2D shearing matrix
    static Self shear(Type xy, Type xz, Type yx, Type yz, Type zx, Type zy);  // Generate 3D shearing matrix
  };
}
