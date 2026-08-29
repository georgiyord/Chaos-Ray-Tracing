#ifndef RENDERENGINE_LIGHT_HPP
#define RENDERENGINE_LIGHT_HPP

#include "vec3.hpp"

namespace RenderEngine {
struct Light {
  float intensity_;
  vec3 position_;

  Light(float intensity = 10, const vec3 &position = vec3::zero()) noexcept;
};
} // namespace RenderEngine

#endif // RENDERENGINE_LIGHT_HPP