#pragma once

#include <cmath>
#include <stdexcept>
#include <string>
#include <array>

namespace Math
{
  template<unsigned int Row, unsigned int Col = Row>
  class Matrix
  {
  public:

  private:
    std::array<std::array<float, Row>, Col>  _matrix; // Hold matrix values

  public:
    Matrix() :
      _matrix{0}
    {
      // Compilation time error if invalid matrix
      static_assert(Col > 0 && Row > 0, "Invalid matrix size.");
    }

    template<typename ... Floats>
    Matrix(Floats... args)
    {
      float        values[]{ args... };

      // Compilation time error if invalid matrix
      static_assert(Col > 0 && Row > 0, "Invalid matrix size.");
      static_assert(sizeof(values) / sizeof(float) == Col * Row, "Invalid matrix parameters.");

      for (unsigned int col = 0; col < Col; col++)
        for (unsigned int row = 0; row < Row; row++)
          (*this)(row, col) = values[col * Row + row];
    }

    Matrix(const Matrix<Row, Col>&) = default;

    ~Matrix() = default;

    Matrix<Row, Col>& operator=(const Matrix<Row, Col>&) = default;

    bool  operator==(Math::Matrix<Row, Col> const& v) const // Matrix comparison
    {
      for (unsigned int col = 0; col < Col; col++)
        for (unsigned int row = 0; row < Row; row++)
          if ((*this)(row, col) != v(row, col))
            return false;
      return true;
    }

    bool  operator!=(Math::Matrix<Row, Col> const& v) const // Matrix comparison
    {
      return !(*this == v);
    }

    float&  operator()(unsigned int row, unsigned int col) // Get matrix value
    {
      return _matrix[col][row];
    }
    
    float operator()(unsigned int row, unsigned int col) const // Get matrix value
    {
      return _matrix[col][row];
    }

    Math::Matrix<Row, Col>& operator+=(Math::Matrix<Row, Col> const& v) // Matrix addition
    {
      for (unsigned int col = 0; col < Col; col++)
        for (unsigned int row = 0; row < Row; row++)
          (*this)(row, col) += v(row, col);
      return *this;
    }

    Math::Matrix<Row, Col>  operator+(Math::Matrix<Row, Col> const& v) const // Matrix addition
    {
      return Math::Matrix<Row, Col>(*this) += v;
    }

    Math::Matrix<Row, Col>& operator-=(Math::Matrix<Row, Col> const& v) // Matrix addition
    {
      for (unsigned int col = 0; col < Col; col++)
        for (unsigned int row = 0; row < Row; row++)
          (*this)(row, col) -= v(row, col);
      return *this;
    }

    Math::Matrix<Row, Col>  operator-(Math::Matrix<Row, Col> const& v) const // Matrix addition
    {
      return Math::Matrix<Row, Col>(*this) -= v;
    }

    Math::Matrix<Row, Col>& operator*=(float v) // Matrix division
    {
      for (unsigned int col = 0; col < Col; col++)
        for (unsigned int row = 0; row < Row; row++)
          (*this)(row, col) *= v;
      return *this;
    }

    Math::Matrix<Row, Col>  operator*(float v) const // Matrix division
    {
      return Math::Matrix<Row, Col>(*this) *= v;
    }

    Math::Matrix<Row, Col>& operator/=(float v) // Matrix division
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

    Math::Matrix<Row, Col>  operator/(float v) const // Matrix division
    {
      return Math::Matrix<Row, Col>(*this) /= v;
    }

    template<unsigned int OtherCol>
    Math::Matrix<Row, OtherCol>&  operator*=(Math::Matrix<Col, OtherCol> const& v) // Matrix multiplication
    {
      return *this = *this * v;
    }

    template<unsigned int OtherCol>
    Math::Matrix<Row, OtherCol> operator*(Math::Matrix<Col, OtherCol> const& v) const // Matrix multiplication
    {
      Math::Matrix<Row, OtherCol> matrix;

      for (unsigned int col = 0; col < OtherCol; col++)
        for (unsigned int row = 0; row < Row; row++)
          for (unsigned int i = 0; i < Col; i++)
            matrix(row, col) += (*this)(row, i) * v(i, col);
      return matrix;
    }

    Math::Matrix<Col, Row>  transpose() const // Generate transpose matrix
    {
      Math::Matrix<Col, Row>  matrix;

      for (unsigned int col = 0; col < Col; col++)
        for (unsigned int row = 0; row < Row; row++)
          matrix(col, row) = (*this)(row, col);
      return matrix;
    }

    static Math::Matrix<Row, Col> identite() // Generate identity matrix
    {
      // Compilation time error if invalid matrix
      static_assert(Col == Row, "Invalid matrix identite.");

      Math::Matrix<Row, Col>  matrix;

      for (unsigned int i = 0; i < Col; i++)
        matrix(i, i) = 1.f;
      return matrix;
    }

    template<typename ... Doubles>
    static Math::Matrix<Row, Col> translation(Doubles... args) // Generate translation matrix
    {
      Math::Matrix<Row, Col>  matrix = Math::Matrix<Row, Col>::identite();
      float                   transformation[]{ args... };

      static_assert(Col == Row && Row > 1, "Invalid translation matrix.");
      static_assert(sizeof(transformation) / sizeof(float) == Row - 1, "Invalid translation matrix parameters.");

      for (unsigned int i = 0; i < Row - 1; i++)
        matrix(i, Col - 1) = transformation[i];
      return matrix;
    }

    template<typename ... Doubles>
    static Math::Matrix<Row, Col> scale(Doubles... args) // Generate scaling matrix
    {
      Math::Matrix<Row, Col>  matrix = Math::Matrix<Row, Col>::identite();
      float                   transformation[]{ args... };

      static_assert(Col == Row && Row > 1, "Invalid scale matrix.");
      static_assert((sizeof(transformation) / sizeof(float) == Row - 1) || (sizeof(transformation) / sizeof(float) == 1), "Invalid scale matrix parameters.");

      if (sizeof(transformation) / sizeof(float) == Row - 1)
        for (unsigned int i = 0; i < Row - 1; i++)
          matrix(i, i) = transformation[i];
      else
        for (unsigned int i = 0; i < Row - 1; i++)
          matrix(i, i) = transformation[0];
      return matrix;
    }

    // Methods specialized in Matrix.cpp
    Math::Matrix<Row, Col>        inverse() const;                                  // Generate inverse matrix
    static Math::Matrix<Row, Col> reflection(float, float);                         // Generate mirror matrix
    static Math::Matrix<Row, Col> reflection(float, float, float);                  // Generate mirror matrix
    static Math::Matrix<Row, Col> rotation(float);                                  // Generate rotation matrix
    static Math::Matrix<Row, Col> rotation(float, float, float);                    // Generate rotation matrix
    static Math::Matrix<Row, Col> rotation(float, float, float, float);             // Generate rotation matrix
    static Math::Matrix<Row, Col> shear(float, float);                              // Generate shearing matrix
    static Math::Matrix<Row, Col> shear(float, float, float, float, float, float);  // Generate shearing matrix
  };
}
