#include "RenderEngine/AABB.hpp"
#include "RenderEngine/utils.hpp"
#include <algorithm>

namespace RenderEngine {

bool AABB::intersects(const Ray &ray) const noexcept {
    float tmin = 0.0, tmax = floatInf;

    float t1 = (min.x_ - ray.origin_.x_) * ray.directionInverse_.x_;
    float t2 = (max.x_ - ray.origin_.x_) * ray.directionInverse_.x_;

    tmin = std::max(tmin, std::min(t1, t2));
    tmax = std::min(tmax, std::max(t1, t2));
    t1 = (min.y_ - ray.origin_.y_) * ray.directionInverse_.y_;
    t2 = (max.y_ - ray.origin_.y_) * ray.directionInverse_.y_;

    tmin = std::max(tmin, std::min(t1, t2));
    tmax = std::min(tmax, std::max(t1, t2));
    t1 = (min.z_ - ray.origin_.z_) * ray.directionInverse_.z_;
    t2 = (max.z_ - ray.origin_.z_) * ray.directionInverse_.z_;

    tmin = std::max(tmin, std::min(t1, t2));
    tmax = std::min(tmax, std::max(t1, t2));

    return tmin < tmax;
}
} // namespace RenderEngine