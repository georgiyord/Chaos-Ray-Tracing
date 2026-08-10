#ifndef RENDERENGINE_TRIANGLE_HPP
#define RENDERENGINE_TRIANGLE_HPP

#include <RenderEngine/vec3.hpp>
#include <RenderEngine/Ray.hpp>

namespace RenderEngine {
// Note: in right-hand rule coordinate system, triangles points are defined
// counter clockwise
class Triangle {
  vec3 point1_;
  vec3 point2_;
  vec3 point3_;

public:
 Triangle(vec3 point1, vec3 point2, vec3 point3) noexcept;
 Triangle() noexcept;

  [[nodiscard]] vec3 calculateNormal() const noexcept;
  [[nodiscard]] double getArea() const noexcept;

  [[nodiscard]] const vec3 &point1() const noexcept;
  [[nodiscard]] const vec3 &point2() const noexcept;
  [[nodiscard]] const vec3 &point3() const noexcept;

  [[nodiscard]] vec3 &point1() noexcept;
  [[nodiscard]] vec3 &point2() noexcept;
  [[nodiscard]] vec3 &point3() noexcept;

 void update(const vec3 &p1, const vec3 &p2, const vec3 &p3) noexcept;

  [[nodiscard]] double intersects(const Ray &ray,
                                            const vec3 &normal) const noexcept;
};
} // namespace RenderEngine

#endif // RENDERENGINE_TRIANGLE_HPP