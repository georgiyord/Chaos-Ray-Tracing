#include "RenderEngine/AccelerationTree.hpp"
#include <algorithm>
#include <utility>
#include <vector>

namespace RenderEngine {

#ifndef RENDERENGINE_ACCELERATIONTREE_MAXDEPTH
#define RENDERENGINE_ACCELERATIONTREE_MAXDEPTH 20
#endif // RENDERENGINE_ACCELERATIONTREE_MAXDEPTH

#ifndef RENDERENGINE_ACCELERATIONTREE_MINIMUM_TRIANGLES_IN_NODE
#define RENDERENGINE_ACCELERATIONTREE_MINIMUM_TRIANGLES_IN_NODE 1
#endif // RENDERENGINE_ACCELERATIONTREE_MINIMUM_TRIANGLES_IN_NODE

enum class Axis { X, Y, Z };
constexpr Axis updateAxis(Axis axis) {
  switch (axis) {
  case Axis::X:
    axis = Axis::Y;
    break;
  case Axis::Y:
    axis = Axis::Z;
    break;
  case Axis::Z:
    axis = Axis::X;
    break;
  default:
    std::unreachable();
  }
  return axis;
}

inline const float &getAxisCoordinate(const vec3 &vec, const Axis &axis) {
  switch (axis) {
  case Axis::X:
    return vec.x_;
  case Axis::Y:
    return vec.y_;
  case Axis::Z:
    return vec.z_;
    break;
  default:
    std::unreachable();
  }
}

inline float &getAxisCoordinate(vec3 &vec, const Axis &axis) {
  switch (axis) {
  case Axis::X:
    return vec.x_;
  case Axis::Y:
    return vec.y_;
  case Axis::Z:
    return vec.z_;
    break;
  default:
    std::unreachable();
  }
}

size_t initHelper(std::vector<AccelerationTreeNode> &nodes,
                  const std::vector<Vertex> &vertices,
                  const std::vector<Triangle> &triangles,
                  std::vector<size_t> &&triangleIds, const AABB &box,
                  size_t currentNodePos, size_t depth,
                  Axis splittingAxis) noexcept {
  if (triangleIds.size() <=
          RENDERENGINE_ACCELERATIONTREE_MINIMUM_TRIANGLES_IN_NODE ||
      depth >= RENDERENGINE_ACCELERATIONTREE_MAXDEPTH) {
    nodes.emplace_back(box, std::move(triangleIds),
                       AccelerationTreeNode::undefinedChildId,
                       AccelerationTreeNode::undefinedChildId);
    std::sort(nodes.back().triangleIds.begin(), nodes.back().triangleIds.end());
    return currentNodePos;
  } else {
    // try to split by trying to keep triangles in the children around the same amount
    nodes.emplace_back(box, std::vector<size_t>{},
                       AccelerationTreeNode::undefinedChildId,
                       AccelerationTreeNode::undefinedChildId);
    std::vector<size_t> triangleIds1 = std::move(triangleIds);

    // should replace with deque if triangles get too much

    // sorts ids for triangles ascending, by comparing the boundry of the
    // splittingAxis in the positive direction
    std::sort(
        triangleIds1.begin(), triangleIds1.end(),
        [splittingAxis, &triangles, &vertices](size_t id1, size_t id2) {
          const float &pos_triangle1_vertex1 = getAxisCoordinate(
              vertices[triangles[id1].id_vertex1_].position, splittingAxis);
          const float &pos_triangle1_vertex2 = getAxisCoordinate(
              vertices[triangles[id1].id_vertex2_].position, splittingAxis);
          const float &pos_triangle1_vertex3 = getAxisCoordinate(
              vertices[triangles[id1].id_vertex3_].position, splittingAxis);
          const float &pos_triangle2_vertex1 = getAxisCoordinate(
              vertices[triangles[id2].id_vertex1_].position, splittingAxis);
          const float &pos_triangle2_vertex2 = getAxisCoordinate(
              vertices[triangles[id2].id_vertex2_].position, splittingAxis);
          const float &pos_triangle2_vertex3 = getAxisCoordinate(
              vertices[triangles[id2].id_vertex3_].position, splittingAxis);
          const float maxPos1 =
              std::max(pos_triangle1_vertex1,
                       std::max(pos_triangle1_vertex2, pos_triangle1_vertex3));
          const float maxPos2 =
              std::max(pos_triangle2_vertex1,
                       std::max(pos_triangle2_vertex2, pos_triangle2_vertex3));
          return maxPos1 < maxPos2;
        });
    const auto middleIterator =
        std::next(triangleIds1.cbegin(),
                  static_cast<decltype(triangleIds1)::difference_type>(
                      triangleIds1.size() / 2));
    std::vector<size_t> triangleIds2{middleIterator, triangleIds1.cend()};
    triangleIds1.erase(middleIterator, triangleIds1.cend());
    AABB box1{box};
    AABB box2{box};
    getAxisCoordinate(box1.max, splittingAxis) = std::max(
        getAxisCoordinate(
            vertices[triangles[triangleIds1.back()].id_vertex1_].position,
            splittingAxis),
        std::max(
            getAxisCoordinate(
                vertices[triangles[triangleIds1.back()].id_vertex2_].position,
                splittingAxis),
            getAxisCoordinate(
                vertices[triangles[triangleIds1.back()].id_vertex3_].position,
                splittingAxis)));
    for (const auto id : triangleIds2) {
      float &boundry = getAxisCoordinate(box2.min, splittingAxis);
      boundry = std::min(
          boundry,
          std::min(
              getAxisCoordinate(vertices[triangles[id].id_vertex1_].position,
                                splittingAxis),
              std::min(getAxisCoordinate(
                           vertices[triangles[id].id_vertex2_].position,
                           splittingAxis),
                       getAxisCoordinate(
                           vertices[triangles[id].id_vertex3_].position,
                           splittingAxis))));
    }
    size_t parentIdx = nodes.size() - 1;
    nodes[parentIdx].nodeChild1 = currentNodePos + 1;
    size_t endNode1 =
        initHelper(nodes, vertices, triangles, std::move(triangleIds1), box1,
                   currentNodePos + 1, depth + 1, updateAxis(splittingAxis));
    nodes[parentIdx].nodeChild2 = endNode1 + 1;
    size_t endNode2 =
        initHelper(nodes, vertices, triangles, std::move(triangleIds2), box2,
                   endNode1 + 1, depth + 1, updateAxis(splittingAxis));

    return endNode2;
  }
}

void intersectsHelper(const std::vector<AccelerationTreeNode> &nodes,
                      const Ray &ray, size_t nodeId,
                      std::vector<size_t> &out_triangleIds) noexcept {
  if (nodeId == AccelerationTreeNode::undefinedChildId)
    return;
  const auto &node = nodes[nodeId];
  if (!node.box.intersects(ray))
    return;
  if (!node.triangleIds.empty()) {
    out_triangleIds.insert(out_triangleIds.end(), node.triangleIds.begin(),
                           node.triangleIds.end());
    return;
  }
  intersectsHelper(nodes, ray, node.nodeChild1, out_triangleIds);
  intersectsHelper(nodes, ray, node.nodeChild2, out_triangleIds);
}

AccelerationTree::AccelerationTree(const std::vector<Vertex> &vertices,
                                   const std::vector<Triangle> &triangles,
                                   const AABB &box) noexcept {
  std::vector<size_t> triangleIds;
  triangleIds.reserve(triangles.size());
  for (size_t i = 0; i < triangles.size(); ++i) {
    triangleIds.push_back(i);
  }
  initHelper(nodes, vertices, triangles, std::move(triangleIds), box, 0, 1,
             Axis::X);
};

void AccelerationTree::intersects(
    const Ray &ray, std::vector<size_t> &out_triangleIds) const noexcept {
  return intersectsHelper(nodes, ray, 0, out_triangleIds);
}
} // namespace RenderEngine