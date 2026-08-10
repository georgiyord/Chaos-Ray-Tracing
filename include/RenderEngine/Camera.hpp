#ifndef RENDERENGINE_CAMERA_HPP
#define RENDERENGINE_CAMERA_HPP

#include "RenderEngine/Matrix3x3.hpp"
#include "RenderEngine/vec3.hpp"

namespace RenderEngine {

class Camera {
  vec3 position_;
  Matrix3x3 orientation_;

public:
 Camera(vec3 position, Matrix3x3 orientation) noexcept;
 Camera() noexcept;

  [[nodiscard]] const vec3 &position() const noexcept;
  [[nodiscard]] const Matrix3x3 &orientation() const noexcept;

 void updatePosition(const vec3 &value) noexcept;
 void updateOrientation(const Matrix3x3 &value) noexcept;

 void truck(double value) noexcept;
 void pedestal(double value) noexcept;
 void dolly(double value) noexcept;

 void tilt(double degrees) noexcept;
 void pan(double degrees) noexcept;
 void roll(double degrees) noexcept;
};
} // namespace RenderEngine

#endif // RENDERENGINE_CAMERA_HPP