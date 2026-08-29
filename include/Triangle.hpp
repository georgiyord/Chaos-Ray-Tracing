#ifndef RENDERENGINE_TRIANGLE_HPP
#define RENDERENGINE_TRIANGLE_HPP

#include <cstddef>

namespace RenderEngine {
// Note: in right-hand rule coordinate system, triangles points are defined
// counter clockwise
struct Triangle {
  size_t id_vertex1_;
  size_t id_vertex2_;
  size_t id_vertex3_;
  size_t id_mesh_;

  Triangle(size_t, size_t, size_t, size_t) noexcept;
};
} // namespace RenderEngine

#endif // RENDERENGINE_TRIANGLE_HPP