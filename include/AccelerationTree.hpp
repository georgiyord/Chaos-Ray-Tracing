#ifndef RENDERENGINE_ACCELERATIONTREE_HPP
#define RENDERENGINE_ACCELERATIONTREE_HPP

#include "AABB.hpp"
#include "Triangle.hpp"
#include "Vertex.hpp"
#include <cstddef>
#include <limits>
#include <vector>

namespace RenderEngine {
struct AccelerationTreeNode {
  static constexpr auto undefinedChildId = std::numeric_limits<size_t>::max();
  AABB box;
  std::vector<size_t> triangleIds;
  size_t nodeChild1 = undefinedChildId;
  size_t nodeChild2 = undefinedChildId;
};
class AccelerationTree {
  static constexpr auto undefinedChildId = std::numeric_limits<size_t>::max();
  std::vector<AccelerationTreeNode> nodes;

public:
  AccelerationTree(const std::vector<Vertex> &, const std::vector<Triangle> &,
                   const AABB &) noexcept;

  void intersects(const Ray &, std::vector<size_t> &) const noexcept;
};
} // namespace RenderEngine

#endif // RENDERENGINE_ACCELERATIONTREE_HPP