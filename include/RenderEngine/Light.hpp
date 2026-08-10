#ifndef RENDERENGINE_LIGHT_HPP
#define RENDERENGINE_LIGHT_HPP

#include <RenderEngine/vec3.hpp>

namespace RenderEngine {
class Light {
  double intensity_;
  vec3 position_;

public:
  Light(double intensity = 10, const vec3 &position = vec3::zero())
      : intensity_(intensity), position_(position) {}

  [[nodiscard]] auto intensity() const noexcept { return intensity_; }
  [[nodiscard]] auto position() const noexcept { return position_; }
};
} // namespace RenderEngine

#endif // RENDERENGINE_LIGHT_HPP