#ifndef RENDERENGINE_CAMERA_HPP
#define RENDERENGINE_CAMERA_HPP

#include "Matrix3x3.hpp"
#include "vec3.hpp"

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

  void truck(float value) noexcept;
  void pedestal(float value) noexcept;
  void dolly(float value) noexcept;

  void tilt(float degrees) noexcept;
  void pan(float degrees) noexcept;
  void roll(float degrees) noexcept;
};
} // namespace RenderEngine

#endif // RENDERENGINE_CAMERA_HPP