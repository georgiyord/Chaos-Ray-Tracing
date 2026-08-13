#ifndef RENDERENGINE_ACCELERATIONTREE_HPP
#define RENDERENGINE_ACCELERATIONTREE_HPP

#include "RenderEngine/AABB.hpp"
#include "RenderEngine/Triangle.hpp"
#include "RenderEngine/Vertex.hpp"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <limits>
#include <utility>
#include <vector>

#ifndef RENDERENGINE_ACCELERATIONTREE_MAXDEPTH
#define RENDERENGINE_ACCELERATIONTREE_MAXDEPTH 10
#endif // RENDERENGINE_ACCELERATIONTREE_MAXDEPTH

#ifndef RENDERENGINE_ACCELERATIONTREE_MINIMUM_TRIANGLES_IN_NODE
#define RENDERENGINE_ACCELERATIONTREE_MINIMUM_TRIANGLES_IN_NODE 10
#endif // RENDERENGINE_ACCELERATIONTREE_MINIMUM_TRIANGLES_IN_NODE

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
  AccelerationTree(const std::vector<Vertex> &vertices,
                   const std::vector<Triangle> &triangles,
                   const AABB &box) noexcept;

  std::vector<size_t> intersects(const Ray &ray) const noexcept;
};
} // namespace RenderEngine

#endif // RENDERENGINE_ACCELERATIONTREE_HPP