#ifndef RENDERENGINE_VEC3_HPP
#define RENDERENGINE_VEC3_HPP

namespace RenderEngine {
struct vec3 {
  float x_;
  float y_;
  float z_;

  vec3(float x, float y, float z) noexcept;
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

  [[nodiscard]] vec3 operator/(float rhs) const noexcept;

  vec3 &operator*=(float rhs) noexcept;
  vec3 &operator/=(float rhs) noexcept;

  [[nodiscard]] float lengthSquared() const noexcept;
  [[nodiscard]] float length() const noexcept;

  vec3 &normalise();

  void update(float x, float y, float z) noexcept;
};

[[nodiscard]] vec3 crossProduct(const vec3 &a, const vec3 &b) noexcept;
[[nodiscard]] float dotProduct(const vec3 &a, const vec3 &b) noexcept;

[[nodiscard]] vec3 operator*(float scalar, const vec3 &vec) noexcept;
[[nodiscard]] vec3 operator*(const vec3 &vec, float scalar) noexcept;

} // namespace RenderEngine

#endif // RENDERENGINE_VEC3_HPP