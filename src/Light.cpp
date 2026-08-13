#include "RenderEngine/Light.hpp"

namespace RenderEngine {

Light::Light(double intensity, const vec3 &position) noexcept
    : intensity_(intensity), position_(position) {}
} // namespace RenderEngine