#ifndef RENDERENGINE_RENDERER_HPP
#define RENDERENGINE_RENDERER_HPP

#include "RenderEngine/Scene.hpp"
#include "RenderEngine/ThreadPool.hpp"
#include "RenderEngine/utils.hpp"

namespace RenderEngine {
class Renderer {

  ThreadPool &threadPool_;
  size_t rayMaxDepth_ = 5;
  const Scene &scene_;

public:
  Renderer(const Scene &) noexcept;
  void takeSnapshot(Color *buffer,
                    RenderMode debugRenderMode = RenderMode::Default) const;
  Color *createColorBuffer() const;

  void overwriteMaxRayDepth(size_t) noexcept;
};
} // namespace RenderEngine

#endif // RENDERENGINE_RENDERER_HPP