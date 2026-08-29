#include "Light.hpp"

namespace RenderEngine {

Light::Light(float intensity, const vec3 &position) noexcept
    : intensity_(intensity), position_(position) {}
} // namespace RenderEngine