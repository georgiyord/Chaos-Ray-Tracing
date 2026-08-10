#include <RenderEngine/Color.hpp>
#include <RenderEngine/ColorView.tpp>
#include <random>
#include <stdexcept>
namespace RenderEngine {
double r, g, b;

[[nodiscard]] double Color::clampChannel(double channel) noexcept {
  return channel > 1. ? 1. : channel;
}

 Color::Color(double red, double green, double blue)
    : r(red), g(green), b(blue) {
  if (red < 0. || green < 0. || blue < 0.)
    throw std::runtime_error("Color values cannot be negative");
}

 Color::Color(double grayValue)
    : r(grayValue), g(grayValue), b(grayValue) {
  if (grayValue < 0.)
    throw std::runtime_error("Color values cannot be negative");
}

[[nodiscard]] ColorView<u8> Color::getU8View() const noexcept {
  return ColorView<u8>(*this);
}
[[nodiscard]] ColorView<u16> Color::getU16View() const noexcept {
  return ColorView<u16>(*this);
}

[[nodiscard]] Color operator*(const Color &c, double val) {
  if (val < 0.)
    throw std::runtime_error("Color values cannot be negative");
  return Color{c.red() * val, c.green() * val, c.blue() * val};
}
[[nodiscard]] Color operator*(double val, const Color &c) {
  return c * val;
}

 Color &Color::operator*=(double val) {
  if (val < 0.)
    throw std::runtime_error("Color values cannot be negative");
  r *= val;
  g *= val;
  b *= val;
  return *this;
}

[[nodiscard]] double Color::red() const noexcept { return r; }
[[nodiscard]] double Color::green() const noexcept { return g; }
[[nodiscard]] double Color::blue() const noexcept { return b; }

[[nodiscard]] double &Color::red() noexcept { return r; }
[[nodiscard]] double &Color::green() noexcept { return g; }
[[nodiscard]] double &Color::blue() noexcept { return b; }

[[nodiscard]] Color
Color::elementWiseMultiplication(const Color &lhs, const Color &rhs) {
  return {lhs.r * rhs.r, lhs.g * rhs.g, lhs.b * rhs.b};
}
[[nodiscard]] Color Color::elementWiseAddition(const Color &lhs,
                                                         const Color &rhs) {
  return {lhs.r + rhs.r, lhs.g + rhs.g, lhs.b + rhs.b};
}

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

} // namespace RenderEngine

namespace RenderEngine::Colors {
Color Black = Color(0., 0., 0.);
Color Red = Color(1., 0., 0.);
Color Green = Color(0., 1., 0.);
Color Blue = Color(0., 0., 1.);
Color Yellow = Color(1., 1., 0.);
Color Cyan = Color(0., 1., 1.);
Color Purple = Color(1., 0., 1.);
Color White = Color(1., 1., 1.);
} // namespace RenderEngine::Colors