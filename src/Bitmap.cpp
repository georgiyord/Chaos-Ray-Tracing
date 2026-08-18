#include <RenderEngine/Bitmap.hpp>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <stdexcept>

// TODO: instead of using compiler flags, disable by passing include path via -isystem instead of -I
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
#include <stb/stb_image.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

namespace RenderEngine {
Bitmap::Bitmap(std::string bitmapPath) {
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
}
Bitmap &Bitmap::operator=(Bitmap &&rhs) noexcept {
  if (&rhs != this) {
    width_ = rhs.width_;
    height_ = rhs.height_;
    stbi_image_free(data_);
    data_ = rhs.data_;
    rhs.data_ = nullptr;
  }
  return *this;
}
Bitmap::~Bitmap() noexcept {
  if (data_) {
    stbi_image_free(data_);
  }
}

[[nodiscard]] Color Bitmap::getColor(float x, float y) const noexcept {
  size_t x_ = static_cast<size_t>(x * static_cast<float>(width_)) - 1;
  size_t y_ = static_cast<size_t>(y * static_cast<float>(height_)) - 1;
  unsigned char *offset = data_ + (x_ + y_ * static_cast<size_t>(width_)) * 3;
  float r = offset[0] / 255.f;
  float g = offset[1] / 255.f;
  float b = offset[2] / 255.f;
  return {r, g, b};
}
} // namespace RenderEngine