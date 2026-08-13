#ifndef RENDERENGINE_TEXTURE_HPP
#define RENDERENGINE_TEXTURE_HPP

#include "RenderEngine/Color.hpp"
#include <string>
namespace RenderEngine {

enum class TextureType { ALBEDO, EDGES, CHECKER, BITMAP };

struct AlbedoTextureView {
  Color &albedo;
};

struct EdgesTextureView {
  Color &edge_color;
  Color &inner_color;
  double &edge_width;
};

struct CheckerTextureView {
  Color &color_A;
  Color &color_B;
  double &square_size;
};

struct BitmapTextureView {
  size_t &id_bitmap;
};

// clang-format off
// color1, color2 and extra are interpreted differently depending on the texture type
// ALBEDO:      color1: UNUSED;         color2: UNUSED;         extra: UNUSED;      
// EDGES:       color1: edge_color;     color2: inner_color;    extra: edge_width;
// CHECKER:     color1: color_A;        color2: color_B;        extra: square_size;
// BITMAP:      color1: UNUSED;         color2: UNUSED;         extra: UNUSED;
// clang-format on

struct Texture {
  TextureType type;
  Color color1;
  Color color2;
  double extra;
  size_t id_bitmap;

  AlbedoTextureView asAlbedoTexture() noexcept;
  EdgesTextureView asEdgesTexture() noexcept;
  CheckerTextureView asCheckerTexture() noexcept;
  BitmapTextureView asBitmapTexture() noexcept;

  const AlbedoTextureView asAlbedoTexture() const noexcept;
  const EdgesTextureView asEdgesTexture() const noexcept;
  const CheckerTextureView asCheckerTexture() const noexcept;
  const BitmapTextureView asBitmapTexture() const noexcept;

  static Texture createAlbedoTexture(const Color &) noexcept;
  static Texture createEdgesTexture(const Color &, const Color &,
                                    const double) noexcept;
  static Texture createCheckerTexture(const Color &, const Color &,
                                      const double) noexcept;
  static Texture createBitmapTexture(const size_t);
};

[[nodiscard]] TextureType getTextureTypeFromString(const std::string &);

} // namespace RenderEngine

#endif // RENDERENGINE_TEXTURE_HPP