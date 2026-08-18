// TODO: Change later to instead take number of bits per each channel (or
// different for each channel) ColorView does not update automatically with
// Color, currently a new ColorView needs to be generated for an update Color

#ifndef RenderEngine_ColorView_TPP
#define RenderEngine_ColorView_TPP

#include <RenderEngine/utils.hpp>
#include <RenderEngine/Color.hpp>
#include <cmath>
#include <ostream>
namespace RenderEngine {
  template <typename T>
    requires std::same_as<T, u16> || std::same_as<T, u8>
  class ColorView {
    const Color &color_;

    [[nodiscard]] T parseChannel(float channel) const noexcept {
      return static_cast<T>(std::round(Color::clampChannel(channel) *
                                       std::numeric_limits<T>::max()));
    }

    T red_ = parseChannel(color_.red());
    T green_ = parseChannel(color_.green());
    T blue_ = parseChannel(color_.blue());

  public:
 ColorView(const Color &color) : color_(color) {}

    inline friend std::ostream &operator<<(std::ostream &os, ColorView<T> cv) {
      os << static_cast<u32>(cv.red_) << " " << static_cast<u32>(cv.green_)
         << " " << static_cast<u32>(cv.blue_);
      return os;
    }

    [[nodiscard]] std::string toString() const {
      return std::to_string(static_cast<u32>(red_)) + " " +
             std::to_string(static_cast<u32>(green_)) + " " +
             std::to_string(static_cast<u32>(blue_));
    }

    [[nodiscard]] T red() const noexcept { return red_; }
    [[nodiscard]] T green() const noexcept { return green_; }
    [[nodiscard]] T blue() const noexcept { return blue_; }
  };
} // namespace RenderEngine

#endif // RenderEngine_ColorView_TPP