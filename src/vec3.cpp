#include "vec3.hpp"

#include <cmath>
#include <stdexcept>

namespace RenderEngine {
 vec3::vec3(float x, float y, float z) noexcept
    : x_(x), y_(y), z_(z) {}
 vec3::vec3() noexcept : x_(0.f), y_(0.f), z_(0.f) {}

[[nodiscard]] vec3 vec3::zero() noexcept { return vec3(0.f, 0.f, 0.f); }
[[nodiscard]] vec3 vec3::POS_X() noexcept { return vec3{1.f, 0.f, 0.f}; }
[[nodiscard]] vec3 vec3::POS_Y() noexcept { return vec3{0.f, 1.f, 0.f}; }
[[nodiscard]] vec3 vec3::POS_Z() noexcept { return vec3{0.f, 0.f, 1.f}; }
[[nodiscard]] vec3 vec3::NEG_X() noexcept {
  return vec3{-1.f, 0.f, 0.f};
}
[[nodiscard]] vec3 vec3::NEG_Y() noexcept {
  return vec3{0.f, -1.f, 0.f};
}
[[nodiscard]] vec3 vec3::NEG_Z() noexcept {
  return vec3{0.f, 0.f, -1.f};
}

[[nodiscard]] bool vec3::operator==(const vec3 &rhs) const noexcept {
  return (x_ == rhs.x_) && (y_ == rhs.y_) && (z_ == rhs.z_);
}

[[nodiscard]] bool vec3::operator!=(const vec3 &rhs) const noexcept {
  return (x_ != rhs.x_) || (y_ != rhs.y_) || (z_ != rhs.z_);
}

[[nodiscard]] vec3 vec3::operator-(const vec3 &rhs) const noexcept {
  return vec3(x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_);
}

[[nodiscard]] vec3 vec3::operator+(const vec3 &rhs) const noexcept {
  return vec3(x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_);
}

 vec3 &vec3::operator-=(const vec3 &rhs) noexcept {
  x_ -= rhs.x_;
  y_ -= rhs.y_;
  z_ -= rhs.z_;
  return *this;
}

 vec3 &vec3::operator+=(const vec3 &rhs) noexcept {
  x_ += rhs.x_;
  y_ += rhs.y_;
  z_ += rhs.z_;
  return *this;
}

[[nodiscard]] vec3 operator*(float scalar, const vec3 &vec) noexcept {
  return vec3(vec.x_ * scalar, vec.y_ * scalar, vec.z_ * scalar);
}

[[nodiscard]] vec3 operator*(const vec3 &vec, float scalar) noexcept {
  return vec3(vec.x_ * scalar, vec.y_ * scalar, vec.z_ * scalar);
}

[[nodiscard]] vec3 vec3::operator/(float rhs) const noexcept {
  return vec3(x_ / rhs, y_ / rhs, z_ / rhs);
}

 vec3 &vec3::operator*=(float rhs) noexcept {
  x_ *= rhs;
  y_ *= rhs;
  z_ *= rhs;
  return *this;
}

 vec3 &vec3::operator/=(float rhs) noexcept {
  x_ /= rhs;
  y_ /= rhs;
  z_ /= rhs;
  return *this;
}

[[nodiscard]] float vec3::lengthSquared() const noexcept {
  return x_ * x_ + y_ * y_ + z_ * z_;
}

[[nodiscard]] float vec3::length() const noexcept {
  return std::sqrt(lengthSquared());
}

 vec3 &vec3::normalise() {
  float _length = lengthSquared();
  if (_length == 0)
    throw std::runtime_error("Cannot normalise a zero vector!");
  if (_length == 1)
    return *this;
  _length = std::sqrt(_length);
  x_ /= _length;
  y_ /= _length;
  z_ /= _length;
  return *this;
}

 void vec3::update(float x, float y, float z) noexcept {
  x_ = x;
  y_ = y;
  z_ = z;
}

// std::ostream &operator<<(std::ostream &os, const vec3 &vec) {
//   os << static_cast<int>(vec.x_()) << " " << static_cast<int>(vec.y_()) << " "
//      << static_cast<int>(vec.z_());
//   return os;
// }

[[nodiscard]] vec3 crossProduct(const vec3 &a,
                                          const vec3 &b) noexcept {
  return vec3(a.y_ * b.z_ - a.z_ * b.y_, a.z_ * b.x_ - a.x_ * b.z_,
              a.x_ * b.y_ - a.y_ * b.x_);
}
[[nodiscard]] float dotProduct(const vec3 &a,
                                          const vec3 &b) noexcept {
  return a.x_ * b.x_ + a.y_ * b.y_ + a.z_ * b.z_;
}

} // namespace RenderEngine