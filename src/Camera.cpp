#include <RenderEngine/Camera.hpp>
#include <cmath>
#include <numbers>

namespace RenderEngine {
 Camera::Camera(vec3 position, Matrix3x3 orientation) noexcept : position_(position), orientation_(orientation) {}
 Camera::Camera() noexcept : position_(vec3::zero()), orientation_(Matrix3x3::one()) {}

  [[nodiscard]] const vec3 &Camera::position() const noexcept {
    return position_;
  }
  [[nodiscard]] const Matrix3x3 &Camera::orientation() const noexcept {
    return orientation_;
  }

 void Camera::updatePosition(const vec3 &value) noexcept {
    position_ = value;
  }
 void Camera::updateOrientation(const Matrix3x3 &value) noexcept {
    orientation_ = value;
  }

 void Camera::truck(float value) noexcept {
    position_ += vec3{value, 0, 0} * orientation_;
  }
 void Camera::pedestal(float value) noexcept {
    position_ += vec3{0, value, 0} * orientation_;
  }
 void Camera::dolly(float value) noexcept {
    position_ += vec3{0, 0, value} * orientation_;
  }

 void Camera::tilt(float degrees) noexcept {
    auto const radians = degrees * (std::numbers::pi_v<float> / 180.f);
    const float SIN = std::sin(radians);
    const float COS = std::cos(radians);
    orientation_ = Matrix3x3{{1, 0, 0, 0, COS, SIN, 0, -SIN, COS}} * orientation_;
  }
 void Camera::pan(float degrees) noexcept {
    auto const radians = degrees * (std::numbers::pi_v<float> / 180.f);
    const float SIN = std::sin(radians);
    const float COS = std::cos(radians);
    orientation_ =
        Matrix3x3{{COS, 0, -SIN, 0, 1, 0, SIN, 0, COS}} * orientation_;
  }
 void Camera::roll(float degrees) noexcept {
    auto const radians = degrees * (std::numbers::pi_v<float> / 180.f);
    const float SIN = std::sin(radians);
    const float COS = std::cos(radians);
    orientation_ =
        Matrix3x3{{COS, SIN, 0, -SIN, COS, 0, 0, 0, 1}} * orientation_;
  }
} // namespace RenderEngine