#include <RenderEngine/Triangle.hpp>
#include <RenderEngine/utils.hpp>
namespace RenderEngine {
// Note: in right-hand rule coordinate system, triangles points are defined
// counter clockwise

 Triangle::Triangle(vec3 point1, vec3 point2, vec3 point3) noexcept
      : point1_(point1), point2_(point2), point3_(point3) {}
 Triangle::Triangle() noexcept
      : point1_(vec3::zero()), point2_(vec3::zero()), point3_(vec3::zero()) {}

  [[nodiscard]] vec3 Triangle::calculateNormal() const noexcept {
    const vec3 vec1 = point2_ - point1_;
    const vec3 vec2 = point3_ - point1_;
    return crossProduct(vec1, vec2).normalise();
  }

  [[nodiscard]] double Triangle::getArea() const noexcept {
    const vec3 vec1 = point2_ - point1_;
    const vec3 vec2 = point3_ - point1_;
    return crossProduct(vec1, vec2).length() / 2;
  }

  [[nodiscard]] const vec3& Triangle::point1() const noexcept { return point1_; }
  [[nodiscard]] const vec3& Triangle::point2() const noexcept { return point2_; }
  [[nodiscard]] const vec3& Triangle::point3() const noexcept { return point3_; }

    [[nodiscard]] vec3& Triangle::point1() noexcept { return point1_; }
    [[nodiscard]] vec3& Triangle::point2() noexcept { return point2_; }
    [[nodiscard]] vec3& Triangle::point3() noexcept { return point3_; }

 void Triangle::update(const vec3& p1, const vec3& p2, const vec3& p3) noexcept {
    point1_ = p1;
    point2_ = p2;
    point3_ = p3;
  }

  [[nodiscard]] double Triangle::intersects(const Ray &ray,
                                            const vec3 &normal) const noexcept {
    const auto &NaN = doubleNaN;
    const auto rayStep = dotProduct(ray.direction_, normal);
    if (rayStep == 0)
      return NaN;
    const auto planeDistance = dotProduct(point1() - ray.origin_, normal);
    double tSteps = planeDistance / rayStep;

    if (tSteps < 0)
      return NaN;
    vec3 pointPlaneIntersection = ray.origin_ + tSteps * ray.direction_;

    if (dotProduct(normal, crossProduct(point2() - point1(),
                                        pointPlaneIntersection - point1())) <
        -RENDERENGINE_HITPOINT_BIAS)
      return NaN;
    if (dotProduct(normal, crossProduct(point3() - point2(),
                                        pointPlaneIntersection - point2())) <
        -RENDERENGINE_HITPOINT_BIAS)
      return NaN;
    if (dotProduct(normal, crossProduct(point1() - point3(),
                                        pointPlaneIntersection - point3())) <
        -RENDERENGINE_HITPOINT_BIAS)
      return NaN;

    return tSteps;
  }
} // namespace RenderEngine