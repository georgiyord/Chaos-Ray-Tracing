
#include <RenderEngine/Ray.hpp>

namespace RenderEngine {
Ray::Ray(vec3 origin, vec3 direction, size_t depthChances) noexcept
    : origin_(origin), direction_(direction.normalise()),
      depthChances_(depthChances) {}
}; // namespace RenderEngine