#ifndef RenderEngine_Color_HPP
#define RenderEngine_Color_HPP

#include <RenderEngine/utils.hpp>
#include <concepts>
namespace RenderEngine {

template <typename T>
  requires std::same_as<T, u16> || std::same_as<T, u8>
class ColorView;

class Color {
  double r, g, b;

public:
 Color(double red = 0., double green = 0., double blue = 0.);
 Color(double grayValue);

  [[nodiscard]] ColorView<u8> getU8View() const noexcept;
  [[nodiscard]] ColorView<u16> getU16View() const noexcept;

 Color &operator*=(double val);

  [[nodiscard]] double red() const noexcept;
  [[nodiscard]] double green() const noexcept;
  [[nodiscard]] double blue() const noexcept;

  [[nodiscard]] double &red() noexcept;
  [[nodiscard]] double &green() noexcept;
  [[nodiscard]] double &blue() noexcept;

  [[nodiscard]] static Color
  elementWiseMultiplication(const Color &lhs, const Color &rhs);
  [[nodiscard]] static Color elementWiseAddition(const Color &lhs,
                                                           const Color &rhs);

  [[nodiscard]] static double clampChannel(double channel) noexcept;
};

[[nodiscard]] Color operator*(const Color &c, double val);
[[nodiscard]] Color operator*(double val, const Color &c);

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