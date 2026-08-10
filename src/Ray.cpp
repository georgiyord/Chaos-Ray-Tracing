
#include <RenderEngine/Ray.hpp>

namespace RenderEngine {
Ray::Ray(vec3 origin, vec3 direction, size_t depthChances) noexcept
    : origin_(origin), direction_(direction.normalise()),
      depthChances_(depthChances) {}
Ray::Ray() noexcept
    : origin_(vec3::zero()), direction_(vec3::NEG_Z()), depthChances_(5) {}
}; // namespace RenderEngine