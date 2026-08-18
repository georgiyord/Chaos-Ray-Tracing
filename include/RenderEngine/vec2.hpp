
namespace RenderEngine {
struct vec2 {
  float x_;
  float y_;
  
 vec2(float x, float y) noexcept;
 vec2() noexcept;

  [[nodiscard]] static vec2 zero() noexcept;

  [[nodiscard]] bool operator==(const vec2 &rhs) const noexcept;
  [[nodiscard]] bool operator!=(const vec2 &rhs) const noexcept;
  [[nodiscard]] vec2 operator-(const vec2 &rhs) const noexcept;
  [[nodiscard]] vec2 operator+(const vec2 &rhs) const noexcept;

 vec2 &operator-=(const vec2 &rhs) noexcept;
 vec2 &operator+=(const vec2 &rhs) noexcept;

  [[nodiscard]] vec2 operator/(float rhs) const noexcept;

 vec2 &operator*=(float rhs) noexcept;
 vec2 &operator/=(float rhs) noexcept;

  [[nodiscard]] float lengthSquared() const noexcept;
  [[nodiscard]] float length() const noexcept;

  [[nodiscard]] vec2 &normalise();

 void update(float x, float y) noexcept;
};
[[nodiscard]] vec2 operator*(float scalar, const vec2 &vec) noexcept;
[[nodiscard]] vec2 operator*(const vec2 &vec, float scalar) noexcept;
} // namespace RenderEngine
