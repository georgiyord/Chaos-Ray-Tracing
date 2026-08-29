#ifndef RENDERENGINE_BITMAP_HPP
#define RENDERENGINE_BITMAP_HPP

#include "Color.hpp"

#include <cstddef>
#include <string>

namespace RenderEngine {
class Bitmap {
  size_t width_;
  size_t height_;
  unsigned char *data_;

public:
  Bitmap(std::string);
  Bitmap(Bitmap &&) noexcept;
  Bitmap &operator=(Bitmap &&) noexcept;
  Bitmap(const Bitmap &) = delete;
  Bitmap &operator=(const Bitmap &) = delete;
  ~Bitmap() noexcept;

  [[nodiscard]] Color getColor(float x, float y) const noexcept;
};
} // namespace RenderEngine

#endif // RENDERENGINE_BITMAP_HPP