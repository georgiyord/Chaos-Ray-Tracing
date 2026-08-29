#ifndef RENDERENGINE_MATRIX3X3_HPP
#define RENDERENGINE_MATRIX3X3_HPP

#include "vec3.hpp"
#include <array>
#include <cstddef>

namespace RenderEngine {
class Matrix3x3 {
  using Matrix = Matrix3x3;
  std::array<float, 9> data_;

public:
  Matrix3x3() noexcept;
  Matrix3x3(std::array<float, 9> data) noexcept;

  [[nodiscard]] const float &operator[](size_t row, size_t col) const;

  [[nodiscard]] static Matrix3x3 zero() noexcept;
  [[nodiscard]] static Matrix3x3 one() noexcept;

  [[nodiscard]] Matrix operator*(const Matrix &rhs) const noexcept;
};

[[nodiscard]] vec3 operator*(const vec3 &vec, const Matrix3x3 &matrix) noexcept;
vec3 &operator*=(vec3 &vec, const Matrix3x3 &matrix) noexcept;
}

#endif // RENDERENGINE_MATRIX3X3_HPP