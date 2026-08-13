#ifndef RENDERENGINE_SCENE_HPP
#define RENDERENGINE_SCENE_HPP

#include "RenderEngine/AccelerationTree.hpp"
#include "RenderEngine/Bitmap.hpp"
#include "RenderEngine/Camera.hpp"
#include "RenderEngine/Color.hpp"
#include "RenderEngine/Light.hpp"
#include "RenderEngine/Material.hpp"
#include "RenderEngine/Mesh.hpp"
#include "RenderEngine/Texture.hpp"
#include "RenderEngine/Triangle.hpp"
#include "RenderEngine/Vertex.hpp"
#include "RenderEngine/utils.hpp"
#include "RenderEngine/vec2.hpp"
#include <string>
#include <vector>

namespace RenderEngine {
struct Scene {
  struct Settings {
    Color backgroundColor;
    struct ImageSettings {
      size_t width;
      size_t height;
    } imageSettings;
    size_t rayMaxDepth = 5;
  };

  Settings settings_;
  Camera camera_;
  std::vector<Light> lights_;
  std::vector<Bitmap> bitmaps_;
  std::vector<Texture> textures_;
  std::vector<Material> materials_;
  std::vector<Vertex> vertices_;
  std::vector<Triangle> triangles_;
  std::vector<vec3> triangleNormals_;
  std::vector<Mesh> meshes_;
  AccelerationTree accelerationTree_;

  Scene(Settings &&, Camera &&, std::vector<Light> &&, std::vector<Bitmap> &&,
        std::vector<Texture> &&, std::vector<Material> &&,
        std::vector<Vertex> &&, std::vector<Triangle> &&, std::vector<vec3> &&,
        std::vector<Mesh> &&, AccelerationTree&&);

  [[nodiscard]] bool intersectsFast(const Ray &, const size_t,
                                    const double) const noexcept;

  [[nodiscard]] double traceShadowRay(const vec3 &rayOrigin,
                                      const vec3 &surfaceNormal) const;
  [[nodiscard]] Color traceRay(const Ray &, RenderMode) const;

public:
  [[nodiscard]] static Scene loadScene(const std::string &filename);
  [[nodiscard]] Camera &camera() noexcept;

  void
  cameraTakeSnapshot(const std::string &outFileName,
                     RenderMode debugRenderMode = RenderMode::Default) const;

  [[nodiscard]] Settings settings() const noexcept;

  void overwriteWidth(size_t width) noexcept;

  void overwriteHeight(size_t height) noexcept;

  void overwriteBackgroundColor(const Color &c) noexcept;

  void overwriteMaxRayDepth(size_t value) { settings_.rayMaxDepth = value; }

  [[nodiscard]] const Material &getMaterialFromId(size_t id) const noexcept;
};
} // namespace RenderEngine

#endif // RENDERENGINE_SCENE_HPP