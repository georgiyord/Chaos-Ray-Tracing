#include "Texture.hpp"

#include <stdexcept>

namespace RenderEngine {
AlbedoTextureView Texture::asAlbedoTexture() noexcept { return {color1}; }
EdgesTextureView Texture::asEdgesTexture() noexcept {
  return {color1, color2, extra};
}
CheckerTextureView Texture::asCheckerTexture() noexcept {
  return {color1, color2, extra};
}
BitmapTextureView Texture::asBitmapTexture() noexcept { return {id_bitmap}; }
const AlbedoTextureView Texture::asAlbedoTexture() const noexcept {
  return const_cast<Texture *>(this)->asAlbedoTexture();
}
const EdgesTextureView Texture::asEdgesTexture() const noexcept {
  return const_cast<Texture *>(this)->asEdgesTexture();
}
const CheckerTextureView Texture::asCheckerTexture() const noexcept {
  return const_cast<Texture *>(this)->asCheckerTexture();
}
const BitmapTextureView Texture::asBitmapTexture() const noexcept {
  return const_cast<Texture *>(this)->asBitmapTexture();
}

[[nodiscard]] TextureType getTextureTypeFromString(const std::string &str) {
  if (str == "albedo") {
    return TextureType::ALBEDO;
  } else if (str == "edges") {
    return TextureType::EDGES;
  } else if (str == "checker") {
    return TextureType::CHECKER;
  } else if (str == "bitmap") {
    return TextureType::BITMAP;
  } else [[unlikely]] {
    throw std::runtime_error("Invalid Texture Type enum string: " + str +
                             "! This should have been caught by the validator "
                             "but failed during the enum parser?");
  }
}

Texture Texture::createAlbedoTexture(const Color &albedo) noexcept {
  return {TextureType::ALBEDO, albedo, {}, {}, {}};
}
Texture Texture::createEdgesTexture(const Color &inner_color,
                                    const Color &edge_color,
                                    const float edge_width) noexcept {
  return {TextureType::EDGES, inner_color, edge_color, edge_width, {}};
}
Texture Texture::createCheckerTexture(const Color &color_A,
                                      const Color &color_B,
                                      const float square_size) noexcept {
  return {TextureType::CHECKER, color_A, color_B, square_size, {}};
}
Texture Texture::createBitmapTexture(const size_t id_bitmap) {
  return {TextureType::BITMAP, {}, {}, {}, id_bitmap};
}

} // namespace RenderEngine