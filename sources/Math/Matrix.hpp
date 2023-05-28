#pragma once

#include <cmath>
#include <stdexcept>
#include <string>
#include <array>

namespace Math
{
  template<unsigned int Row, unsigned int Col = Row, typename Type = float>
  class Matrix
  {
  private:
    std::array<std::array<Type, Row>, Col> _matrix; // Hold matrix values

  public:
    Matrix() :
      _matrix{0}
    {
      // Compilation time error if invalid matrix
      static_assert(Col > 0 && Row > 0, "Invalid matrix size.");
    }

    template<typename ... Types>
    Matrix(Types... args)
    {
      Type values[]{ args... };

      // Compilation time error if invalid matrix
      static_assert(Col > 0 && Row > 0, "Invalid matrix size.");
      static_assert(sizeof(values) / sizeof(Type) == Col * Row, "Invalid matrix parameters.");

      for (unsigned int col = 0; col < Col; col++)
        for (unsigned int row = 0; row < Row; row++)
          (*this)(row, col) = values[col * Row + row];
    }

    Matrix(const Matrix<Row, Col, Type>&) = default;

    ~Matrix() = default;

    Matrix<Row, Col, Type>& operator=(const Matrix<Row, Col, Type>&) = default;

    bool  operator==(Math::Matrix<Row, Col, Type> const& v) const // Matrix comparison
    {
      for (unsigned int col = 0; col < Col; col++)
        for (unsigned int row = 0; row < Row; row++)
          if ((*this)(row, col) != v(row, col))
            return false;
      return true;
    }

    bool  operator!=(Math::Matrix<Row, Col, Type> const& v) const // Matrix comparison
    {
      return !(*this == v);
    }

    template<unsigned int R, unsigned int C>
    inline Type&  get() { return _matrix[C][R]; } // Get nth component of vector

    template<unsigned int R, unsigned int C>
    inline Type   get() const { return _matrix[C][R]; } // Get nth component of vector

    Type& operator()(unsigned int row, unsigned int col) // Get matrix value
    {
      return _matrix[col][row];
    }
    
    Type  operator()(unsigned int row, unsigned int col) const // Get matrix value
    {
      return _matrix[col][row];
    }

    Math::Matrix<Row, Col, Type>& operator+=(Math::Matrix<Row, Col, Type> const& v) // Matrix addition
    {
      for (unsigned int col = 0; col < Col; col++)
        for (unsigned int row = 0; row < Row; row++)
          (*this)(row, col) += v(row, col);
      return *this;
    }

    Math::Matrix<Row, Col, Type>  operator+(Math::Matrix<Row, Col, Type> const& v) const // Matrix addition
    {
      return Math::Matrix<Row, Col, Type>(*this) += v;
    }

    Math::Matrix<Row, Col, Type>& operator-=(Math::Matrix<Row, Col, Type> const& v) // Matrix addition
    {
      for (unsigned int col = 0; col < Col; col++)
        for (unsigned int row = 0; row < Row; row++)
          (*this)(row, col) -= v(row, col);
      return *this;
    }

    Math::Matrix<Row, Col, Type>  operator-(Math::Matrix<Row, Col, Type> const& v) const // Matrix addition
    {
      return Math::Matrix<Row, Col, Type>(*this) -= v;
    }

    Math::Matrix<Row, Col, Type>& operator*=(Type v) // Matrix division
    {
      for (unsigned int col = 0; col < Col; col++)
        for (unsigned int row = 0; row < Row; row++)
          (*this)(row, col) *= v;
      return *this;
    }

    Math::Matrix<Row, Col, Type>  operator*(Type v) const // Matrix division
    {
      return Math::Matrix<Row, Col, Type>(*this) *= v;
    }

    Math::Matrix<Row, Col, Type>& operator/=(Type v) // Matrix division
    {
#ifdef _DEBUG
      if (v == 0.)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif
      for (unsigned int col = 0; col < Col; col++)
        for (unsigned int row = 0; row < Row; row++)
          (*this)(row, col) /= v;
      return *this;
    }

    Math::Matrix<Row, Col, Type>  operator/(Type v) const // Matrix division
    {
      return Math::Matrix<Row, Col, Type>(*this) /= v;
    }

    template<unsigned int OtherCol>
    Math::Matrix<Row, OtherCol, Type>&  operator*=(Math::Matrix<Col, OtherCol, Type> const& v) // Matrix multiplication
    {
      return *this = *this * v;
    }

    template<unsigned int OtherCol>
    Math::Matrix<Row, OtherCol, Type> operator*(Math::Matrix<Col, OtherCol, Type> const& v) const // Matrix multiplication
    {
      Math::Matrix<Row, OtherCol, Type> matrix;

      for (unsigned int col = 0; col < OtherCol; col++)
        for (unsigned int row = 0; row < Row; row++)
          for (unsigned int i = 0; i < Col; i++)
            matrix(row, col) += (*this)(row, i) * v(i, col);
      return matrix;
    }

    Math::Matrix<Col, Row, Type>  transpose() const // Generate transpose matrix
    {
      Math::Matrix<Col, Row, Type>  matrix;

      for (unsigned int col = 0; col < Col; col++)
        for (unsigned int row = 0; row < Row; row++)
          matrix(col, row) = (*this)(row, col);
      return matrix;
    }

    static Math::Matrix<Row, Col, Type> identite() // Generate identity matrix
    {
      // Compilation time error if invalid matrix
      static_assert(Col == Row, "Invalid matrix identite.");

      Math::Matrix<Row, Col, Type>  matrix;

      for (unsigned int i = 0; i < Col; i++)
        matrix(i, i) = 1.f;
      return matrix;
    }

    template<typename ... Types>
    static Math::Matrix<Row, Col, Type> translation(Types... args) // Generate translation matrix
    {
      Math::Matrix<Row, Col, Type>  matrix = Math::Matrix<Row, Col, Type>::identite();
      Type                          transformation[]{ args... };

      static_assert(Col == Row && Row > 1, "Invalid translation matrix.");
      static_assert(sizeof(transformation) / sizeof(Type) == Row - 1, "Invalid translation matrix parameters.");

      for (unsigned int i = 0; i < Row - 1; i++)
        matrix(i, Col - 1) = transformation[i];
      return matrix;
    }

    template<typename ... Types>
    static Math::Matrix<Row, Col, Type> scale(Types... args) // Generate scaling matrix
    {
      Math::Matrix<Row, Col, Type>  matrix = Math::Matrix<Row, Col, Type>::identite();
      Type                          transformation[]{ args... };

      static_assert(Col == Row && Row > 1, "Invalid scale matrix.");
      static_assert((sizeof(transformation) / sizeof(Type) == Row - 1) || (sizeof(transformation) / sizeof(Type) == 1), "Invalid scale matrix parameters.");

      if (sizeof(transformation) / sizeof(Type) == Row - 1)
        for (unsigned int i = 0; i < Row - 1; i++)
          matrix(i, i) = transformation[i];
      else
        for (unsigned int i = 0; i < Row - 1; i++)
          matrix(i, i) = transformation[0];
      return matrix;
    }

    // Methods specialized in Matrix.cpp
    Math::Matrix<Row, Col, Type>        inverse() const;                                              // Generate inverse matrix
    static Math::Matrix<Row, Col, Type> reflection(Type x, Type y);                                   // Generate 2D mirror matrix
    static Math::Matrix<Row, Col, Type> reflection(Type x, Type y, Type z);                           // Generate 3D mirror matrix
    static Math::Matrix<Row, Col, Type> rotation(Type a);                                             // Generate 2D rotation matrix
    static Math::Matrix<Row, Col, Type> rotation(Type x, Type y, Type z);                             // Generate 3D rotation matrix
    static Math::Matrix<Row, Col, Type> rotation(Type a, Type x, Type y, Type z);                     // Generate 3D rotation matrix
    static Math::Matrix<Row, Col, Type> shear(Type xy, Type yx);                                      // Generate 2D shearing matrix
    static Math::Matrix<Row, Col, Type> shear(Type xy, Type xz, Type yx, Type yz, Type zx, Type zy);  // Generate 3D shearing matrix
  };
}
