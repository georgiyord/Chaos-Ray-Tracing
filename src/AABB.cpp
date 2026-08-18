#include "RenderEngine/AABB.hpp"

namespace RenderEngine {

bool AABB::intersects(const Ray &ray) const noexcept {
  float t;
  t = (min.z_ - ray.origin_.z_) / ray.direction_.z_;
  if (t >= 0) {
    const vec3 point = ray.origin_ + ray.direction_ * t;
    if (point.x_ >= min.x_ && point.x_ <= max.x_ && point.y_ >= min.y_ &&
        point.y_ <= max.y_)
      return true;
  }
  t = (max.z_ - ray.origin_.z_) / ray.direction_.z_;
  if (t >= 0) {
    const vec3 point = ray.origin_ + ray.direction_ * t;
    if (point.x_ >= min.x_ && point.x_ <= max.x_ && point.y_ >= min.y_ &&
        point.y_ <= max.y_)
      return true;
  }
  t = (min.y_ - ray.origin_.y_) / ray.direction_.y_;
  if (t >= 0) {
    const vec3 point = ray.origin_ + ray.direction_ * t;
    if (point.x_ >= min.x_ && point.x_ <= max.x_ && point.z_ >= min.z_ &&
        point.z_ <= max.z_)
      return true;
  }
  t = (max.y_ - ray.origin_.y_) / ray.direction_.y_;
  if (t >= 0) {
    const vec3 point = ray.origin_ + ray.direction_ * t;
    if (point.x_ >= min.x_ && point.x_ <= max.x_ && point.z_ >= min.z_ &&
        point.z_ <= max.z_)
      return true;
  }
  t = (min.x_ - ray.origin_.x_) / ray.direction_.x_;
  if (t >= 0) {
    const vec3 point = ray.origin_ + ray.direction_ * t;
    if (point.y_ >= min.y_ && point.y_ <= max.y_ && point.z_ >= min.z_ &&
        point.z_ <= max.z_)
      return true;
  }
  t = (max.x_ - ray.origin_.x_) / ray.direction_.x_;
  if (t >= 0) {
    const vec3 point = ray.origin_ + ray.direction_ * t;
    if (point.y_ >= min.y_ && point.y_ <= max.y_ && point.z_ >= min.z_ &&
        point.z_ <= max.z_)
      return true;
  }
  return false;
}
} // namespace RenderEngine