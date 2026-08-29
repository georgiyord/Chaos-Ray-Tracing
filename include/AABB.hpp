#ifndef RENDERENGINE_AABB_HPP
#define RENDERENGINE_AABB_HPP

#include "Ray.hpp"
#include "vec3.hpp"

namespace RenderEngine {
struct AABB {
  vec3 min;
  vec3 max;

  bool intersects(const Ray &ray) const noexcept;
};
} // namespace RenderEngine

#endif // RENDERENGINE_AABB_HPP