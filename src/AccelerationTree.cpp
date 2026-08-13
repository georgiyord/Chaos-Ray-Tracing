#include "RenderEngine/AccelerationTree.hpp"
#include <vector>

namespace RenderEngine {


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

  inline const double &getAxisCoordinate(const vec3 &vec, const Axis &axis) {
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

  inline double &getAxisCoordinate(vec3 &vec, const Axis &axis) {
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

  size_t initHelper(std::vector<AccelerationTreeNode>& nodes, const std::vector<Vertex> &vertices,
                    const std::vector<Triangle> &triangles,
                    std::vector<size_t> &&triangleIds, const AABB &box,
                    size_t currentNodePos, size_t depth,
                    Axis splittingAxis) noexcept {
    if (triangleIds.size() <=
            RENDERENGINE_ACCELERATIONTREE_MINIMUM_TRIANGLES_IN_NODE ||
        depth >= RENDERENGINE_ACCELERATIONTREE_MAXDEPTH) {
      nodes.emplace_back(box, std::move(triangleIds), AccelerationTreeNode::undefinedChildId,
                         AccelerationTreeNode::undefinedChildId);
      std::sort(nodes.back().triangleIds.begin(),
                nodes.back().triangleIds.end());
      return currentNodePos;
    } else {
      // try to split by trying to keep triangles in the children around the same amount
      // ended up with a mess that should probably be rewritten
      nodes.emplace_back(box, std::vector<size_t>{}, AccelerationTreeNode::undefinedChildId,
                         AccelerationTreeNode::undefinedChildId);
      std::vector<size_t> triangleIds1 = std::move(triangleIds);

      // should replace with deque if triangles get too much

      // sorts ids for triangles ascending, by comparing the boundry of the
      // splittingAxis in the positive direction
      std::sort(
          triangleIds1.begin(), triangleIds1.end(),
          [splittingAxis, &triangles, &vertices](size_t id1, size_t id2) {
            const double &pos_triangle1_vertex1 = getAxisCoordinate(
                vertices[triangles[id1].id_vertex1_].position, splittingAxis);
            const double &pos_triangle1_vertex2 = getAxisCoordinate(
                vertices[triangles[id1].id_vertex2_].position, splittingAxis);
            const double &pos_triangle1_vertex3 = getAxisCoordinate(
                vertices[triangles[id1].id_vertex3_].position, splittingAxis);
            const double &pos_triangle2_vertex1 = getAxisCoordinate(
                vertices[triangles[id2].id_vertex1_].position, splittingAxis);
            const double &pos_triangle2_vertex2 = getAxisCoordinate(
                vertices[triangles[id2].id_vertex2_].position, splittingAxis);
            const double &pos_triangle2_vertex3 = getAxisCoordinate(
                vertices[triangles[id2].id_vertex3_].position, splittingAxis);
            const double maxPos1 = std::max(
                pos_triangle1_vertex1,
                std::max(pos_triangle1_vertex2, pos_triangle1_vertex3));
            const double maxPos2 = std::max(
                pos_triangle2_vertex1,
                std::max(pos_triangle2_vertex2, pos_triangle2_vertex3));
            return maxPos1 < maxPos2;
          });
      const auto middleIterator =
          std::next(triangleIds1.cbegin(),
                    static_cast<decltype(triangleIds1)::difference_type>(
                        triangleIds1.size() / 2));
      auto iteratorEnd1 = std::next(middleIterator);
      auto iteratorBegin2 = middleIterator;
      const Triangle &middleTriangle = triangles[*middleIterator];
      const double &middlePoint = std::min(
          getAxisCoordinate(vertices[middleTriangle.id_vertex1_].position,
                            splittingAxis),
          std::min(
              getAxisCoordinate(vertices[middleTriangle.id_vertex2_].position,
                                splittingAxis),
              getAxisCoordinate(vertices[middleTriangle.id_vertex3_].position,
                                splittingAxis)));

      while (iteratorEnd1 != triangleIds1.cend()) {
        const Triangle &tmpTriangle = triangles[*iteratorEnd1];
        if (getAxisCoordinate(vertices[tmpTriangle.id_vertex1_].position,
                              splittingAxis) < middlePoint) {
          ++iteratorEnd1;
          continue;
        }
        if (getAxisCoordinate(vertices[tmpTriangle.id_vertex2_].position,
                              splittingAxis) < middlePoint) {
          ++iteratorEnd1;
          continue;
        }
        if (getAxisCoordinate(vertices[tmpTriangle.id_vertex3_].position,
                              splittingAxis) < middlePoint) {
          ++iteratorEnd1;
          continue;
        }
        break;
      }
      do {
        --iteratorBegin2;
        const Triangle &tmpTriangle = triangles[*iteratorBegin2];
        if (getAxisCoordinate(vertices[tmpTriangle.id_vertex1_].position,
                              splittingAxis) > middlePoint) {
          continue;
        }
        if (getAxisCoordinate(vertices[tmpTriangle.id_vertex2_].position,
                              splittingAxis) > middlePoint) {
          continue;
        }
        if (getAxisCoordinate(vertices[tmpTriangle.id_vertex3_].position,
                              splittingAxis) > middlePoint) {
          continue;
        }
        ++iteratorBegin2;
        break;
      } while (iteratorBegin2 != triangleIds1.cbegin());
      double box1_BoundryEnd = std::max(
          getAxisCoordinate(
              vertices[triangles[*std::prev(iteratorEnd1)].id_vertex1_]
                  .position,
              splittingAxis),
          std::max(getAxisCoordinate(
                       vertices[triangles[*std::prev(iteratorEnd1)].id_vertex2_]
                           .position,
                       splittingAxis),
                   getAxisCoordinate(
                       vertices[triangles[*std::prev(iteratorEnd1)].id_vertex3_]
                           .position,
                       splittingAxis)));
      double box2_BoundryBegin = std::numeric_limits<double>::max();
      for (auto it = iteratorBegin2; it != triangleIds1.cend(); ++it) {
        const Triangle &tri = triangles[*it];
        box2_BoundryBegin = std::min(
            box2_BoundryBegin,
            std::min(
                getAxisCoordinate(vertices[tri.id_vertex1_].position,
                                  splittingAxis),
                std::min(getAxisCoordinate(vertices[tri.id_vertex2_].position,
                                           splittingAxis),
                         getAxisCoordinate(vertices[tri.id_vertex3_].position,
                                           splittingAxis))));
      }
      AABB box1{box};
      AABB box2{box};
      getAxisCoordinate(box1.max, splittingAxis) = box1_BoundryEnd;
      getAxisCoordinate(box2.min, splittingAxis) = box2_BoundryBegin;
      std::vector<size_t> triangleIds2{iteratorBegin2, triangleIds1.cend()};
      triangleIds1.erase(iteratorEnd1, triangleIds1.cend());
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

  std::vector<size_t> intersectsHelper(const std::vector<AccelerationTreeNode>& nodes, const Ray &ray, size_t nodeId) noexcept {
    if (nodeId == AccelerationTreeNode::undefinedChildId){
      return {};
    }
    const auto &node = nodes[nodeId];
    if (!node.box.intersects(ray)) {
      return {};
    }
    if (!node.triangleIds.empty()) {
      return node.triangleIds;
    }
    std::vector<size_t> out;
    std::vector<size_t> child1{intersectsHelper(nodes, ray, node.nodeChild1)};
    std::vector<size_t> child2{intersectsHelper(nodes, ray, node.nodeChild2)};
    out.reserve(child1.size() + child2.size());
    std::set_union(child1.begin(), child1.end(), child2.begin(), child2.end(),
                   std::back_inserter(out));
    return out;
  }
  
  AccelerationTree::AccelerationTree(const std::vector<Vertex> &vertices,
                   const std::vector<Triangle> &triangles,
                   const AABB &box) noexcept {
    std::vector<size_t> triangleIds;
    triangleIds.reserve(triangles.size());
    for (size_t i = 0; i < triangles.size(); ++i) {
      triangleIds.push_back(i);
    }
    initHelper(nodes, vertices, triangles, std::move(triangleIds), box, 0, 1, Axis::X);
  };

  std::vector<size_t> AccelerationTree::intersects(const Ray &ray) const noexcept {
    return intersectsHelper(nodes, ray, 0);
  }
} // namespace RenderEngine