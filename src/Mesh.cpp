#include <RenderEngine/Mesh.hpp>
#include <cmath>

namespace RenderEngine {
Mesh::Mesh(std::vector<vec3> &&vertices,
           std::vector<std::array<size_t, 3>> &&triangleVertexIndices,
           std::vector<vec3> &&uvs, size_t materialId)
    : vertices_{std::move(vertices)},
      triangleVertexIndices_{std::move(triangleVertexIndices)},
      materialId_{materialId}, uvs_{std::move(uvs)} {
  vertexNormals_.resize(vertices_.size());
  for (const auto &vertexIndices : triangleVertexIndices_) {
    const auto triangleNormal = getTriangle(vertexIndices).calculateNormal();
    triangleNormals_.emplace_back(triangleNormal);
    vertexNormals_[vertexIndices[0]] += triangleNormal;
    vertexNormals_[vertexIndices[1]] += triangleNormal;
    vertexNormals_[vertexIndices[2]] += triangleNormal;
  }
  for (auto &vertexNormal : vertexNormals_) {
    vertexNormal.normalise();
  }
}
[[nodiscard]] const std::vector<vec3> &Mesh::vertices() const noexcept {
  return vertices_;
}
[[nodiscard]] const std::vector<vec3> &Mesh::vertexNormals() const noexcept {
  return vertexNormals_;
}
[[nodiscard]] const std::vector<std::array<size_t, 3>> &
Mesh::triangleVertexIndices() const noexcept {
  return triangleVertexIndices_;
}
[[nodiscard]] const std::vector<vec3> &Mesh::triangleNormals() const noexcept {
  return triangleNormals_;
}
[[nodiscard]] size_t Mesh::materialId() const noexcept { return materialId_; }

[[nodiscard]] const std::vector<vec3> &Mesh::uvs() const noexcept {
  return uvs_;
}

[[nodiscard]] Triangle
Mesh::getTriangle(const std::array<size_t, 3> &indices) const noexcept {
  return Triangle{vertices_[indices[0]], vertices_[indices[1]],
                  vertices_[indices[2]]};
}

[[nodiscard]] const Mesh::IntersectResult
Mesh::intersects(const Ray &ray) const noexcept {
  IntersectResult result;
  for (size_t i = 0; i < triangleVertexIndices_.size(); ++i) {
    const auto triangleTmp = getTriangle(triangleVertexIndices_[i]);
    const auto stepsTmp = triangleTmp.intersects(ray, triangleNormals_[i]);
    if (std::isnan(stepsTmp))
      continue;
    if (stepsTmp < result.steps) {
      result = {stepsTmp,
                ray.origin_ + stepsTmp * ray.direction_,
                &triangleNormals_[i],
                materialId_,
                {
                    &vertices_[triangleVertexIndices_[i][0]],
                    &vertices_[triangleVertexIndices_[i][1]],
                    &vertices_[triangleVertexIndices_[i][2]],
                },
                {&vertexNormals_[triangleVertexIndices_[i][0]],
                 &vertexNormals_[triangleVertexIndices_[i][1]],
                 &vertexNormals_[triangleVertexIndices_[i][2]]},
                {&uvs_[triangleVertexIndices_[i][0]],
                 &uvs_[triangleVertexIndices_[i][1]],
                 &uvs_[triangleVertexIndices_[i][2]]},
                this};
    }
  }
  return result;
}
[[nodiscard]] Mesh::IntersectResult
Mesh::intersects(const Ray &ray, double maxDistanceSquared) const noexcept {
  IntersectResult result;
  for (size_t i = 0; i < triangleVertexIndices_.size(); ++i) {
    const auto triangleTmp = getTriangle(triangleVertexIndices_[i]);
    const auto stepsTmp = triangleTmp.intersects(ray, triangleNormals_[i]);
    if (std::isnan(stepsTmp))
      continue;
    if (stepsTmp * stepsTmp > maxDistanceSquared)
      continue;
    if (stepsTmp < result.steps) {
      result = {stepsTmp,
                ray.origin_ + stepsTmp * ray.direction_,
                &triangleNormals_[i],
                materialId_,
                {
                    &vertices_[triangleVertexIndices_[i][0]],
                    &vertices_[triangleVertexIndices_[i][1]],
                    &vertices_[triangleVertexIndices_[i][2]],
                },
                {&vertexNormals_[triangleVertexIndices_[i][0]],
                 &vertexNormals_[triangleVertexIndices_[i][1]],
                 &vertexNormals_[triangleVertexIndices_[i][2]]},
                {&uvs_[triangleVertexIndices_[i][0]],
                 &uvs_[triangleVertexIndices_[i][1]],
                 &uvs_[triangleVertexIndices_[i][2]]},
                this};
    }
  }
  return result;
}
[[nodiscard]] bool Mesh::intersectsFast(const Ray &ray) const noexcept {
  for (size_t i = 0; i < triangleVertexIndices_.size(); ++i) {
    const auto triangleTmp = getTriangle(triangleVertexIndices_[i]);
    const auto stepsTmp = triangleTmp.intersects(ray, triangleNormals_[i]);
    if (std::isnan(stepsTmp))
      continue;
    return true;
  }
  return false;
}
[[nodiscard]] bool
Mesh::intersectsFast(const Ray &ray, double maxDistanceSquared) const noexcept {
  for (size_t i = 0; i < triangleVertexIndices_.size(); ++i) {
    const auto triangleTmp = getTriangle(triangleVertexIndices_[i]);
    const auto stepsTmp = triangleTmp.intersects(ray, triangleNormals_[i]);
    if (std::isnan(stepsTmp))
      continue;
    if (stepsTmp * stepsTmp > maxDistanceSquared)
      continue;
    return true;
  }
  return false;
}
} // namespace RenderEngine