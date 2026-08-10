#ifndef RENDERENGINE_VEC3_HPP
#define RENDERENGINE_VEC3_HPP

namespace RenderEngine {
struct vec3 {
  double x_;
  double y_;
  double z_;
  
 vec3(double x, double y, double z) noexcept;
 vec3() noexcept;

  [[nodiscard]] static vec3 zero() noexcept;
  [[nodiscard]] static vec3 POS_X() noexcept;
  [[nodiscard]] static vec3 POS_Y() noexcept;
  [[nodiscard]] static vec3 POS_Z() noexcept;
  [[nodiscard]] static vec3 NEG_X() noexcept;
  [[nodiscard]] static vec3 NEG_Y() noexcept;
  [[nodiscard]] static vec3 NEG_Z() noexcept;

  [[nodiscard]] bool operator==(const vec3 &rhs) const noexcept;
  [[nodiscard]] bool operator!=(const vec3 &rhs) const noexcept;
  [[nodiscard]] vec3 operator-(const vec3 &rhs) const noexcept;
  [[nodiscard]] vec3 operator+(const vec3 &rhs) const noexcept;

 vec3 &operator-=(const vec3 &rhs) noexcept;
 vec3 &operator+=(const vec3 &rhs) noexcept;

  [[nodiscard]] vec3 operator/(double rhs) const noexcept;

 vec3 &operator*=(double rhs) noexcept;
 vec3 &operator/=(double rhs) noexcept;

  [[nodiscard]] double lengthSquared() const noexcept;
  [[nodiscard]] double length() const noexcept;

 vec3 &normalise();

 void update(double x, double y, double z) noexcept;
};

[[nodiscard]] vec3 crossProduct(const vec3 &a,
                                          const vec3 &b) noexcept;
[[nodiscard]] double dotProduct(const vec3 &a,
                                          const vec3 &b) noexcept;

[[nodiscard]] vec3 operator*(double scalar, const vec3 &vec) noexcept;
[[nodiscard]] vec3 operator*(const vec3 &vec, double scalar) noexcept;

} // namespace RenderEngine

#endif // RENDERENGINE_VEC3_HPP