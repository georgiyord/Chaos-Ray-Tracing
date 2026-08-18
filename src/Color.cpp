#include <RenderEngine/Color.hpp>
#include <RenderEngine/ColorView.tpp>
#include <random>
#include <stdexcept>
namespace RenderEngine {
float r, g, b;

[[nodiscard]] float Color::clampChannel(float channel) noexcept {
  return channel > 1.f ? 1.f : channel;
}

 Color::Color(float red, float green, float blue)
    : r(red), g(green), b(blue) {
  if (red < 0.f || green < 0.f || blue < 0.f)
    throw std::runtime_error("Color values cannot be negative");
}

 Color::Color(float grayValue)
    : r(grayValue), g(grayValue), b(grayValue) {
  if (grayValue < 0.f)
    throw std::runtime_error("Color values cannot be negative");
}

[[nodiscard]] ColorView<u8> Color::getU8View() const noexcept {
  return ColorView<u8>(*this);
}
[[nodiscard]] ColorView<u16> Color::getU16View() const noexcept {
  return ColorView<u16>(*this);
}

[[nodiscard]] Color operator*(const Color &c, float val) {
  if (val < 0.f)
    throw std::runtime_error("Color values cannot be negative");
  return Color{c.red() * val, c.green() * val, c.blue() * val};
}
[[nodiscard]] Color operator*(float val, const Color &c) {
  return c * val;
}

 Color &Color::operator*=(float val) {
  if (val < 0.f)
    throw std::runtime_error("Color values cannot be negative");
  r *= val;
  g *= val;
  b *= val;
  return *this;
}

[[nodiscard]] float Color::red() const noexcept { return r; }
[[nodiscard]] float Color::green() const noexcept { return g; }
[[nodiscard]] float Color::blue() const noexcept { return b; }

[[nodiscard]] float &Color::red() noexcept { return r; }
[[nodiscard]] float &Color::green() noexcept { return g; }
[[nodiscard]] float &Color::blue() noexcept { return b; }

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
  static std::uniform_real_distribution<float> realDist(0.f, 1.f);
  static std::uniform_int_distribution<u8> intDist(0, 5);

  u8 variation = intDist(gen);
  float offset = realDist(gen);

  switch (variation) {
  case 0:
    return Color{1.f, offset, 0.f};
  case 1:
    return Color{1.f - offset, 1.f, 0.f};
  case 2:
    return Color{0.f, 1.f, offset};
  case 3:
    return Color{0.f, 1.f - offset, 1.f};
  case 4:
    return Color{offset, 0.f, 1.f};
  default:
    return Color{1.f, 0, 1.f - offset};
  }
}

} // namespace RenderEngine

namespace RenderEngine::Colors {
Color Black = Color(0.f, 0.f, 0.f);
Color Red = Color(1.f, 0.f, 0.f);
Color Green = Color(0.f, 1.f, 0.f);
Color Blue = Color(0.f, 0.f, 1.f);
Color Yellow = Color(1.f, 1.f, 0.f);
Color Cyan = Color(0.f, 1.f, 1.f);
Color Purple = Color(1.f, 0.f, 1.f);
Color White = Color(1.f, 1.f, 1.f);
} // namespace RenderEngine::Colors