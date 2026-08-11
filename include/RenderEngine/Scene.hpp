#ifndef RENDERENGINE_SCENE_HPP
#define RENDERENGINE_SCENE_HPP

#include "RenderEngine/Camera.hpp"
#include "RenderEngine/Color.hpp"
#include "RenderEngine/Light.hpp"
#include "RenderEngine/Material.hpp"
#include "RenderEngine/Mesh.hpp"
#include "RenderEngine/Texture.hpp"
#include "RenderEngine/utils.hpp"
#include "RenderEngine/vec2.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace RenderEngine {
class Scene {
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
  std::unordered_map<std::string, Texture> textures_;
  std::vector<Material> materials_;
  std::vector<Mesh> meshes_;

  Scene(Settings &&, Camera &&, std::vector<Light> &&,
        std::unordered_map<std::string, Texture> &&, std::vector<Material> &&,
        std::vector<Mesh> &&);

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
  [[nodiscard]] Color
  getTextureColor(const Mesh::IntersectResult &intersectResult) const;

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