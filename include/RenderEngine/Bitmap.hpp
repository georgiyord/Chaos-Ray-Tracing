#ifndef RENDERENGINE_BITMAP_HPP
#define RENDERENGINE_BITMAP_HPP

#include "RenderEngine/Color.hpp"
#include <cstddef>
#include <string>

namespace RenderEngine {
class Bitmap{
    bool valid;
    size_t width_;
    size_t height_;
    unsigned char* data_;
    std::string bitmapPath_;

public:
    Bitmap();
    Bitmap(std::string);
    Bitmap(Bitmap&&) noexcept;
    Bitmap& operator=(Bitmap&&) noexcept;
    Bitmap(const Bitmap&) = delete;
    Bitmap& operator=(const Bitmap&) = delete;
    ~Bitmap() noexcept;

    [[nodiscard]] Color getColor(double x, double y) const noexcept;
};
}

#endif // RENDERENGINE_BITMAP_HPP