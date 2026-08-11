#include <RenderEngine/Texture.hpp>

namespace RenderEngine {
AlbedoTextureView Texture::asAlbedoTexture() noexcept {
  return {internalName, color1};
}
EdgesTextureView Texture::asEdgesTexture() noexcept {
  return {internalName, color1, color2, extra};
}
CheckerTextureView Texture::asCheckerTexture() noexcept {
  return {internalName, color1, color2, extra};
}
BitmapTextureView Texture::asBitmapTexture() noexcept {
  return {internalName, bitmap};
}
ConstAlbedoTextureView Texture::asAlbedoTexture() const noexcept {
  return {internalName, color1};
}
ConstEdgesTextureView Texture::asEdgesTexture() const noexcept {
  return {internalName, color1, color2, extra};
}
ConstCheckerTextureView Texture::asCheckerTexture() const noexcept {
  return {internalName, color1, color2, extra};
}
ConstBitmapTextureView Texture::asBitmapTexture() const noexcept {
  return {internalName, bitmap};
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

Texture Texture::createAlbedoTexture(std::string name, Color albedo) noexcept {
  return {name, TextureType::ALBEDO, albedo, {}, {}, {}};
}
Texture Texture::createEdgesTexture(std::string name, Color inner_color, Color edge_color, double edge_width) noexcept {
  return {name, TextureType::EDGES, inner_color, edge_color, edge_width, {}};
}
Texture Texture::createCheckerTexture(std::string name, Color color_A, Color color_B, double square_size) noexcept {
  return {name, TextureType::CHECKER, color_A, color_B, square_size, {}};
}
Texture Texture::createBitmapTexture(std::string name, std::string bitmapPath) {
  return {name, TextureType::BITMAP, {}, {}, {}, {bitmapPath}};
}

} // namespace RenderEngine