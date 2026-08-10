#include <RenderEngine/vec3.hpp>
#include <cmath>
#include <stdexcept>

namespace RenderEngine {
 vec3::vec3(double x, double y, double z) noexcept
    : x_(x), y_(y), z_(z) {}
 vec3::vec3() noexcept : x_(0.), y_(0.), z_(0.) {}

[[nodiscard]] vec3 vec3::zero() noexcept { return vec3(0., 0., 0.); }
[[nodiscard]] vec3 vec3::POS_X() noexcept { return vec3{1., 0., 0.}; }
[[nodiscard]] vec3 vec3::POS_Y() noexcept { return vec3{0., 1., 0.}; }
[[nodiscard]] vec3 vec3::POS_Z() noexcept { return vec3{0., 0., 1.}; }
[[nodiscard]] vec3 vec3::NEG_X() noexcept {
  return vec3{-1., 0., 0.};
}
[[nodiscard]] vec3 vec3::NEG_Y() noexcept {
  return vec3{0., -1., 0.};
}
[[nodiscard]] vec3 vec3::NEG_Z() noexcept {
  return vec3{0., 0., -1.};
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

[[nodiscard]] vec3 operator*(double scalar, const vec3 &vec) noexcept {
  return vec3(vec.x_ * scalar, vec.y_ * scalar, vec.z_ * scalar);
}

[[nodiscard]] vec3 operator*(const vec3 &vec, double scalar) noexcept {
  return vec3(vec.x_ * scalar, vec.y_ * scalar, vec.z_ * scalar);
}

[[nodiscard]] vec3 vec3::operator/(double rhs) const noexcept {
  return vec3(x_ / rhs, y_ / rhs, z_ / rhs);
}

 vec3 &vec3::operator*=(double rhs) noexcept {
  x_ *= rhs;
  y_ *= rhs;
  z_ *= rhs;
  return *this;
}

 vec3 &vec3::operator/=(double rhs) noexcept {
  x_ /= rhs;
  y_ /= rhs;
  z_ /= rhs;
  return *this;
}

[[nodiscard]] double vec3::lengthSquared() const noexcept {
  return x_ * x_ + y_ * y_ + z_ * z_;
}

[[nodiscard]] double vec3::length() const noexcept {
  return std::sqrt(lengthSquared());
}

 vec3 &vec3::normalise() {
  double _length = lengthSquared();
  if (_length == 0)
    throw std::runtime_error("Cannot normalise a zero vector!");
  if (_length == 1)
    return *this;
  _length = sqrt(_length);
  x_ /= _length;
  y_ /= _length;
  z_ /= _length;
  return *this;
}

 void vec3::update(double x, double y, double z) noexcept {
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
[[nodiscard]] double dotProduct(const vec3 &a,
                                          const vec3 &b) noexcept {
  return a.x_ * b.x_ + a.y_ * b.y_ + a.z_ * b.z_;
}

} // namespace RenderEngine