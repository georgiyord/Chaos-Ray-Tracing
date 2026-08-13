#ifndef RENDERENGINE_VERTEX_HPP
#define RENDERENGINE_VERTEX_HPP

#include <RenderEngine/vec3.hpp>

namespace RenderEngine {

struct Vertex {
  vec3 position;
  vec3 normal;
  vec3 uv;
};
} // namespace RenderEngine


#endif // RENDERENGINE_VERTEX_HPP