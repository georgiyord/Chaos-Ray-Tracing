#include <RenderEngine/Mesh.hpp>

namespace RenderEngine {
Mesh::Mesh(size_t id_firstTriangle, size_t n_Triangles,
           size_t id_material) noexcept
    : id_firstTriangle_(id_firstTriangle), n_Triangles_(n_Triangles),
      id_material_(id_material) {}
} // namespace RenderEngine