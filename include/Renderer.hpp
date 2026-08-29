#ifndef RENDERENGINE_RENDERER_HPP
#define RENDERENGINE_RENDERER_HPP

#include "Scene.hpp"
#include "ThreadPool.hpp"
#include "utils.hpp"

#include <chrono>

namespace RenderEngine {

struct IntersectResult {
  float steps = floatInf;
  vec3 hitPoint;
  size_t id_triangle;
  size_t id_material;
  size_t id_mesh;
};

class Renderer {

  ThreadPool &threadPool_;
  size_t rayMaxDepth_ = 5;
  size_t n_diffuseReflectionsGI_ = 0;
  const Scene &scene_;

public:
  Renderer(const Scene &) noexcept;
  std::chrono::milliseconds
  takeSnapshot(Color *buffer, RenderMode debugRenderMode = RenderMode::Default,
               size_t raySamplesPerPixelSquareSide = 1) const;
  std::unique_ptr<Color[]> createColorBuffer() const;

  void overwriteMaxRayDepth(size_t) noexcept;
  void overwriteReflectionsGI(size_t) noexcept;

  Color handleDiffuseMaterial(const IntersectResult &, size_t, RenderMode,
                              bool) const noexcept;
  Color handleReflectiveMaterial(const Scene &, const IntersectResult &,
                                 const Ray &, bool) const noexcept;
  Color handleRefractiveMaterial(const Scene &, const IntersectResult &,
                                 const Ray &, bool) const noexcept;
  [[nodiscard]] Color traceRay(const Scene &, const Ray &, RenderMode,
                               bool) const;
  void renderBucket(const Scene &, const size_t, const size_t, Color *const,
                    RenderMode, size_t, size_t) const noexcept;
};
} // namespace RenderEngine

#endif // RENDERENGINE_RENDERER_HPP