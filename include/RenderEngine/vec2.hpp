
namespace RenderEngine {
struct vec2 {
  double x_;
  double y_;
  
 vec2(double x, double y) noexcept;
 vec2() noexcept;

  [[nodiscard]] static vec2 zero() noexcept;

  [[nodiscard]] bool operator==(const vec2 &rhs) const noexcept;
  [[nodiscard]] bool operator!=(const vec2 &rhs) const noexcept;
  [[nodiscard]] vec2 operator-(const vec2 &rhs) const noexcept;
  [[nodiscard]] vec2 operator+(const vec2 &rhs) const noexcept;

 vec2 &operator-=(const vec2 &rhs) noexcept;
 vec2 &operator+=(const vec2 &rhs) noexcept;

  [[nodiscard]] vec2 operator/(double rhs) const noexcept;

 vec2 &operator*=(double rhs) noexcept;
 vec2 &operator/=(double rhs) noexcept;

  [[nodiscard]] double lengthSquared() const noexcept;
  [[nodiscard]] double length() const noexcept;

  [[nodiscard]] vec2 &normalise();

 void update(double x, double y) noexcept;
};
[[nodiscard]] vec2 operator*(double scalar, const vec2 &vec) noexcept;
[[nodiscard]] vec2 operator*(const vec2 &vec, double scalar) noexcept;
} // namespace RenderEngine
