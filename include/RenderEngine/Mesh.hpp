#ifndef RENDERENGINE_MESH_HPP
#define RENDERENGINE_MESH_HPP

#include "RenderEngine/Ray.hpp"
#include "RenderEngine/Triangle.hpp"
#include "RenderEngine/utils.hpp"
#include <RenderEngine/vec3.hpp>
#include <array>
#include <cstddef>
#include <vector>

namespace RenderEngine {
class Mesh {
private:
  std::vector<vec3> vertices_;
  std::vector<vec3> vertexNormals_;
  std::vector<std::array<size_t, 3>> triangleVertexIndices_;
  std::vector<vec3> triangleNormals_;
  size_t materialId_;

public:
  Mesh(std::vector<vec3> &&vertices,
       std::vector<std::array<size_t, 3>> &&triangleVertexIndices,
       size_t materialId);
  [[nodiscard]] const std::vector<vec3> &vertices() const noexcept;
  [[nodiscard]] const std::vector<vec3> &vertexNormals() const noexcept;
  [[nodiscard]] const std::vector<std::array<size_t, 3>> &
  triangleVertexIndices() const noexcept;
  [[nodiscard]] const std::vector<vec3> &triangleNormals() const noexcept;
  [[nodiscard]] size_t materialId() const noexcept;

  [[nodiscard]] Triangle
  getTriangle(const std::array<size_t, 3> &indices) const noexcept;

  struct IntersectResult {
    double steps = doubleInf;
    vec3 hitPoint = { doubleNaN, doubleNaN, doubleNaN };
    const vec3 *triangleNormal = nullptr;
    size_t materialId = 0xBAAD0000;
    std::array<const vec3 *, 3> vertexPos = {};
    std::array<const vec3 *, 3> vertexNormals = {};
    const Mesh *mesh = nullptr;
  };

  [[nodiscard]] const IntersectResult intersects(const Ray &ray) const noexcept;
  [[nodiscard]] IntersectResult
  intersects(const Ray &ray, double maxDistanceSquared) const noexcept;
  [[nodiscard]] bool intersectsFast(const Ray &ray) const noexcept;
  [[nodiscard]] bool intersectsFast(const Ray &ray,
                                    double maxDistanceSquared) const noexcept;
};
} // namespace RenderEngine

#endif // RENDERENGINE_MESH_HPP