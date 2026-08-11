#include <RenderEngine/Bitmap.hpp>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <stdexcept>

// Disable all warnings for vendored stb/image.hpp
#if defined(_MSC_VER)
#pragma warning(push, 0)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wdouble-promotion"
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#include <stb/stb_image.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

namespace RenderEngine {
Bitmap::Bitmap() : valid(false) {}
Bitmap::Bitmap(std::string bitmapPath) : valid(true) {
  int tmpWidth, tmpHeight;
  data_ = stbi_load(bitmapPath.c_str(), &tmpWidth, &tmpHeight, nullptr, 3);
  if (data_ == nullptr) {
    std::string err = stbi_failure_reason();
    if (err == "can't fopen") {
      err += '\n';
      // trigger error again to make sure errno was not overwritten since stbi
      // called fopen
      [[maybe_unused]] auto tmp = fopen(bitmapPath.c_str(), "rb");
      err += std::strerror(errno);
    }
    err += "\n" + bitmapPath;
    throw std::runtime_error(err);
  }
  width_ = static_cast<size_t>(tmpWidth);
  height_ = static_cast<size_t>(tmpHeight);
}
Bitmap::Bitmap(Bitmap &&rhs) noexcept {
  width_ = rhs.width_;
  height_ = rhs.height_;
  data_ = rhs.data_;
  rhs.data_ = nullptr;
  bitmapPath_ = std::move(rhs.bitmapPath_);
}
Bitmap &Bitmap::operator=(Bitmap &&rhs) noexcept {
  if (&rhs != this) {
    width_ = rhs.width_;
    height_ = rhs.height_;
    stbi_image_free(data_);
    data_ = rhs.data_;
    rhs.data_ = nullptr;
    bitmapPath_ = std::move(rhs.bitmapPath_);
  }
  return *this;
}
Bitmap::~Bitmap() noexcept {
  if (data_) {
    stbi_image_free(data_);
  }
}

[[nodiscard]] Color Bitmap::getColor(double x, double y) const noexcept {
  size_t x_ = static_cast<size_t>(x * static_cast<double>(width_));
  size_t y_ = static_cast<size_t>(y * static_cast<double>(height_));
  unsigned char *offset = data_ + (x_ + y_ * static_cast<size_t>(width_)) * 3;
  double r = offset[0] / 255.;
  double g = offset[1] / 255.;
  double b = offset[2] / 255.;
  return {r, g, b};
}
} // namespace RenderEngine