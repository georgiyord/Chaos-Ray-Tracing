#ifndef __UTILS_CRT
#define __UTILS_CRT

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <stdexcept>

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;
using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8 = int8_t;

template <typename T> struct vec3 {
  T x;
  T y;
  T z;

  static constexpr vec3<T> zero() {
    return {
        .x = static_cast<T>(0), .y = static_cast<T>(0), .z = static_cast<T>(0)};
  };

  bool operator==(const vec3<T> &rhs) const {
    return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
  }

  vec3<T> operator-(const vec3<T> &rhs) const {
    return {x - rhs.x, y - rhs.y, z - rhs.z};
  }

  vec3<T> operator+(const vec3<T> &rhs) const {
    return {x + rhs.x, y + rhs.y, z + rhs.z};
  }

  T length() {
    return std::sqrt(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2));
  }

  vec3<T> &normalise() {
    double _length = length();
    if (_length != 0) {
      x /= _length;
      y /= _length;
      z /= _length;
    }
    return *this;
  }

  friend vec3<T> crossProduct(const vec3<T> &a, const vec3<T> &b) {
    return vec3<T>{.x = a.y * b.z - a.z * b.y,
                   .y = a.z * b.x - a.x * b.z,
                   .z = a.x * b.y - a.y * b.x};
  }

  friend T dotProduct(const vec3<T> &a, const vec3<T> &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }

  template <typename K> vec3<T> operator*(K scalar) {
    return vec3<T>{
        .x = this->x * scalar, .y = this->y * scalar, .z = this->z * scalar};
  }

  template <typename K>
  friend vec3<T> operator*(const K scalar, const vec3<T> vec) {
    return vec3<T>{
        .x = vec.x * scalar, .y = vec.y * scalar, .z = vec.z * scalar};
  }

  template <typename K>
  friend vec3<T> operator*(const vec3<T> vec, const K scalar) {
    return vec3<T>{
        .x = vec.x * scalar, .y = vec.y * scalar, .z = vec.z * scalar};
  }
};

template <typename T> struct vec2 {
  T x;
  T y;

  constexpr vec2(T _x, T _y) : x(_x), y(_y) {}

  constexpr vec2<T> operator/(const vec2<T> &rhs) const {
    return vec2<T>(x / rhs.x, y / rhs.y);
  }

  constexpr vec2<T> operator/(size_t rhs) const {
    return vec2<T>(x / rhs, y / rhs);
  }

  bool operator==(const vec2 &rhs) const { return x == rhs.x && y == rhs.y; }

  bool operator!=(const vec2 &rhs) const { return x != rhs.x || y != rhs.y; }
};

template <typename T> class Matrix {
  size_t _rows;
  size_t _cols;
  std::unique_ptr<T[]> _arr;

public:
  Matrix(const vec2<size_t> &size)
      : _rows(size.x), _cols(size.y), _arr(new T[size.x * size.y]) {}
  Matrix(const size_t rows, const size_t cols)
      : _rows(rows), _cols(cols), _arr(new T[rows * cols]) {}

  Matrix(const Matrix &rhs)
      : _rows(rhs._rows), _cols(rhs._cols), _arr(new T[_rows * _cols]) {
    for (u32 i = 0; i < _rows * _cols; ++i) {
      _arr[i] = rhs._arr[i];
    }
  }

  Matrix &operator=(const Matrix &rhs) {
    if (this != &rhs) {
      if (_rows != rhs._rows || _cols != rhs._cols) {
        _rows = rhs._rows;
        _cols = rhs._cols;
        _arr = new T[_rows * _cols];
      }
      for (u32 i = 0; i < _rows * _cols; ++i) {
        _arr[i] = rhs._arr[i];
      }
    }
    return *this;
  }

  [[nodiscard]] const T *begin() const { return _arr; }

  [[nodiscard]] const T *end() const { return _arr + _rows * _cols; }

  [[nodiscard]] T *begin() { return _arr; }

  [[nodiscard]] T *end() { return _arr + _rows * _cols; }

  [[nodiscard]] T get(const vec2<size_t> &pos) {
    if (pos.x > _cols || pos.y > _rows) {
      throw std::out_of_range("");
    }
    return _arr[(pos.y * _cols) + pos.x];
  }

  [[nodiscard]] T get(const size_t x, const size_t y) {
    if (x > _cols || y > _rows) {
      throw std::out_of_range("");
    }
    return _arr[(y * _cols) + x];
  }

  [[nodiscard]] const T &get(const vec2<size_t> &pos) const {
    if (pos.x > _cols || pos.y > _rows) {
      throw std::out_of_range("");
    }
    return _arr[(pos.y * _cols) + pos.x];
  }

  [[nodiscard]] const T &get(u32 x, u32 y) const {
    if (x > _cols || y > _rows) {
      throw std::out_of_range("");
    }
    return _arr[(y * _cols) + x];
  }

  [[nodiscard]] size_t getSize() const { return _cols * _rows; }
  [[nodiscard]] vec2<size_t> getDimensions() const {
    return vec2(_cols, _rows);
  }
};

using Color = vec3<u8>;

namespace Colors {
constexpr Color Black = Color{0, 0, 0};
constexpr Color Red = Color{255, 0, 0};
constexpr Color Green = Color{0, 255, 0};
constexpr Color Blue = Color{0, 0, 255};
constexpr Color Yellow = Color{255, 255, 0};
constexpr Color Cyan = Color{0, 255, 255};
constexpr Color Purple = Color{255, 0, 255};
constexpr Color White = Color{255, 255, 255};
}; // namespace Colors

enum class Anchor : u8 {
  TOP_RIGHT,
  TOP,
  TOP_LEFT,
  LEFT,
  CENTER,
  RIGHT,
  BOTTOM_LEFT,
  BOTTOM,
  BOTTOM_RIGHT
};

class Shape {
  vec2<u64> pos;
  Anchor anchor;

protected:
  Shape(vec2<u64> _pos) : pos(_pos), anchor(Anchor::CENTER) {}
  Shape(vec2<u64> _pos, Anchor _anchor) : pos(_pos), anchor(_anchor) {}

public:
  [[nodiscard]] vec2<u64> getPosition() const { return pos; }
};

class Recht : public Shape {
  vec2<size_t> size;

public:
  Recht(vec2<size_t> _size, vec2<u64> _pos) : Shape(_pos), size(_size) {}
};

class Circle : public Shape {
  size_t radius;

public:
  Circle(size_t _radius, vec2<u64> _pos) : Shape(_pos), radius(_radius) {}
  [[nodiscard]] size_t getRadius() const { return radius; }
};

const Color RED = {255, 0, 0};
const Color GREEN = {0, 255, 0};
const Color BLUE = {0, 0, 255};

// Note: in right-hand rule coordinate system, triangles points are defined
// clockwise
class Triangle {
  using point = vec3<double>;
  point point1;
  point point2;
  point point3;
  mutable point normal;
  mutable bool dirty;

public:
  Triangle(point p1, point p2, point p3)
      : point1(p1), point2(p2), point3(p3), dirty(true) {}
  Triangle()
      : point1(point::zero()), point2(point::zero()), point3(point::zero()),
        dirty(true) {}
  point getNormal() const {
    if (dirty) {
      const point vec1 = point2 - point1;
      const point vec2 = point3 - point1;
      normal = crossProduct(vec1, vec2).normalise();
      dirty = false;
    }
    return normal;
  }

  double getArea() const {
    const point vec1 = point2 - point1;
    const point vec2 = point3 - point1;
    return crossProduct(vec1, vec2).length() / 2;
  }

  point getPoint1() const { return point1; }
  point getPoint2() const { return point2; }
  point getPoint3() const { return point3; }

  void update(point p1, point p2, point p3) {
    point1 = p1;
    point2 = p2;
    point3 = p3;
    dirty = true;
  }
};

struct Ray {
  vec3<double> origin;
  vec3<double> direction;

  // returns the distance from the ray origin to the intersection point, or NaN
  // if the ray doesn't intersect the triangle.
  double intersects(const Triangle triangle) {
    if (dotProduct(direction, triangle.getNormal()) >= 0) {
      return std::numeric_limits<double>::quiet_NaN();
    }
    double tStep =
        dotProduct(triangle.getPoint1() - origin, triangle.getNormal()) /
        dotProduct(direction, triangle.getNormal());
    vec3<double> pointPlaneIntersection = origin + tStep * direction;

    if (dotProduct(
            triangle.getNormal(),
            crossProduct(triangle.getPoint2() - triangle.getPoint1(),
                         pointPlaneIntersection - triangle.getPoint1())) < 0)
      return std::numeric_limits<double>::quiet_NaN();
    if (dotProduct(
            triangle.getNormal(),
            crossProduct(triangle.getPoint3() - triangle.getPoint2(),
                         pointPlaneIntersection - triangle.getPoint2())) < 0)
      return std::numeric_limits<double>::quiet_NaN();
    if (dotProduct(
            triangle.getNormal(),
            crossProduct(triangle.getPoint1() - triangle.getPoint3(),
                         pointPlaneIntersection - triangle.getPoint3())) < 0)
      return std::numeric_limits<double>::quiet_NaN();

    return tStep;
  }
};

#endif //__UTILS_CRT
