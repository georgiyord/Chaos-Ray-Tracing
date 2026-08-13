#ifndef RENDERENGINE_MESH_HPP
#define RENDERENGINE_MESH_HPP

#include <RenderEngine/vec3.hpp>
#include <cstddef>

namespace RenderEngine {
struct Mesh {
  size_t id_firstTriangle_;
  size_t n_Triangles_;
  size_t id_material_;

  Mesh(size_t id_firstTriangle, size_t n_Triangles,
       size_t id_material) noexcept;
};
} // namespace RenderEngine

#endif // RENDERENGINE_MESH_HPP