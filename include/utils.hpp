// TODO: this header is getting too big and hard to read. Split into multiple
// headers

#ifndef UTILS_CRT_HPP
#define UTILS_CRT_HPP

#define RAPIDJSON_HAS_STDSTRING 1
#define RAPIDJSON_USE_MEMBERSMAP 1

#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <memory>
#include <numbers>
#include <ostream>
#include <random>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/schema.h>

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

template <typename T = double>
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
    double _length = lengthSquared();
    if (_length == 0)
      return *this;
    // TODO: check if floating point errors reduce the effectiveness of a simple
    // == comparison
    if (_length == 1)
      return *this;
    _length = sqrt(_length);
    x_ /= _length;
    y_ /= _length;
    z_ /= _length;
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
    if (red < 0. || green < 0. || blue < 0.)
      throw std::runtime_error("Color values cannot be negative");
  }

  constexpr Color(double grayValue) : r(grayValue), g(grayValue), b(grayValue) {
    if (grayValue < 0.)
      throw std::runtime_error("Color values cannot be negative");
  }

  [[nodiscard]] constexpr auto getU8View() const noexcept {
    return ColorView<u8>(*this);
  }
  [[nodiscard]] constexpr auto getU16View() const noexcept {
    return ColorView<u16>(*this);
  }

  [[nodiscard]] friend constexpr Color operator*(const Color &c, double val) {
    if (val < 0.)
      throw std::runtime_error("Color values cannot be negative");
    return Color{c.r * val, c.g * val, c.b * val};
  }
  [[nodiscard]] friend constexpr Color operator*(double val, const Color &c) {
    if (val < 0.)
      throw std::runtime_error("Color values cannot be negative");
    return Color{c.r * val, c.g * val, c.b * val};
  }
  friend constexpr Color &operator*=(Color &c, double val) {
    if (val < 0.)
      throw std::runtime_error("Color values cannot be negative");
    c.r *= val;
    c.g *= val;
    c.b *= val;
    return c;
  }

  constexpr static Color createGrayscale(double value) {
    if (value < 0.)
      throw std::runtime_error("Color values cannot be negative");
    return Color{value, value, value};
  }

  [[nodiscard]] constexpr double red() const noexcept { return r; }
  [[nodiscard]] constexpr double green() const noexcept { return g; }
  [[nodiscard]] constexpr double blue() const noexcept { return b; }

  [[nodiscard]] static constexpr Color
  elementWiseMultiplication(const Color &lhs, const Color &rhs) {
    return {lhs.r * rhs.r, lhs.g * rhs.g, lhs.b * rhs.b};
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

  [[nodiscard]] constexpr vec3<double> &origin() noexcept { return origin_; }
  [[nodiscard]] constexpr vec3<double> &direction() noexcept {
    return direction_;
  }
  [[nodiscard]] constexpr const vec3<double> &origin() const noexcept {
    return origin_;
  }
  [[nodiscard]] constexpr const vec3<double> &direction() const noexcept {
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
    const auto &NaN = std::numeric_limits<double>::quiet_NaN();
    const auto rayStep = dotProduct(ray.direction(), normal);
    if (rayStep == 0)
      return NaN;
    const auto planeDistance = dotProduct(point1() - ray.origin(), normal);
    double tSteps = planeDistance / rayStep;

    if (tSteps < 0)
      return NaN;
    vec3<double> pointPlaneIntersection =
        ray.origin() + tSteps * ray.direction();

    if (dotProduct(normal, crossProduct(point2() - point1(),
                                        pointPlaneIntersection - point1())) < 0)
      return NaN;
    if (dotProduct(normal, crossProduct(point3() - point2(),
                                        pointPlaneIntersection - point2())) < 0)
      return NaN;
    if (dotProduct(normal, crossProduct(point1() - point3(),
                                        pointPlaneIntersection - point3())) < 0)
      return NaN;

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
  constexpr Matrix3x3() noexcept : arr_{} {}
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

enum class MaterialType { DIFFUSE, REFLECTIVE };
[[nodiscard]] inline MaterialType
getMaterialTypeFromString(const std::string &str) {
  // std::string strUp;
  // strUp.resize(str.size());
  // std::transform(str.begin(), str.end(), strUp.begin(), ::toupper);

  // if (strUp == "DIFFUSE") {
  //   return MaterialType::DIFFUSE;
  // } else if (strUp == "REFLECTIVE") {
  //   return MaterialType::REFLECTIVE;
  // } else [[unlikely]] {
  //   throw std::runtime_error("Invalid Material Type enum string: " + strUp);
  // }

  if (str == "diffuse") {
    return MaterialType::DIFFUSE;
  } else if (str == "reflective") {
    return MaterialType::REFLECTIVE;
  } else [[unlikely]] {
    throw std::runtime_error("Invalid Material Type enum string: " + str +
                             "! This should have been caught by the validator "
                             "but failed during the enum parser?");
  }
}

class Material {
  MaterialType materialType_;
  Color albedo_;
  bool smoothShading_;

public:
  constexpr Material(MaterialType materialType, Color albedo,
                     bool smoothShading)
      : materialType_(materialType), albedo_(albedo),
        smoothShading_(smoothShading) {}
  [[nodiscard]] MaterialType materialType() const noexcept {
    return materialType_;
  }
  [[nodiscard]] const Color &albedo() const noexcept { return albedo_; }
  [[nodiscard]] bool smoothShading() const noexcept { return smoothShading_; }
};

class Mesh {
private:
  std::vector<vec3<double>> vertices_;
  std::vector<vec3<double>> vertexNormals_;
  std::vector<std::array<size_t, 3>> triangleVertexIndices_;
  std::vector<vec3<double>> triangleNormals_;
  const Material *material_;

public:
  Mesh(std::vector<vec3<double>> &&vertices,
       std::vector<std::array<size_t, 3>> &&triangleVertexIndices,
       const Material *material)
      : vertices_{std::move(vertices)},
        triangleVertexIndices_{std::move(triangleVertexIndices)},
        material_{material} {
    vertexNormals_.resize(vertices_.size());
    for (const auto &vertexIndices : triangleVertexIndices_) {
      const auto triangleNormal = getTriangle(vertexIndices).normal();
      triangleNormals_.emplace_back(triangleNormal);
      vertexNormals_[vertexIndices[0]] += triangleNormal;
      vertexNormals_[vertexIndices[1]] += triangleNormal;
      vertexNormals_[vertexIndices[2]] += triangleNormal;
    }
    for (auto &vertexNormal : vertexNormals_) {
      vertexNormal.normalise();
    }
  }
  [[nodiscard]] const auto &vertices() const noexcept { return vertices_; }
  [[nodiscard]] const auto &vertexNormals() const noexcept {
    return vertexNormals_;
  }
  [[nodiscard]] const auto &triangleVertexIndices() const noexcept {
    return triangleVertexIndices_;
  }
  [[nodiscard]] const auto &triangleNormals() const noexcept {
    return triangleNormals_;
  }
  [[nodiscard]] auto material() const noexcept { return material_; }

  [[nodiscard]] Triangle
  getTriangle(const std::array<size_t, 3> &indices) const noexcept {
    return Triangle{vertices_[indices[0]], vertices_[indices[1]],
                    vertices_[indices[2]]};
  }

  struct IntersectResult {
    double steps = std::numeric_limits<double>::infinity();
    const vec3<double> *triangleNormal = nullptr;
    const Material *material = nullptr;
    const vec3<double> *vertexPos1 = nullptr;
    const vec3<double> *vertexPos2 = nullptr;
    const vec3<double> *vertexPos3 = nullptr;
    const vec3<double> *vertexNormal1 = nullptr;
    const vec3<double> *vertexNormal2 = nullptr;
    const vec3<double> *vertexNormal3 = nullptr;
  };

  [[nodiscard]] const IntersectResult
  intersects(const Ray &ray) const noexcept {
    IntersectResult result;
    for (size_t i = 0; i < triangleVertexIndices_.size(); ++i) {
      const auto triangleTmp = getTriangle(triangleVertexIndices_[i]);
      const auto stepsTmp = triangleTmp.intersects(ray, triangleNormals_[i]);
      if (std::isnan(stepsTmp))
        continue;
      if (stepsTmp < result.steps) {
        result = {
            stepsTmp,
            &triangleNormals_[i],
            material_,
            &vertices_[triangleVertexIndices_[i][0]],
            &vertices_[triangleVertexIndices_[i][1]],
            &vertices_[triangleVertexIndices_[i][2]],
            &vertexNormals_[triangleVertexIndices_[i][0]],
            &vertexNormals_[triangleVertexIndices_[i][1]],
            &vertexNormals_[triangleVertexIndices_[i][2]],
        };
      }
    }
    return result;
  }
  [[nodiscard]] IntersectResult
  intersects(const Ray &ray, double maxDistanceSquared) const noexcept {
    IntersectResult result;
    result.steps = std::numeric_limits<double>::infinity();
    for (size_t i = 0; i < triangleVertexIndices_.size(); ++i) {
      const auto triangleTmp = getTriangle(triangleVertexIndices_[i]);
      const auto stepsTmp = triangleTmp.intersects(ray, triangleNormals_[i]);
      if (std::isnan(stepsTmp))
        continue;
      if (stepsTmp * stepsTmp > maxDistanceSquared)
        continue;
      if (stepsTmp < result.steps) {
        result.steps = stepsTmp;
        result.triangleNormal = &triangleNormals_[i];
        result.material = material_;
      }
    }
    return result;
  }
  [[nodiscard]] bool intersectsFast(const Ray &ray) const noexcept {
    IntersectResult result;
    result.steps = std::numeric_limits<double>::infinity();
    for (size_t i = 0; i < triangleVertexIndices_.size(); ++i) {
      const auto triangleTmp = getTriangle(triangleVertexIndices_[i]);
      const auto stepsTmp = triangleTmp.intersects(ray, triangleNormals_[i]);
      if (std::isnan(stepsTmp))
        continue;
      return true;
    }
    return false;
  }
  [[nodiscard]] bool intersectsFast(const Ray &ray,
                                    double maxDistanceSquared) const noexcept {
    IntersectResult result;
    result.steps = std::numeric_limits<double>::infinity();
    for (size_t i = 0; i < triangleVertexIndices_.size(); ++i) {
      const auto triangleTmp = getTriangle(triangleVertexIndices_[i]);
      const auto stepsTmp = triangleTmp.intersects(ray, triangleNormals_[i]);
      if (std::isnan(stepsTmp))
        continue;
      if (stepsTmp * stepsTmp > maxDistanceSquared)
        continue;
      return true;
    }
    return false;
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
        vec3<double> hitPoint;
        // TODO: change this struct type location to global namespace
        Mesh::IntersectResult intersectResult;
        size_t reflectiveDepth = 0;
        Color previousAlbedo = Colors::White;
        bool isReflective = false;
        do {
          for (size_t j = 0; j < meshes.size(); ++j) {
            const auto newIntersectResult = meshes[j].intersects(ray);
            if (newIntersectResult.steps < intersectResult.steps)
              intersectResult = newIntersectResult;
          }
          if (intersectResult.steps == std::numeric_limits<double>::infinity())
            break;
          hitPoint = ray.origin() + intersectResult.steps * ray.direction();
          if (intersectResult.material->materialType() !=
              MaterialType::REFLECTIVE)
            break;
          isReflective = true;
          ray = {hitPoint + *intersectResult.triangleNormal * 1e-4,
                 direction - 2 *
                                 dotProduct(direction,
                                            *intersectResult.triangleNormal) *
                                 *intersectResult.triangleNormal};
          previousAlbedo = Color::elementWiseMultiplication(
              previousAlbedo, intersectResult.material->albedo());
          intersectResult = {};
          ++reflectiveDepth;
        } while (reflectiveDepth < 5);
// #define RENDER_LIGHT_SOURCES
#ifdef RENDER_LIGHT_SOURCES
        bool lightHit = false;
        ray = {position_, direction};
        for (size_t j = 0; j < lights.size(); ++j) {
          const auto stepsLight = Sphere{lights[j].position()}.intersects(ray);
          if (std::isnan(stepsLight))
            continue;
          if (stepsLight < intersectResult.steps) {
            intersectResult.steps = stepsLight;
            lightHit = true;
          }
        }
#endif // RENDER_LIGHT_SOURCES
        if (intersectResult.steps == std::numeric_limits<double>::infinity()) {
          image << (isReflective ? Color::elementWiseMultiplication(
                                       previousAlbedo, backgroundColor)
                                 : backgroundColor)
                       .getU8View()
                << " ";
        } else {
          double pixelLightReached = 0;
#ifdef RENDER_LIGHT_SOURCES
          if (lightHit) {
            image << Colors::Yellow.getU8View() << " ";
            continue;
          }
#endif // RENDER_LIGHT_SOURCES
          vec3<double> pointNormalVector;
          const auto v1p = hitPoint - *intersectResult.vertexPos1;
          const auto v1v2 =
              *intersectResult.vertexPos2 - *intersectResult.vertexPos1;
          const auto v1v3 =
              *intersectResult.vertexPos3 - *intersectResult.vertexPos1;
          const auto triangleArea = crossProduct(v1v2, v1v3).length();
          const auto u = crossProduct(v1p, v1v3).length() / triangleArea;
          const auto v = crossProduct(v1v2, v1p).length() / triangleArea;
          if (intersectResult.material->smoothShading() == true) {
            pointNormalVector = (u * *intersectResult.vertexNormal2 +
                                 v * *intersectResult.vertexNormal3 +
                                 (1 - u - v) * *intersectResult.vertexNormal1)
                                    .normalise();
          } else {
            pointNormalVector = *intersectResult.triangleNormal;
          }
          for (const auto &light : lights) {
            const vec3<double> pointToLightSourceVec =
                light.position() - hitPoint;
            vec3<double> pointToLightSourceVecNormalised =
                pointToLightSourceVec;
            pointToLightSourceVecNormalised.normalise();
            const double triangleSideHit =
                dotProduct(ray.direction(), *intersectResult.triangleNormal) >
                        0.
                    ? -1.
                    : 1.;
            const auto cosineLawFactor =
                std::max(0., dotProduct(pointToLightSourceVecNormalised,
                                        triangleSideHit * pointNormalVector));
            auto tmpLight =
                light.intensity() /
                (4 * std::numbers::pi * pointToLightSourceVec.lengthSquared()) *
                cosineLawFactor;
            // if smooth shading is used, use a bigger bias to prevent sharp
            // edges in phong shading
            const double shadowBias =
                intersectResult.material->smoothShading() ? 1e-2 * 5 : 1e-4;
            Ray shadowRay{hitPoint +
                              triangleSideHit * pointNormalVector * shadowBias,
                          pointToLightSourceVec};
            bool shadowRayIntersection = false;
            for (size_t j = 0; j < meshes.size(); ++j) {
              shadowRayIntersection = meshes[j].intersectsFast(
                  shadowRay, pointToLightSourceVec.lengthSquared());
              if (shadowRayIntersection) {
                tmpLight = 0;
                break;
              }
            }
            pixelLightReached += tmpLight;
          }
          Color albedo = intersectResult.material->albedo();
          if (isReflective) {
            albedo = Color::elementWiseMultiplication(albedo, previousAlbedo);
          }
          const double LIGHT_EXPOSURE = 1;
          image << (albedo * pixelLightReached * LIGHT_EXPOSURE).getU8View()
                << " ";
          // image << (Color{u, v, 0}).getU8View() << " ";
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
    [[nodiscard]] ImageSettings &imageSettings() noexcept {
      return imageSettings_;
    }
    [[nodiscard]] const ImageSettings &imageSettings() const noexcept {
      return imageSettings_;
    }
  };

  Settings settings_;
  Camera camera_;
  std::vector<Mesh> meshes_;
  std::vector<Light> lights_;
  std::vector<Material> materials_;

  Scene(Settings &&settings, Camera &&camera, std::vector<Mesh> &&meshes,
        std::vector<Light> &&lights, std::vector<Material> &&materials)
      : settings_{std::move(settings)}, camera_{std::move(camera)},
        meshes_{std::move(meshes)}, lights_{std::move(lights)},
        materials_{std::move(materials)} {}

public:
  [[nodiscard]] static Scene loadScene(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs.is_open()) {
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

    constexpr char schemaJson[] = {
#embed "../schema.json"
    };
    rapidjson::Document sd;
    sd.Parse(static_cast<const char *>(schemaJson), sizeof(schemaJson));
    rapidjson::SchemaDocument schema(sd);

    rapidjson::IStreamWrapper is(ifs, fileBuffer, sizeof(fileBuffer));
    rapidjson::Document document;
    document.ParseStream(is);
    if (document.HasParseError()) {
      throw std::runtime_error(
          "Failed to parse the scene file " + filename + ": " +
          rapidjson::GetParseError_En(document.GetParseError()));
    }
    if (!document.IsObject()) {
      throw std::runtime_error(
          "Invalid crtscene file: root is not a JSON object");
    }

    rapidjson::SchemaValidator validator(schema);
    // validator.SetValidateFlags(rapidjson::kValidateContinueOnErrorFlag);
    if (!document.Accept(validator)) {
      const char *msg =
          rapidjson::GetValidateError_En(validator.GetInvalidSchemaCode());
      const char *kw = validator.GetInvalidSchemaKeyword();
      throw std::runtime_error("Failed to validate the scene file " + filename +
                               " on " + kw + ": " + msg);
    }

    // verify scene file correctness
    // matrix of camera and vertices indeces
    Settings settings = {
        arrToColorObject(document["settings"]["background_color"]),
        document["settings"]["image_settings"]["width"].GetUint64(),
        document["settings"]["image_settings"]["height"].GetUint64()};

    Camera camera = {arrToVec3(document["camera"]["position"]),
                     arrToMatrix3x3(document["camera"]["matrix"])};

    std::vector<Material> materials;
    for (const auto &material : document["materials"].GetArray()) {
      const auto typeMember = material["type"].GetString();
      const auto albedoMember = material["albedo"].GetArray();
      const auto smoothShadingMember = material["smooth_shading"].GetBool();
      materials.emplace_back(getMaterialTypeFromString(typeMember),
                             arrToColorObject(albedoMember),
                             smoothShadingMember);
    }

    std::vector<Mesh> meshes;
    std::vector<vec3<double>> vertices;
    std::vector<std::array<size_t, 3>> indices;
    for (const auto &object : document["objects"].GetArray()) {
      const auto verticesMember = object["vertices"].GetArray();
      const auto trianglesMember = object["triangles"].GetArray();
      const auto materialIndexMember = object["material_index"].GetUint64();
      if (verticesMember.Size() % 3 != 0) {
        throw std::runtime_error("Invalid crtscene file: root.objects.vertices "
                                 "length is not a multiple of 3!");
      }
      for (auto vertexCoordinateItr = verticesMember.Begin();
           vertexCoordinateItr != verticesMember.End();) {
        const double x = vertexCoordinateItr++->GetDouble();
        const double y = vertexCoordinateItr++->GetDouble();
        const double z = vertexCoordinateItr++->GetDouble();
        vertices.emplace_back(x, y, z);
      }
      if (trianglesMember.Size() % 3 != 0) {
        throw std::runtime_error(
            "Invalid crtscene file: root.objects.triangles length is not a "
            "multiple of 3!");
      }
      for (auto triangleIndexItr = trianglesMember.Begin();
           triangleIndexItr != trianglesMember.End();) {
        const size_t x = triangleIndexItr++->GetUint64();
        const size_t y = triangleIndexItr++->GetUint64();
        const size_t z = triangleIndexItr++->GetUint64();
        indices.push_back({x, y, z});
      }
      meshes.emplace_back(std::move(vertices), std::move(indices),
                          &materials[materialIndexMember]);
      vertices.clear();
      indices.clear();
    }

    std::vector<Light> lights;
    if (document.HasMember("lights")) {
      for (const auto &light : document["lights"].GetArray()) {
        const auto positionMember = light["position"].GetArray();
        const auto intensityMember = light["intensity"].GetDouble();
        lights.emplace_back(intensityMember,
                            vec3<double>{positionMember[0].GetDouble(),
                                         positionMember[1].GetDouble(),
                                         positionMember[2].GetDouble()});
      }
    }

    Scene scene{std::move(settings), std::move(camera), std::move(meshes),
                std::move(lights), std::move(materials)};
    return scene;
  }

  [[nodiscard]] Camera &camera() noexcept { return camera_; }

  void cameraTakeSnapshot(const std::string &outFileName) const {
    camera_.takeSnapshot(
        outFileName,
        {settings_.imageSettings().width, settings_.imageSettings().height},
        meshes_, lights_, settings_.backgroundColor());
  }

  [[nodiscard]] Settings settings() const noexcept { return settings_; }

  void overwriteWidth(size_t width) noexcept {
    settings_.imageSettings().width = width;
  }

  void overwriteHeight(size_t height) noexcept {
    settings_.imageSettings().height = height;
  }

  void overwriteBackgroundColor(const Color &c) noexcept {
    settings_.backgroundColor() = c;
  }

  [[nodiscard]] const Material &getMaterialFromId(size_t id) const noexcept {
    return materials_[id];
  }
};

#endif // UTILS_CRT_HPP