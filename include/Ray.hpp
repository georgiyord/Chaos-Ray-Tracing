#ifndef RENDERENGINE_RAY_HPP
#define RENDERENGINE_RAY_HPP
#include "vec3.hpp"
#include <cstddef>

namespace RenderEngine {
struct Ray {
  vec3 origin_;
  vec3 direction_;
  vec3 directionInverse_;
  size_t depthChances_;
  // returns the distance from the ray origin to the intersection point, or NaN
  // if the ray doesn't intersect the triangle.
  Ray(vec3, vec3, size_t = 5) noexcept;
};
}; // namespace RenderEngine

#endif // RENDERENGINE_RAY_HPP