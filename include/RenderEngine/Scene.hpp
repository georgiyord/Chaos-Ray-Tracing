#ifndef RENDERENGINE_SCENE_HPP
#define RENDERENGINE_SCENE_HPP

#include "RenderEngine/Camera.hpp"
#include "RenderEngine/Color.hpp"
#include "RenderEngine/Light.hpp"
#include "RenderEngine/Material.hpp"
#include "RenderEngine/Mesh.hpp"
#include "RenderEngine/utils.hpp"
#include "RenderEngine/vec2.hpp"
#include <vector>

#ifndef RENDERENGINE_RAY_MAXDEPTH
#define RENDERENGINE_RAY_MAXDEPTH 5
#endif

namespace RenderEngine {
class Scene {
  struct Settings {
    Color backgroundColor;
    struct ImageSettings {
      size_t width;
      size_t height;
    } imageSettings;
  };

  Settings settings_;
  Camera camera_;
  std::vector<Mesh> meshes_;
  std::vector<Light> lights_;
  std::vector<Material> materials_;

  Scene(Settings &&settings, Camera &&camera, std::vector<Mesh> &&meshes,
        std::vector<Light> &&lights, std::vector<Material> &&materials);

  [[nodiscard]] double traceShadowRay(const vec3 &rayOrigin,
                                      const vec3 &surfaceNormal) const;
  [[nodiscard]] Color traceRay(const Ray &, RenderMode) const;

  [[nodiscard]] Color
  handleDiffuseMaterial(const Mesh::IntersectResult &) const noexcept;
  [[nodiscard]] Color handleReflectiveMaterial(const Mesh::IntersectResult &,
                                               const Ray &) const noexcept;
  [[nodiscard]] Color handleRefractiveMaterial(const Mesh::IntersectResult &,
                                               const Ray &) const noexcept;

  [[nodiscard]] Color goochShade(const Mesh::IntersectResult &) const noexcept;
  [[nodiscard]] static vec3
  interpolateNormal(const vec3 &, const std::array<const vec3 *, 3> &,
                    const std::array<const vec3 *, 3> &) noexcept;
  [[nodiscard]] static vec2
  getBarycentricCoordinates(const vec3 &,
                            const std::array<const vec3 *, 3> &) noexcept;

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

  [[nodiscard]] const Material &getMaterialFromId(size_t id) const noexcept;
};
} // namespace RenderEngine

#endif // RENDERENGINE_SCENE_HPP