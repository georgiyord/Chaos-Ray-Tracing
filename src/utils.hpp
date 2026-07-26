#ifndef __UTILS_CRT
#define __UTILS_CRT

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;
using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8 = int8_t;

template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

template <typename T>
  requires Numeric<T>
class vec3 {
  T x_;
  T y_;
  T z_;

public:
  constexpr vec3<T>(T x, T y, T z) noexcept : x_(x), y_(y), z_(z) {}
  constexpr vec3<T>() noexcept
      : x_(static_cast<T>(0)), y_(static_cast<T>(0)), z_(static_cast<T>(0)) {}

  [[nodiscard]] constexpr T x() const noexcept { return x_; }
  [[nodiscard]] constexpr T y() const noexcept { return y_; }
  [[nodiscard]] constexpr T z() const noexcept { return z_; }

  [[nodiscard]] static constexpr vec3<T> zero() noexcept {
    return vec3<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
  };

  [[nodiscard]] constexpr bool operator==(const vec3<T> &rhs) const noexcept {
    return (x_ == rhs.x_) && (y_ == rhs.y_) && (z_ == rhs.z_);
  }

  [[nodiscard]] constexpr bool operator!=(const vec3<T> &rhs) const noexcept {
    return (x_ != rhs.x_) || (y_ != rhs.y_) || (z_ == rhs.z_);
  }

  [[nodiscard]] constexpr vec3<T> operator-(const vec3<T> &rhs) const noexcept {
    return vec3<T>(x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_);
  }

  [[nodiscard]] constexpr vec3<T> operator+(const vec3<T> &rhs) const noexcept {
    return vec3<T>(x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_);
  }

  template <typename K>
  [[nodiscard]] constexpr friend vec3<T>
  operator*(const K scalar, const vec3<T> &vec) noexcept {
    return vec3<T>(vec.x() * scalar, vec.y() * scalar, vec.z() * scalar);
  }

  template <typename K>
  [[nodiscard]] constexpr friend vec3<T> operator*(const vec3<T> &vec,
                                                   const K scalar) noexcept {
    return vec3<T>(vec.x_ * scalar, vec.y_ * scalar, vec.z_ * scalar);
  }

  [[nodiscard]] constexpr vec3<T> operator/(const vec3<T> &rhs) const {
    return vec3<T>(x_ / rhs.x_, y_ / rhs.y_, z_ / rhs.z_);
  }

  [[nodiscard]] constexpr vec3<T> operator/(size_t rhs) const {
    return vec3<T>(x_ / rhs, y_ / rhs, z_ / rhs);
  }

  [[nodiscard]] constexpr T length() const noexcept {
    return std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
  }

  constexpr vec3<T> &normalise() noexcept {
    double _length = length();
    if (_length != 0) {
      x_ /= _length;
      y_ /= _length;
      z_ /= _length;
    }
    return *this;
  }
};

template <typename T>
  requires Numeric<T>
class vec2 {
  T x_;
  T y_;

public:
  constexpr vec2(T x, T y) noexcept : x_(x), y_(y) {}

  [[nodiscard]] constexpr T x() const noexcept { return x_; }
  [[nodiscard]] constexpr T y() const noexcept { return y_; }

  [[nodiscard]] static constexpr vec2<T> zero() noexcept {
    return vec2<T>(static_cast<T>(0), static_cast<T>(0));
  };

  [[nodiscard]] bool operator==(const vec2 &rhs) const noexcept {
    return x_ == rhs.x_ && y_ == rhs.y_;
  }

  [[nodiscard]] bool operator!=(const vec2 &rhs) const noexcept {
    return x_ != rhs.x_ || y_ != rhs.y_;
  }

  [[nodiscard]] constexpr vec2<T> operator-(const vec2<T> &rhs) const noexcept {
    return vec2<T>(x_ - rhs.x_, y_ - rhs.y_);
  }

  [[nodiscard]] constexpr vec2<T> operator+(const vec2<T> &rhs) const noexcept {
    return vec2<T>(x_ + rhs.x_, y_ + rhs.y_);
  }

  template <typename K>
  [[nodiscard]] constexpr friend vec2<T>
  operator*(const K scalar, const vec2<T> &vec) noexcept {
    return vec2<T>(vec.x_ * scalar, vec.y_ * scalar);
  }

  template <typename K>
  [[nodiscard]] constexpr friend vec2<T> operator*(const vec2<T> &vec,
                                                   const K scalar) noexcept {
    return vec2<T>(vec.x_ * scalar, vec.y_ * scalar);
  }

  [[nodiscard]] constexpr vec2<T> operator/(const vec2<T> &rhs) const {
    return vec2<T>(x_ / rhs.x_, y_ / rhs.y_);
  }

  [[nodiscard]] constexpr vec2<T> operator/(size_t rhs) const {
    return vec2<T>(x_ / rhs, y_ / rhs);
  }

  [[nodiscard]] constexpr T length() const noexcept {
    return std::sqrt(x_ * x_ + y_ * y_);
  }

  [[nodiscard]] constexpr vec3<T> &normalise() noexcept {
    double _length = length();
    if (_length != 0) {
      x_ /= _length;
      y_ /= _length;
    }
    return *this;
  }
};

template <typename T>
  requires Numeric<T>
[[nodiscard]] constexpr vec3<T> crossProduct(const vec3<T> &a,
                                             const vec3<T> &b) noexcept {
  return vec3<T>(a.y() * b.z() - a.z() * b.y(), a.z() * b.x() - a.x() * b.z(),
                 a.x() * b.y() - a.y() * b.x());
}

template <typename T>
  requires Numeric<T>
[[nodiscard]] constexpr T crossProduct(const vec2<T> &a,
                                       const vec2<T> &b) noexcept {
  return a.x() * b.y() - a.y() * b.x();
}

template <typename T>
  requires Numeric<T>
[[nodiscard]] constexpr T dotProduct(const vec3<T> &a,
                                     const vec3<T> &b) noexcept {
  return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
}

template <typename T>
  requires Numeric<T>
[[nodiscard]] constexpr T dotProduct(const vec2<T> &a,
                                     const vec2<T> &b) noexcept {
  return a.x() * b.x() + a.y() * b.y();
}

template <typename T> class Table {
  size_t rows_;
  size_t cols_;
  std::unique_ptr<T[]> arr_;

public:
  constexpr Table(const vec2<size_t> &size)
      : rows_(size.y()), cols_(size.x()), arr_(new T[size.x() * size.y()]) {}
  constexpr Table(const size_t rows, const size_t cols)
      : rows_(rows), cols_(cols), arr_(new T[rows * cols]) {}

  constexpr Table(const Table &rhs)
      : rows_(rhs.rows_), cols_(rhs.cols_), arr_(new T[rows_ * cols_]) {
    for (u32 i = 0; i < rows_ * cols_; ++i) {
      arr_[i] = rhs.arr_[i];
    }
  }

  constexpr Table &operator=(const Table &rhs) {
    if (this != &rhs) {
      if (rows_ != rhs.rows_ || cols_ != rhs.cols_) {
        rows_ = rhs.rows_;
        cols_ = rhs.cols_;
        arr_ = new T[rows_ * cols_];
      }
      for (u32 i = 0; i < rows_ * cols_; ++i) {
        arr_[i] = rhs.arr_[i];
      }
    }
    return *this;
  }

  [[nodiscard]] constexpr const T *begin() const noexcept { return arr_; }

  [[nodiscard]] constexpr const T *end() const noexcept {
    return arr_ + rows_ * cols_;
  }

  [[nodiscard]] constexpr T *begin() noexcept { return arr_; }

  [[nodiscard]] constexpr T *end() noexcept { return arr_ + rows_ * cols_; }

  [[nodiscard]] constexpr T get(const vec2<size_t> &pos) {
    if (pos.x() > cols_ || pos.y() > rows_) {
      throw std::out_of_range("");
    }
    return arr_[(pos.y() * cols_) + pos.x()];
  }

  [[nodiscard]] constexpr T get(const size_t x, const size_t y) {
    if (x > cols_ || y > rows_) {
      throw std::out_of_range("");
    }
    return arr_[(y * cols_) + x];
  }

  [[nodiscard]] constexpr const T &get(const vec2<size_t> &pos) const {
    if (pos.x() > cols_ || pos.y() > rows_) {
      throw std::out_of_range("");
    }
    return arr_[(pos.y() * cols_) + pos.x()];
  }

  [[nodiscard]] constexpr const T &get(u32 x, u32 y) const {
    if (x > cols_ || y > rows_) {
      throw std::out_of_range("");
    }
    return arr_[(y * cols_) + x];
  }

  [[nodiscard]] constexpr size_t getSize() const noexcept {
    return cols_ * rows_;
  }
  [[nodiscard]] constexpr vec2<size_t> getDimensions() const noexcept {
    return vec2(cols_, rows_);
  }
};

using Color = vec3<u8>;

namespace Colors {
constexpr Color Black = Color(0, 0, 0);
constexpr Color Red = Color(255, 0, 0);
constexpr Color Green = Color(0, 255, 0);
constexpr Color Blue = Color(0, 0, 255);
constexpr Color Yellow = Color(255, 255, 0);
constexpr Color Cyan = Color(0, 255, 255);
constexpr Color Purple = Color(255, 0, 255);
constexpr Color White = Color(255, 255, 255);
}; // namespace Colors

class Shape {
  vec2<double> pos_;

protected:
  constexpr Shape(const vec2<double> &pos) noexcept : pos_(pos) {}
  constexpr Shape(double x, double y) noexcept : pos_(x, y) {}

public:
  [[nodiscard]] constexpr vec2<double> position() const noexcept {
    return pos_;
  }
};

class Recht : public Shape {
  vec2<double> size_;

public:
  constexpr Recht(const vec2<double> &size, const vec2<double> &pos) noexcept
      : Shape(pos), size_(size) {}
  constexpr Recht(const vec2<double> &size, double pos_x, double pos_y) noexcept
      : Shape(pos_x, pos_y), size_(size) {}
  constexpr Recht(double size_x, double size_y,
                  const vec2<double> &pos) noexcept
      : Shape(pos), size_(size_x, size_y) {}
  constexpr Recht(double size_x, double size_y, double pos_x,
                  double pos_y) noexcept
      : Shape(pos_x, pos_y), size_(size_x, size_y) {}
};

class Circle : public Shape {
  double radius_;

public:
  constexpr Circle(double radius, const vec2<double> &pos) noexcept
      : Shape(pos), radius_(radius) {}
  constexpr Circle(double radius, double pos_x, double pos_y) noexcept
      : Shape(pos_x, pos_y), radius_(radius) {}
  [[nodiscard]] constexpr size_t radius() const noexcept { return radius_; }
};

// Note: in right-hand rule coordinate system, triangles points are defined
// counter clockwise
class Triangle {
  using Point = vec3<double>;
  Point point1_;
  Point point2_;
  Point point3_;
  mutable Point normal_;
  mutable bool dirty;

public:
  constexpr Triangle(Point point1, Point point2, Point point3) noexcept
      : point1_(point1), point2_(point2), point3_(point3), dirty(true),
        normal_(vec3<double>::zero()) {}
  constexpr Triangle() noexcept
      : point1_(Point::zero()), point2_(Point::zero()), point3_(Point::zero()),
        dirty(true), normal_(vec3<double>::zero()) {}
  [[nodiscard]] constexpr Point normal() const noexcept {
    if (dirty) {
      const Point vec1 = point2_ - point1_;
      const Point vec2 = point3_ - point1_;
      normal_ = crossProduct(vec1, vec2).normalise();
      dirty = false;
    }
    return normal_;
  }

  [[nodiscard]] double getArea() const noexcept {
    const Point vec1 = point2_ - point1_;
    const Point vec2 = point3_ - point1_;
    return crossProduct(vec1, vec2).length() / 2;
  }

  [[nodiscard]] constexpr Point point1() const noexcept { return point1_; }
  [[nodiscard]] constexpr Point point2() const noexcept { return point2_; }
  [[nodiscard]] constexpr Point point3() const noexcept { return point3_; }

  constexpr void update(Point p1, Point p2, Point p3) noexcept {
    point1_ = p1;
    point2_ = p2;
    point3_ = p3;
    dirty = true;
  }
};

class Ray {
  vec3<double> origin_;
  vec3<double> direction_;

public:
  // returns the distance from the ray origin to the intersection point, or NaN
  // if the ray doesn't intersect the triangle.
  constexpr Ray(vec3<double> origin, vec3<double> direction) noexcept
      : origin_(origin), direction_(direction) {}
  constexpr Ray() noexcept
      : origin_(vec3<double>::zero()), direction_(vec3<double>::zero()) {}

  [[nodiscard]] constexpr vec3<double> origin() noexcept { return origin_; }
  [[nodiscard]] constexpr vec3<double> direction() noexcept {
    return direction_;
  }

  [[nodiscard]] constexpr double
  intersects(const Triangle triangle) const noexcept {
    if (dotProduct(direction_, triangle.normal()) >= 0) {
      return std::numeric_limits<double>::quiet_NaN();
    }
    double tStep = dotProduct(triangle.point1() - origin_, triangle.normal()) /
                   dotProduct(direction_, triangle.normal());
    vec3<double> pointPlaneIntersection = origin_ + tStep * direction_;

    if (dotProduct(triangle.normal(),
                   crossProduct(triangle.point2() - triangle.point1(),
                                pointPlaneIntersection - triangle.point1())) <
        0)
      return std::numeric_limits<double>::quiet_NaN();
    if (dotProduct(triangle.normal(),
                   crossProduct(triangle.point3() - triangle.point2(),
                                pointPlaneIntersection - triangle.point2())) <
        0)
      return std::numeric_limits<double>::quiet_NaN();
    if (dotProduct(triangle.normal(),
                   crossProduct(triangle.point1() - triangle.point3(),
                                pointPlaneIntersection - triangle.point3())) <
        0)
      return std::numeric_limits<double>::quiet_NaN();

    return tStep;
  }
};

#endif //__UTILS_CRT
