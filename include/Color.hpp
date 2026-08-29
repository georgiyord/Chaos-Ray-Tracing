#ifndef RenderEngine_Color_HPP
#define RenderEngine_Color_HPP

#include "utils.hpp"

#include <concepts>

namespace RenderEngine {

template <typename T>
  requires std::same_as<T, u16> || std::same_as<T, u8>
class ColorView;

class Color {
  float r, g, b;

public:
  Color(float red = 0.f, float green = 0.f, float blue = 0.f);
  Color(float grayValue);

  [[nodiscard]] ColorView<u8> getU8View() const noexcept;
  [[nodiscard]] ColorView<u16> getU16View() const noexcept;

  Color &operator*=(float val);
  [[nodiscard]] bool operator==(Color rhs) const noexcept;
  [[nodiscard]] bool operator!=(Color rhs) const noexcept;
  [[nodiscard]] float red() const noexcept;
  [[nodiscard]] float green() const noexcept;
  [[nodiscard]] float blue() const noexcept;

  [[nodiscard]] float &red() noexcept;
  [[nodiscard]] float &green() noexcept;
  [[nodiscard]] float &blue() noexcept;

  [[nodiscard]] static Color elementWiseMultiplication(const Color &lhs,
                                                       const Color &rhs);
  [[nodiscard]] static Color elementWiseAddition(const Color &lhs,
                                                 const Color &rhs);

  [[nodiscard]] static float clampChannel(float channel) noexcept;
};

[[nodiscard]] Color operator*(const Color &c, float val);
[[nodiscard]] Color operator*(float val, const Color &c);

namespace Colors {
extern Color Black;
extern Color Red;
extern Color Green;
extern Color Blue;
extern Color Yellow;
extern Color Cyan;
extern Color Purple;
extern Color White;
}; // namespace Colors

} // namespace RenderEngine
#endif // RenderEngine_Color_HPP