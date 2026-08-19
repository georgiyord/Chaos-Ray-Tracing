
#include <RenderEngine/Ray.hpp>

namespace RenderEngine {
Ray::Ray(vec3 origin, vec3 direction, size_t depthChances) noexcept
    : origin_(origin), direction_(direction.normalise()),
      directionInverse_({1 / direction.x_, 1 / direction.y_, 1 / direction.z_}),
      depthChances_(depthChances) {}
}; // namespace RenderEngine