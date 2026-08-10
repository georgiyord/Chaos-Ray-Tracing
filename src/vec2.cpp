#include <RenderEngine/vec2.hpp>
#include <cmath>
#include <stdexcept>

namespace RenderEngine {
 vec2::vec2(double x, double y) noexcept : x_(x), y_(y) {}
 vec2::vec2() noexcept : x_(0.), y_(0.) {}

[[nodiscard]] vec2 vec2::zero() noexcept { return vec2(0., 0.); };

[[nodiscard]] bool vec2::operator==(const vec2 &rhs) const noexcept {
  return x_ == rhs.x_ && y_ == rhs.y_;
}

[[nodiscard]] bool vec2::operator!=(const vec2 &rhs) const noexcept {
  return x_ != rhs.x_ || y_ != rhs.y_;
}

[[nodiscard]] vec2 vec2::operator-(const vec2 &rhs) const noexcept {
  return vec2(x_ - rhs.x_, y_ - rhs.y_);
}

[[nodiscard]] vec2 vec2::operator+(const vec2 &rhs) const noexcept {
  return vec2(x_ + rhs.x_, y_ + rhs.y_);
}

 vec2 &vec2::operator-=(const vec2 &rhs) noexcept {
  x_ -= rhs.x_;
  y_ -= rhs.y_;
  return *this;
}

 vec2 &vec2::operator+=(const vec2 &rhs) noexcept {
  x_ += rhs.x_;
  y_ += rhs.y_;
  return *this;
}

[[nodiscard]] vec2 operator*(double scalar, const vec2 &vec) noexcept {
  return vec2(vec.x_ * scalar, vec.y_ * scalar);
}

[[nodiscard]] vec2 operator*(const vec2 &vec, double scalar) noexcept {
  return vec2(vec.x_ * scalar, vec.y_ * scalar);
}

[[nodiscard]] vec2 vec2::operator/(double rhs) const noexcept {
  return vec2(x_ / rhs, y_ / rhs);
}

 vec2 &vec2::operator*=(double rhs) noexcept {
  x_ *= rhs;
  y_ *= rhs;
  return *this;
}

 vec2 &vec2::operator/=(double rhs) noexcept {
  x_ /= rhs;
  y_ /= rhs;
  return *this;
}

[[nodiscard]] double vec2::lengthSquared() const noexcept {
  return x_ * x_ + y_ * y_;
}

[[nodiscard]] double vec2::length() const noexcept {
  return std::sqrt(lengthSquared());
}

[[nodiscard]] vec2 &vec2::normalise() {
  double length = lengthSquared();
  if (length == 0) {
    throw std::runtime_error("Cannot normalise zero vector");
  }
  if (length == 1) {
    return *this;
  }
  length = std::sqrt(length);
  x_ /= length;
  y_ /= length;
  return *this;
}

[[nodiscard]] double crossProduct(const vec2 &a,
                                            const vec2 &b) noexcept {
  return a.x_ * b.y_ - a.y_ * b.x_;
}

[[nodiscard]] double dotProduct(const vec2 &a,
                                          const vec2 &b) noexcept {
  return a.x_ * b.x_ + a.y_ * b.y_;
}
} // namespace RenderEngine