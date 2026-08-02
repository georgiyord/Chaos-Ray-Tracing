#ifndef __UTILS_CRT
#define __UTILS_CRT

#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <memory>
#include <ostream>
#include <random>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

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
  constexpr vec3(T x, T y, T z) noexcept : x_(x), y_(y), z_(z) {}
  constexpr vec3() noexcept
      : x_(static_cast<T>(0)), y_(static_cast<T>(0)), z_(static_cast<T>(0)) {}

  [[nodiscard]] constexpr T x() const noexcept { return x_; }
  [[nodiscard]] constexpr T y() const noexcept { return y_; }
  [[nodiscard]] constexpr T z() const noexcept { return z_; }

  [[nodiscard]] static constexpr vec3<T> zero() noexcept {
    return vec3<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
  };

  [[nodiscard]] static constexpr vec3<T> POS_X() noexcept {
    return vec3<T>{static_cast<T>(1), static_cast<T>(0), static_cast<T>(0)};
  }
  [[nodiscard]] static constexpr vec3<T> POS_Y() noexcept {
    return vec3<T>{static_cast<T>(0), static_cast<T>(1), static_cast<T>(0)};
  }
  [[nodiscard]] static constexpr vec3<T> POS_Z() noexcept {
    return vec3<T>{static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)};
  }
  [[nodiscard]] static constexpr vec3<T> NEG_X() noexcept {
    return vec3<T>{static_cast<T>(-1), static_cast<T>(0), static_cast<T>(0)};
  }
  [[nodiscard]] static constexpr vec3<T> NEG_Y() noexcept {
    return vec3<T>{static_cast<T>(0), static_cast<T>(-1), static_cast<T>(0)};
  }
  [[nodiscard]] static constexpr vec3<T> NEG_Z() noexcept {
    return vec3<T>{static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1)};
  }

  [[nodiscard]] constexpr bool operator==(const vec3<T> &rhs) const noexcept {
    return (x_ == rhs.x_) && (y_ == rhs.y_) && (z_ == rhs.z_);
  }

  [[nodiscard]] constexpr bool operator!=(const vec3<T> &rhs) const noexcept {
    return (x_ != rhs.x_) || (y_ != rhs.y_) || (z_ != rhs.z_);
  }

  [[nodiscard]] constexpr vec3<T> operator-(const vec3<T> &rhs) const noexcept {
    return vec3<T>(x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_);
  }

  constexpr vec3<T> &operator-=(const vec3<T> &rhs) noexcept {
    x_ -= rhs.x_;
    y_ -= rhs.y_;
    z_ -= rhs.z_;
    return *this;
  }

  [[nodiscard]] constexpr vec3<T> operator+(const vec3<T> &rhs) const noexcept {
    return vec3<T>(x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_);
  }

  constexpr vec3<T> &operator+=(const vec3<T> &rhs) noexcept {
    x_ += rhs.x_;
    y_ += rhs.y_;
    z_ += rhs.z_;
    return *this;
  }

  template <typename K>
  [[nodiscard]] constexpr friend vec3<T>
  operator*(const K scalar, const vec3<T> &vec) noexcept {
    return vec3<T>(vec.x() * scalar, vec.y() * scalar, vec.z() * scalar);
  }

  template <typename K>
  [[nodiscard]] constexpr friend vec3<T> operator*(const vec3<T> &vec,
                                                   const K scalar) noexcept {
    return vec3<T>(static_cast<T>(vec.x_ * scalar),
                   static_cast<T>(vec.y_ * scalar),
                   static_cast<T>(vec.z_ * scalar));
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

  [[nodiscard]] constexpr T lengthSquared() const noexcept {
    return x_ * x_ + y_ * y_ + z_ * z_;
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

  constexpr void updateX(T value) noexcept { x_ = value; }
  constexpr void updateY(T value) noexcept { y_ = value; }
  constexpr void updateZ(T value) noexcept { z_ = value; }
  constexpr void update(T x, T y, T z) noexcept {
    updateX(x);
    updateY(y);
    updateZ(z);
  }
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const vec3<T> &vec) {
  os << static_cast<int>(vec.x()) << " " << static_cast<int>(vec.y()) << " "
     << static_cast<int>(vec.z());
  return os;
}

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

  [[nodiscard]] constexpr vec2<T> &normalise() noexcept {
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
protected:
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
        arr_ = std::make_unique<T[]>(rows_ * cols_);
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

  [[nodiscard]] constexpr T &get(const vec2<size_t> &pos) {
    if (pos.x() >= cols_ || pos.y() >= rows_) {
      throw std::out_of_range("");
    }
    return arr_[(pos.y() * cols_) + pos.x()];
  }

  [[nodiscard]] constexpr T &get(const size_t x, const size_t y) {
    if (x >= cols_ || y >= rows_) {
      throw std::out_of_range("");
    }
    return arr_[(y * cols_) + x];
  }

  [[nodiscard]] constexpr const T &get(const vec2<size_t> &pos) const {
    if (pos.x() >= cols_ || pos.y() >= rows_) {
      throw std::out_of_range("");
    }
    return arr_[(pos.y() * cols_) + pos.x()];
  }

  [[nodiscard]] constexpr const T &get(u32 x, u32 y) const {
    if (x >= cols_ || y >= rows_) {
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

class Color {
  double r, g, b;

  [[nodiscard]] constexpr static double clampChannel(double channel) noexcept {
    if (channel > 1.)
      return 1.;
    return channel;
  }

  // TODO: Change later to instead take number of bits per each channel (or
  // different for each channel) ColorView does not update automatically with
  // Color, currently a new ColorView needs to be generated for an update Color
  template <typename T>
    requires std::same_as<T, u16> || std::same_as<T, u8>
  class ColorView {
    const Color &color_;

    [[nodiscard]] constexpr T parseChannel(double channel) const noexcept {
      return static_cast<T>(std::round(Color::clampChannel(channel) *
                                       std::numeric_limits<T>::max()));
    }

    T red = parseChannel(color_.r);
    T green = parseChannel(color_.g);
    T blue = parseChannel(color_.b);

  public:
    inline friend std::ostream &operator<<(std::ostream &os, ColorView<T> cv) {
      os << static_cast<u32>(cv.red) << " " << static_cast<u32>(cv.green) << " "
         << static_cast<u32>(cv.blue);
      return os;
    }
    constexpr ColorView(const Color &color) : color_(color) {}
  };

public:
  constexpr Color(double red = 0., double green = 0., double blue = 0.)
      : r(red), g(green), b(blue) {
    r = r < 0. ? 0. : r;
    g = g < 0. ? 0. : g;
    b = b < 0. ? 0. : b;
  }

  [[nodiscard]] constexpr auto getU8View() const noexcept {
    return ColorView<u8>(*this);
  }
  [[nodiscard]] constexpr auto getU16View() const noexcept {
    return ColorView<u16>(*this);
  }

  [[nodiscard]] friend constexpr Color operator*(const Color &c,
                                                 double val) noexcept {
    return Color{c.r * val, c.g * val, c.b * val};
  }
  [[nodiscard]] friend constexpr Color operator*(double val,
                                                 const Color &c) noexcept {
    return Color{c.r * val, c.g * val, c.b * val};
  }
  friend constexpr Color &operator*=(Color &c, double val) noexcept {
    c.r *= val;
    c.g *= val;
    c.b *= val;
    return c;
  }
};

namespace Colors {
constexpr Color Black = Color(0., 0., 0.);
constexpr Color Red = Color(1., 0., 0.);
constexpr Color Green = Color(0., 1., 0.);
constexpr Color Blue = Color(0., 0., 1.);
constexpr Color Yellow = Color(1., 1., 0.);
constexpr Color Cyan = Color(0., 1., 1.);
constexpr Color Purple = Color(1., 0., 1.);
constexpr Color White = Color(1., 1., 1.);
}; // namespace Colors

[[nodiscard]] inline Color randomColor() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_real_distribution<double> realDist(0., 1.);
  static std::uniform_int_distribution<u8> intDist(0, 5);

  u8 variation = intDist(gen);
  double offset = realDist(gen);

  switch (variation) {
  case 0:
    return Color{1., offset, 0.};
  case 1:
    return Color{1. - offset, 1., 0.};
  case 2:
    return Color{0., 1., offset};
  case 3:
    return Color{0., 1. - offset, 1.};
  case 4:
    return Color{offset, 0., 1.};
  default:
    return Color{1., 0, 1. - offset};
  }
}

class Ray {
  vec3<double> origin_;
  vec3<double> direction_;

public:
  // returns the distance from the ray origin to the intersection point, or NaN
  // if the ray doesn't intersect the triangle.
  constexpr Ray(vec3<double> origin, vec3<double> direction) noexcept
      : origin_(origin), direction_(direction.normalise()) {}
  constexpr Ray() noexcept
      : origin_(vec3<double>::zero()), direction_(vec3<double>::NEG_Z()) {}

  [[nodiscard]] constexpr vec3<double> origin() noexcept { return origin_; }
  [[nodiscard]] constexpr vec3<double> direction() noexcept {
    return direction_;
  }
  [[nodiscard]] constexpr const vec3<double> origin() const noexcept {
    return origin_;
  }
  [[nodiscard]] constexpr const vec3<double> direction() const noexcept {
    return direction_;
  }
};

// Note: in right-hand rule coordinate system, triangles points are defined
// counter clockwise
class Triangle {
  using Point = vec3<double>;
  Point point1_;
  Point point2_;
  Point point3_;

public:
  constexpr Triangle(Point point1, Point point2, Point point3) noexcept
      : point1_(point1), point2_(point2), point3_(point3) {}
  constexpr Triangle() noexcept
      : point1_(Point::zero()), point2_(Point::zero()), point3_(Point::zero()) {
  }

  [[nodiscard]] constexpr Point normal() const noexcept {
    const Point vec1 = point2_ - point1_;
    const Point vec2 = point3_ - point1_;
    return crossProduct(vec1, vec2).normalise();
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
  }

  [[nodiscard]] constexpr double
  intersects(const Ray &ray, const vec3<double> &normal) const noexcept {
    const auto rayStep = dotProduct(ray.direction(), normal);
    const auto planeDistance = dotProduct(point1() - ray.origin(), normal);
    double tSteps = planeDistance / rayStep;
    vec3<double> pointPlaneIntersection =
        ray.origin() + tSteps * ray.direction();

    if (dotProduct(normal, crossProduct(point2() - point1(),
                                        pointPlaneIntersection - point1())) < 0)
      return std::numeric_limits<double>::quiet_NaN();
    if (dotProduct(normal, crossProduct(point3() - point2(),
                                        pointPlaneIntersection - point2())) < 0)
      return std::numeric_limits<double>::quiet_NaN();
    if (dotProduct(normal, crossProduct(point1() - point3(),
                                        pointPlaneIntersection - point3())) < 0)
      return std::numeric_limits<double>::quiet_NaN();

    return tSteps;
  }
};

class Sphere {
  vec3<double> position_;
  double radius_;

public:
  Sphere(const vec3<double> &position = vec3<double>::zero(),
         double radius = .1)
      : position_(position), radius_(radius) {}

  [[nodiscard]] double intersects(const Ray &ray) const noexcept {
    const auto rayOriginToCenter = position_ - ray.origin();
    const auto steps = dotProduct(ray.direction(), rayOriginToCenter);
    if (steps < 0) {
      if ((position_ - ray.origin()).lengthSquared() < radius_ * radius_)
        return 0.; // ray origin inside sphere
      return std::numeric_limits<double>::quiet_NaN();
    }
    if ((position_ - steps * ray.direction()).lengthSquared() <
        radius_ * radius_)
      return steps;
    return std::numeric_limits<double>::quiet_NaN();
  }
};

template <typename T>
  requires Numeric<T>
class Matrix3x3 {
  using Matrix = Matrix3x3<T>;
  T arr_[3][3];

public:
  constexpr Matrix3x3() noexcept {}
  constexpr Matrix3x3(T M0x0, T M0x1, T M0x2, T M1x0, T M1x1, T M1x2, T M2x0,
                      T M2x1, T M2x2) noexcept
      : arr_{{M0x0, M0x1, M0x2}, {M1x0, M1x1, M1x2}, {M2x0, M2x1, M2x2}} {}

  [[nodiscard]] constexpr const T &operator[](size_t row, size_t col) const {
    if (row >= 3 || col >= 3)
      throw std::out_of_range("Invalid positions");
    return arr_[row][col];
  }

  [[nodiscard]] static constexpr Matrix3x3<T> zero() noexcept {
    return {
        0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
  }
  [[nodiscard]] static constexpr Matrix3x3<T> one() noexcept {
    return {
        1, 0, 0, 0, 1, 0, 0, 0, 1,
    };
  }

  [[nodiscard]] constexpr Matrix operator*(const Matrix &rhs) const noexcept {
    return {
        arr_[0][0] * rhs[0, 0] + arr_[0][1] * rhs[1, 0] +
            arr_[0][2] * rhs[2, 0],
        arr_[0][0] * rhs[0, 1] + arr_[0][1] * rhs[1, 1] +
            arr_[0][2] * rhs[2, 1],
        arr_[0][0] * rhs[0, 2] + arr_[0][1] * rhs[1, 2] +
            arr_[0][2] * rhs[2, 2],
        arr_[1][0] * rhs[0, 0] + arr_[1][1] * rhs[1, 0] +
            arr_[1][2] * rhs[2, 0],
        arr_[1][0] * rhs[0, 1] + arr_[1][1] * rhs[1, 1] +
            arr_[1][2] * rhs[2, 1],
        arr_[1][0] * rhs[0, 2] + arr_[1][1] * rhs[1, 2] +
            arr_[1][2] * rhs[2, 2],
        arr_[2][0] * rhs[0, 0] + arr_[2][1] * rhs[1, 0] +
            arr_[2][2] * rhs[2, 0],
        arr_[2][0] * rhs[0, 1] + arr_[2][1] * rhs[1, 1] +
            arr_[2][2] * rhs[2, 1],
        arr_[2][0] * rhs[0, 2] + arr_[2][1] * rhs[1, 2] +
            arr_[2][2] * rhs[2, 2],
    };
  }

  [[nodiscard]] friend constexpr vec3<T>
  operator*(const vec3<T> &vec, const Matrix &matrix) noexcept {
    return {vec.x() * matrix[0, 0] + vec.y() * matrix[1, 0] +
                vec.z() * matrix[2, 0],
            vec.x() * matrix[0, 1] + vec.y() * matrix[1, 1] +
                vec.z() * matrix[2, 1],
            vec.x() * matrix[0, 2] + vec.y() * matrix[1, 2] +
                vec.z() * matrix[2, 2]};
  }
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const Matrix3x3<T> &mat) {
  os << std::fixed << std::setprecision(6) << std::right;
  os << std::setw(4) << mat[0, 0] << ", " << std::setw(4) << mat[0, 1] << ", "
     << std::setw(4) << mat[0, 2] << '\n'
     << std::setw(4) << mat[1, 0] << ", " << std::setw(4) << mat[1, 1] << ", "
     << std::setw(4) << mat[1, 2] << '\n'
     << std::setw(4) << mat[2, 0] << ", " << std::setw(4) << mat[2, 1] << ", "
     << std::setw(4) << mat[2, 2];
  return os;
}

class Mesh {
public:
  class ConstTriangleIterator {
    const Mesh *parent_;
    std::vector<vec3<size_t>>::const_iterator itr_;

  public:
    ConstTriangleIterator(const Mesh &parent,
                          std::vector<vec3<size_t>>::const_iterator itr)
        : parent_(&parent), itr_(itr) {}
    [[nodiscard]] const Triangle operator*() const noexcept {
      return parent_->getTriangle(*itr_);
    }
    ConstTriangleIterator &operator++() noexcept {
      ++itr_; 
      return *this;
    }
    bool operator!=(const ConstTriangleIterator &rhs) const noexcept {
      return itr_ != rhs.itr_;
    }
  };

private:
  std::vector<vec3<double>> vertices_;
  std::vector<vec3<size_t>> indicies_;
  std::vector<vec3<double>>
      normals_; // cache normals so they don't get calculated for each pixel
                // while rendering

public:
  Mesh(std::vector<vec3<double>> &&verticies,
       std::vector<vec3<size_t>> indicies)
      : vertices_{std::move(verticies)}, indicies_{std::move(indicies)} {
    for (const auto &vertexIndicies : indicies_) {
      normals_.emplace_back(getTriangle(vertexIndicies).normal());
    }
  }
  ConstTriangleIterator begin() const noexcept {
    return ConstTriangleIterator{*this, indicies_.cbegin()};
  }
  ConstTriangleIterator end() const noexcept {
    return ConstTriangleIterator{*this, indicies_.cend()};
  }
  [[nodiscard]] const auto &indicies() const noexcept { return indicies_; }
  [[nodiscard]] const auto &vertices() const noexcept { return vertices_; }
  [[nodiscard]] const auto &normals() const noexcept { return normals_; }

  [[nodiscard]] Triangle
  getTriangle(const vec3<size_t> &indicies) const noexcept {
    return Triangle{vertices_[indicies.x()], vertices_[indicies.y()],
                    vertices_[indicies.z()]};
  }
};

class Light {
  double intensity_;
  vec3<double> position_;

public:
  Light(double intensity = 10,
        const vec3<double> &position = vec3<double>::zero())
      : intensity_(intensity), position_(position) {}

  [[nodiscard]] auto intensity() const noexcept { return intensity_; }
  [[nodiscard]] auto position() const noexcept { return position_; }
};

class Camera {
  vec3<double> position_;
  Matrix3x3<double> orientation_;

public:
  constexpr Camera(vec3<double> position,
                   Matrix3x3<double> orientation) noexcept
      : position_(position), orientation_(orientation) {}
  constexpr Camera() noexcept
      : position_(vec3<double>::zero()),
        orientation_(Matrix3x3<double>::one()) {}

  [[nodiscard]] constexpr const vec3<double> &position() const noexcept {
    return position_;
  }
  [[nodiscard]] constexpr const Matrix3x3<double> &
  orientation() const noexcept {
    return orientation_;
  }

  constexpr void updatePosition(const vec3<double> &value) noexcept {
    position_ = value;
  }
  constexpr void updateOrientation(const Matrix3x3<double> &value) noexcept {
    orientation_ = value;
  }

  constexpr void truck(double value) noexcept {
    position_ += vec3<double>{value, 0, 0} * orientation_;
  }
  constexpr void pedestal(double value) noexcept {
    position_ += vec3<double>{0, value, 0} * orientation_;
  }
  constexpr void dolly(double value) noexcept {
    position_ += vec3<double>{0, 0, value} * orientation_;
  }

  constexpr void tilt(double degrees) noexcept {
    auto const radians = degrees * (std::numbers::pi / 180.);
    const double SIN = std::sin(radians);
    const double COS = std::cos(radians);
    orientation_ =
        Matrix3x3<double>{1, 0, 0, 0, COS, SIN, 0, -SIN, COS} * orientation_;
  }
  constexpr void pan(double degrees) noexcept {
    auto const radians = degrees * (std::numbers::pi / 180.);
    const double SIN = std::sin(radians);
    const double COS = std::cos(radians);
    orientation_ =
        Matrix3x3<double>{COS, 0, -SIN, 0, 1, 0, SIN, 0, COS} * orientation_;
  }
  constexpr void roll(double degrees) noexcept {
    auto const radians = degrees * (std::numbers::pi / 180.);
    const double SIN = std::sin(radians);
    const double COS = std::cos(radians);
    orientation_ =
        Matrix3x3<double>{COS, SIN, 0, -SIN, COS, 0, 0, 0, 1} * orientation_;
  }

  void takeSnapshot(const std::string &fileName, const vec2<size_t> &resolution,
                    const std::vector<Mesh> &meshes,
                    const std::vector<Light> &lights,
                    const Color &backgroundColor) const {
    std::ofstream image(fileName, std::ios::trunc | std::ios::out);
    if (!image.is_open()) {
      throw std::runtime_error("Could not open " + fileName + " for writing!");
    }
    image << "P3 " << resolution.x() << " " << resolution.y() << " " << 255
          << " ";

    // TODO: research if SIMD can be used here and how to nudge the compiler
    struct PixelInfo {
      Ray ray;
      bool hit;
      Triangle triangle;
      double raySteps;
    };

    const double resolutionWidth = static_cast<double>(resolution.x());
    const double resolutionHeight = static_cast<double>(resolution.y());

    // Table<PixelInfo> pixels(resolution);
    const double aspectRatio = resolutionWidth / resolutionHeight;
    for (size_t y = 0; y < resolution.y(); ++y) {
      for (size_t x = 0; x < resolution.x(); ++x) {
        // get the center of the pixel;
        double world_x = static_cast<double>(x) + .5;
        double world_y = static_cast<double>(y) + .5;

        // convert to Normalised Device Coordinate space
        world_x /= resolutionWidth;
        world_y /= resolutionHeight;

        // convert to Screen space
        world_x = world_x * 2 - 1;
        world_y = 1 - world_y * 2;

        // align to aspect ratio
        world_x *= aspectRatio;
        vec3<double> direction({world_x, world_y, -1.0});
        direction = direction * orientation_;
        Ray ray{position_, direction};
        double steps = std::numeric_limits<double>::infinity();
        Triangle triangle;
        vec3<double> triangleNormal;
        for (size_t j = 0; j < meshes.size(); ++j) {
          for (size_t i = 0; i < meshes[j].indicies().size(); ++i) {
            const auto triangleTmp =
                meshes[j].getTriangle(meshes[j].indicies()[i]);
            const auto stepsTmp =
                triangleTmp.intersects(ray, meshes[j].normals()[i]);
            if (std::isnan(stepsTmp))
              continue;
            if (stepsTmp < steps) {
              steps = stepsTmp;
              triangle = triangleTmp;
              triangleNormal = meshes[j].normals()[i];
            }
          }
        }
// #define RENDER_LIGHT_SOURCES
#ifdef RENDER_LIGHT_SOURCES
        bool lightHit = false;
        for (size_t j = 0; j < lights.size(); ++j) {
          const auto stepsLight = Sphere{lights[j].position()}.intersects(ray);
          if (std::isnan(stepsLight))
            continue;
          if (stepsLight < steps) {
            steps = stepsLight;
            lightHit = true;
          }
        }
#endif // RENDER_LIGHT_SOURCES
        if (steps == std::numeric_limits<double>::infinity()) {
          image << backgroundColor.getU8View() << " ";
        } else {
// #define LIGHT_MULTIPLIER (1./255.)
#define LIGHT_MULTIPLIER 1.
#define CUSTOM_ALBEDO Color{255, 255, 255}
          double pixelLightReached = 0;
#ifdef RENDER_LIGHT_SOURCES
          if (lightHit) {
            image << Colors::Yellow.getU8View() << " ";
            continue;
          }
#endif // RENDER_LIGHT_SOURCES
          for (const auto &light : lights) {
            const auto hitPoint = ray.origin() + steps * ray.direction();
            // const auto hitPointFloatingErrCorrection = hitPoint +
            // ray.direction();
            const auto rayToLight = Ray{hitPoint, light.position() - hitPoint};
            const auto cosineLawFactor = std::max(
                0., dotProduct(rayToLight.direction(), triangleNormal));
            pixelLightReached += cosineLawFactor;
            // if (dotProduct(rayToLight.direction(), triangleNormal) > 0) {
            //   const auto intensityAfterDistance = light.intensity() -
            //   (light.position() - hitPoint).length(); if
            //   (intensityAfterDistance > 0)
            //     pixelLightReached += intensityAfterDistance;
            // }
          }
          // image << Colors::White << " ";
          // image << randomColor() << " ";
          // image << Colors::White *
          //              (1 - dotProduct(triangle.normal(), ray.direction()))
          //       << " ";
          image << (Colors::White * pixelLightReached).getU8View() << " ";
        }
      }
    }
  }
  constexpr void reset() noexcept {
    position_ = vec3<double>::zero();
    orientation_ = Matrix3x3<double>::one();
  }
};

class Scene {
  class Settings {
    Color backgroundColor_;
    struct ImageSettings {
      size_t width;
      size_t height;
    } imageSettings_;

  public:
    Settings(Color backgroundColor, size_t imageWidth, size_t imageHeight)
        : backgroundColor_{backgroundColor},
          imageSettings_(imageWidth, imageHeight) {}
    [[nodiscard]] Color &backgroundColor() noexcept { return backgroundColor_; }
    [[nodiscard]] const Color &backgroundColor() const noexcept {
      return backgroundColor_;
    }
    [[nodiscard]] ImageSettings imageSettings() const noexcept {
      return imageSettings_;
    }
  };

  Settings settings_;
  Camera camera_;
  std::vector<Mesh> meshes_;
  std::vector<Light> lights_;
  std::string fileName_;

  Scene(Settings &&settings, Camera &&camera, std::vector<Mesh> &&meshes,
        std::vector<Light> &&lights, std::string &&fileName)
      : settings_{settings}, camera_{camera}, meshes_{meshes}, lights_(lights),
        fileName_(fileName) {}

public:
  [[nodiscard]] static Scene loadScene(const std::string &filename) {
    FILE *fp = fopen(filename.c_str(), "rb");
    if (!fp) {
      throw std::runtime_error("Failed to open the scene file: " + filename);
    }

    char fileBuffer[65536];
    const auto arrToColorObject = [](const rapidjson::Value &arr) {
      return Color{arr[0].GetDouble(), arr[1].GetDouble(), arr[2].GetDouble()};
    };
    const auto arrToVec3 = [](const rapidjson::Value &arr) {
      return vec3<double>{arr[0].GetDouble(), arr[1].GetDouble(),
                          arr[2].GetDouble()};
    };
    const auto arrToMatrix3x3 = [](const rapidjson::Value &arr) {
      return Matrix3x3<double>{
          arr[0].GetDouble(), arr[1].GetDouble(), arr[2].GetDouble(),
          arr[3].GetDouble(), arr[4].GetDouble(), arr[5].GetDouble(),
          arr[6].GetDouble(), arr[7].GetDouble(), arr[8].GetDouble(),
      };
    };

    rapidjson::FileReadStream is(fp, fileBuffer, sizeof(fileBuffer));
    rapidjson::Document document;
    document.ParseStream(is);
    // TODO: Implement error handling for json parsing
    const rapidjson::Value &bgc = document["settings"]["background_color"];
    Settings settings = {
        arrToColorObject(bgc),
        document["settings"]["image_settings"]["width"].GetUint(),
        document["settings"]["image_settings"]["height"].GetUint()};
    const rapidjson::Value &cameraSettings = document["camera"];
    Camera camera = {arrToVec3(cameraSettings["position"]),
                     arrToMatrix3x3(cameraSettings["matrix"])};
    std::vector<Mesh> meshes;
    const rapidjson::Value &meshesJson = document["objects"];
    std::vector<vec3<double>> vertices;
    std::vector<vec3<size_t>> indicies;
    for (auto &object : meshesJson.GetArray()) {
      if (object["vertices"].Size() % 3 != 0) {
        fclose(fp);
        throw std::runtime_error(
            "Invalid vertices data in scene file: Array root.objects.vertices "
            "length is not a multiple of 3!");
      }
      for (auto itr = object["vertices"].Begin();
           itr != object["vertices"].End();) {
        const double x = itr++->GetDouble();
        const double y = itr++->GetDouble();
        const double z = itr++->GetDouble();
        vertices.emplace_back(x, y, z);
      }
      if (object["triangles"].Size() % 3 != 0) {
        fclose(fp);
        throw std::runtime_error(
            "Invalid triangle vertex indecies data in scene file: Array "
            "root.objects.triangles length is not a multiple of 3!");
      }
      for (auto itr = object["triangles"].Begin();
           itr != object["triangles"].End();) {
        const double x = itr++->GetInt();
        const double y = itr++->GetInt();
        const double z = itr++->GetInt();
        indicies.emplace_back(x, y, z);
      }
      meshes.emplace_back(std::move(vertices), std::move(indicies));
      vertices.clear();
      indicies.clear();
    }
    const rapidjson::Value &lightsJson = document["lights"];
    std::vector<Light> lights;
    double intensity;
    vec3<double> position;
    for (auto &light : lightsJson.GetArray()) {
      if (light["position"].Size() != 3) {
        fclose(fp);
        // TODO: show exact lights array index when implementing error handling
        // for every step of the parsing
        throw std::runtime_error(
            "Invalid crtscene file, root.lights[?].position is not an array of "
            "3 numbers");
      }
      intensity = light["intensity"].GetDouble();
      const auto &positionJson = light["position"];
      position = {positionJson[0].GetDouble(), positionJson[1].GetDouble(),
                  positionJson[2].GetDouble()};
      lights.emplace_back(intensity, position);
    }
    fclose(fp);
    const std::string::size_type startPos = filename[0] == '.' ? 2 : 0;
    Scene scene{
        std::move(settings), std::move(camera), std::move(meshes),
        std::move(lights),
        filename.substr(startPos, filename.find('.', startPos) - startPos)};
    return scene;
  }

  [[nodiscard]] Camera &camera() noexcept { return camera_; }

  void cameraTakeSnapshot() const {
    camera_.takeSnapshot(
        "render.ppm",
        {settings_.imageSettings().width, settings_.imageSettings().height},
        meshes_, settings_.backgroundColor());
  }

  void cameraTakeSnapshot(const std::string &outFileName) const {
    camera_.takeSnapshot(
        outFileName,
        {settings_.imageSettings().width, settings_.imageSettings().height},
        meshes_, settings_.backgroundColor());
  }
};

#endif //__UTILS_CRT