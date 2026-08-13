#include <RenderEngine/Triangle.hpp>
#include <RenderEngine/utils.hpp>
namespace RenderEngine {
// Note: in right-hand rule coordinate system, triangles points are defined
// counter clockwise

Triangle::Triangle(size_t id_vertex1, size_t id_vertex2, size_t id_vertex3,
                   size_t id_mesh) noexcept
    : id_vertex1_(id_vertex1), id_vertex2_(id_vertex2), id_vertex3_(id_vertex3),
      id_mesh_(id_mesh) {}
} // namespace RenderEngine