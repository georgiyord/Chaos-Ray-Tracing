// TODO: Change later to instead take number of bits per each channel (or
// different for each channel)

#ifndef RenderEngine_ColorView_TPP
#define RenderEngine_ColorView_TPP

#include "Color.hpp"
#include "utils.hpp"

#include <cmath>
#include <ostream>

namespace RenderEngine {
template <typename T>
  requires std::same_as<T, u16> || std::same_as<T, u8>
class ColorView {
  const Color &color_;

  [[nodiscard]] static constexpr T parseChannel(float channel) noexcept {
    return static_cast<T>(std::round(Color::clampChannel(channel) *
                                     std::numeric_limits<T>::max()));
  }

public:
  ColorView(const Color &color) : color_(color) {}

  [[nodiscard]] constexpr T red() const noexcept { return parseChannel(color_.red()); }
  [[nodiscard]] constexpr T green() const noexcept { return parseChannel(color_.green()); }
  [[nodiscard]] constexpr T blue() const noexcept { return parseChannel(color_.blue()); }

  [[nodiscard]] inline std::string toString() const {
    // casting to u32 to avoid parsing T=u8 as a char symbol
    return std::to_string(static_cast<u32>(red())) + " " +
           std::to_string(static_cast<u32>(green())) + " " +
           std::to_string(static_cast<u32>(blue()));
  }

  inline friend std::ostream &operator<<(std::ostream &os, const ColorView<T> &cv) {
    os << cv.toString();
    return os;
  }
};
} // namespace RenderEngine

#endif // RenderEngine_ColorView_TPP