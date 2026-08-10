#include <RenderEngine/Matrix3x3.hpp>
#include <iomanip>
#include <stdexcept>

namespace RenderEngine {
 Matrix3x3::Matrix3x3() noexcept : data_{} {}
 Matrix3x3::Matrix3x3(std::array<double, 9> data) noexcept : data_{data} {}

[[nodiscard]] const double &Matrix3x3::operator[](size_t row, size_t col) const {
  if (row >= 3 || col >= 3)
    throw std::out_of_range("Invalid positions");
  return data_[row * 3 + col];
}

[[nodiscard]] Matrix3x3 Matrix3x3::zero() noexcept {
  return {{0, 0, 0, 0, 0, 0, 0, 0, 0}};
}
[[nodiscard]] Matrix3x3 Matrix3x3::one() noexcept {
  return {{1, 0, 0, 0, 1, 0, 0, 0, 1}};
}

[[nodiscard]] Matrix3x3 Matrix3x3::operator*(const Matrix3x3 &rhs) const noexcept {
  return {{
      data_[0] * rhs[0, 0] + data_[1] * rhs[1, 0] + data_[2] * rhs[2, 0],
      data_[0] * rhs[0, 1] + data_[1] * rhs[1, 1] + data_[2] * rhs[2, 1],
      data_[0] * rhs[0, 2] + data_[1] * rhs[1, 2] + data_[2] * rhs[2, 2],
      data_[3] * rhs[0, 0] + data_[4] * rhs[1, 0] + data_[5] * rhs[2, 0],
      data_[3] * rhs[0, 1] + data_[4] * rhs[1, 1] + data_[5] * rhs[2, 1],
      data_[3] * rhs[0, 2] + data_[4] * rhs[1, 2] + data_[5] * rhs[2, 2],
      data_[6] * rhs[0, 0] + data_[7] * rhs[1, 0] + data_[8] * rhs[2, 0],
      data_[6] * rhs[0, 1] + data_[7] * rhs[1, 1] + data_[8] * rhs[2, 1],
      data_[6] * rhs[0, 2] + data_[7] * rhs[1, 2] + data_[8] * rhs[2, 2],
  }};
}

[[nodiscard]] vec3 operator*(const vec3 &vec, const Matrix3x3 &matrix) noexcept {
  return {
      vec.x_ * matrix[0, 0] + vec.y_ * matrix[1, 0] + vec.z_ * matrix[2, 0],
      vec.x_ * matrix[0, 1] + vec.y_ * matrix[1, 1] + vec.z_ * matrix[2, 1],
      vec.x_ * matrix[0, 2] + vec.y_ * matrix[1, 2] + vec.z_ * matrix[2, 2]};
}

 vec3& operator*=(vec3 &vec, const Matrix3x3 &matrix) noexcept {
  double newX = vec.x_ * matrix[0, 0] + vec.y_ * matrix[1, 0] + vec.z_ * matrix[2, 0];
  double newY = vec.x_ * matrix[0, 1] + vec.y_ * matrix[1, 1] + vec.z_ * matrix[2, 1];
  double newZ = vec.x_ * matrix[0, 2] + vec.y_ * matrix[1, 2] + vec.z_ * matrix[2, 2];
  vec.x_ = newX;
  vec.y_ = newY;
  vec.z_ = newZ;
  return vec;
}

std::ostream &operator<<(std::ostream &os, const Matrix3x3 &mat) {
  os << std::fixed << std::setprecision(6) << std::right;
  os << std::setw(4) << mat[0, 0] << ", " << std::setw(4) << mat[0, 1] << ", "
     << std::setw(4) << mat[0, 2] << '\n'
     << std::setw(4) << mat[1, 0] << ", " << std::setw(4) << mat[1, 1] << ", "
     << std::setw(4) << mat[1, 2] << '\n'
     << std::setw(4) << mat[2, 0] << ", " << std::setw(4) << mat[2, 1] << ", "
     << std::setw(4) << mat[2, 2];
  return os;
}
}