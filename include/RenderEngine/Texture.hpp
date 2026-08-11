#ifndef RENDERENGINE_TEXTURE_HPP
#define RENDERENGINE_TEXTURE_HPP

#include "RenderEngine/Bitmap.hpp"
#include "RenderEngine/Color.hpp"
#include <filesystem>
#include <string>
namespace RenderEngine {

enum class TextureType { ALBEDO, EDGES, CHECKER, BITMAP };

struct AlbedoTextureView {
  std::string &internalName;
  Color &albedo;
};

struct EdgesTextureView {
  std::string &internalName;
  Color &edge_color;
  Color &inner_color;
  double &edge_width;
};

struct CheckerTextureView {
  std::string &internalName;
  Color &color_A;
  Color &color_B;
  double &square_size;
};

struct BitmapTextureView {
  std::string &internalName;
  Bitmap &bitmap;
};

struct ConstAlbedoTextureView {
  const std::string &internalName;
  const Color &albedo;
};

struct ConstEdgesTextureView {
  const std::string &internalName;
  const Color &edge_color;
  const Color &inner_color;
  const double &edge_width;
};

struct ConstCheckerTextureView {
  const std::string &internalName;
  const Color &color_A;
  const Color &color_B;
  const double &square_size;
};

struct ConstBitmapTextureView {
  const std::string &internalName;
  const Bitmap &bitmap;
};

// clang-format off
// color1, color2 and extra are interpreted differently depending on the texture type
// ALBEDO:      color1: UNUSED;         color2: UNUSED;         extra: UNUSED;
// EDGES:       color1: edge_color;     color2: inner_color;    extra: edge_width;
// CHECKER:     color1: color_A;        color2: color_B;        extra: square_size;
// BITMAP:      color1: UNUSED;         color2: UNUSED;         extra: UNUSED;
// clang-format on

struct Texture {
  std::string internalName;
  TextureType type;
  Color color1;
  Color color2;
  double extra;
  Bitmap bitmap;

  AlbedoTextureView asAlbedoTexture() noexcept;
  EdgesTextureView asEdgesTexture() noexcept;
  CheckerTextureView asCheckerTexture() noexcept;
  BitmapTextureView asBitmapTexture() noexcept;

  ConstAlbedoTextureView asAlbedoTexture() const noexcept;
  ConstEdgesTextureView asEdgesTexture() const noexcept;
  ConstCheckerTextureView asCheckerTexture() const noexcept;
  ConstBitmapTextureView asBitmapTexture() const noexcept;

  static Texture createAlbedoTexture(std::string name, Color albedo) noexcept;
  static Texture createEdgesTexture(std::string name, Color inner_color, Color edge_color, double edge_width) noexcept;
  static Texture createCheckerTexture(std::string name, Color color_A, Color color_B, double square_size) noexcept;
  static Texture createBitmapTexture(std::string name, std::string bitmapPath);
};

[[nodiscard]] TextureType getTextureTypeFromString(const std::string &str);

} // namespace RenderEngine

#endif // RENDERENGINE_TEXTURE_HPP